/// @source       Transactions.c
/// @description  Simulated POS transactions

// -----------------------------------------------------------------------------

// common configuration options & declarations (always include first)
#include "config.h"

// C language includes
#include <stdio.h>
#include <string.h>

#if PLATFORM(Windows)
#define snprintf _snprintf
#endif

// application includes
#include "Server.h"        /* server_xxx functions */
#include "Server_4.h"      /* main application     */
#include "Database.h"      /* db_xxx functions     */
#include "Operations.h"    /* oper_xxx functions   */
#include "Transactions.h"  /* tran_xxx functions   */

// -----------------------------------------------------------------------------

// global declarations

// for identification purposes in the log file
static const char sourceID[] = "TRA";

// transaction processing functions
static Message* tranFunc_0000(const Message* in);
static Message* tranFunc_0010(const Message* in);
static Message* tranFunc_0020(const Message* in);
static Message* tranFunc_0030(const Message* in);
static Message* tranFunc_0040(const Message* in);
static Message* tranFunc_0050(const Message* in);

// the transaction table
static Transaction transactions[] =
{
   { "0000", tranFunc_0000 } , // terminal is switched on
   { "0010", tranFunc_0010 } , // supervisor authorizes terminal operation
   { "0020", tranFunc_0020 } , // operator begins work shift
   { "0030", tranFunc_0030 } , // operator performs business transactions
   { "0040", tranFunc_0040 } , // operator ends work shift
   { "0050", tranFunc_0050 } , // supervisor closes terminal
   { 0, 0 }
};

// helper functions
static bool checkTerminalConnectionID(short, uint, ushort);
static Message* getReplyMessageTo(const Message*, cchar*, ushort);
static short getTerminalIndex(cchar*);
static void setErrCode(Message*, ushort);
static void setTranCode(Message*, cchar*);

// -----------------------------------------------------------------------------
/// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

// finds the processing function associated with a specific transaction

TranFunc* tran_findFunc(cchar* tranCode)
{
   Transaction* transaction = &transactions[0];

   while (transaction)
      if (!memcmp(tranCode, transaction->code, TRAN_CODE_LEN))
         return transaction->tranFunc;
      else
         ++transaction;

   // not found
   return NULL;
}

// -----------------------------------------------------------------------------
// TRANSACTION 0000
// -----------------------------------------------------------------------------

static Message* tranFunc_0000(const Message* msgIn)
{
   short termIndex;
   static cchar tranCode[] = "0000";

   const Message_0000* msg_0000 =
      (const Message_0000*)server_messageBuffer((Message*)msgIn);

   // checks the (hardware) terminal serial number
   if (!db_findSerialNumber(msg_0000->serialNumber))
   {
      printError("serial number %.*s unknown, discarding", SER_NUM_LEN,
         msg_0000->serialNumber);
      return NULL;
   }

   // checks the (software) terminal id
   if (!(db_findTerminalID(msg_0000->terminalID)))
   {
      printError("terminal id %.*s unknown, discarding", TERM_ID_LEN,
         msg_0000->terminalID);
      return NULL;
   }

   // checks if terminal in terminal table
   termIndex = db_findTerminal(msg_0000->terminalID);

   // if not in terminal table, inserts it
   if (termIndex == -1)
   {
      db_insertTerminal(msg_0000->serialNumber, msg_0000->terminalID,
         server_messageIPaddress(msgIn), server_messageConnSeqNo(msgIn));
      return getReplyMessageTo(msgIn, tranCode, sizeof(Reply_0000)); //******
   }

   // terminal already in terminal table, probably was switched off and then on
   // again

   if (!db_checkTerminalSerialNumber(termIndex, msg_0000->serialNumber))
   {
      printError("invalid serial number %.*s, discarding", SER_NUM_LEN,
         msg_0000->serialNumber);
      return NULL;
   }

   if (db_terminalState(termIndex) == TRM_CLOSED)
   {
      printError("terminal id %.*s CLOSED", TERM_ID_LEN,
         msg_0000->terminalID);
      return NULL;
   }

   // ok, now we can update the terminal table entry

   db_updateTerminalConnectionID(termIndex, server_messageIPaddress(msgIn),
      server_messageConnSeqNo(msgIn));

   printWarn("terminal id %.*s already in terminal table, state:%d ok",
      TERM_ID_LEN, msg_0000->terminalID, db_terminalState(termIndex));

   // reply to be sent to the terminal
   return getReplyMessageTo(msgIn, tranCode, sizeof(Reply_0000));
}

// -----------------------------------------------------------------------------
// TRANSACTION 0010
// -----------------------------------------------------------------------------

static Message* tranFunc_0010(const Message* msgIn)
{
   short termIndex;
   Message* msgOut = NULL;
   static cchar tranCode[] = "0010";
   const Message_0010* msg_0010 =
      (Message_0010*)server_messageBuffer((Message*)msgIn);

   // checks if the originating terminal is in terminal table
   if ((termIndex=getTerminalIndex(msg_0010->terminalID)) == -1)
      return NULL;

   // checks the connection information
   if (!checkTerminalConnectionID(termIndex, server_messageIPaddress(msgIn),
         server_messageConnSeqNo(msgIn)))
      return NULL;

   // prepares the reply to the terminal
   msgOut = getReplyMessageTo(msgIn, tranCode, sizeof(Reply_0010));

   // checks state of terminal
   if (db_terminalState(termIndex) != TRM_RECOGNIZED)
   {
      printError("inv state %d term id %.*s", db_terminalState(termIndex),
         TERM_ID_LEN, msg_0010->terminalID);
      setErrCode(msgOut, E_INV_TERM_STATE);
      return msgOut;
   }

   // checks if the supervisor code received is valid
   if (!db_findSupervisor(msg_0010->supervisor))
   {
      printError("supervisor code %.*s unknown", EMPLOYEE_LEN,
         msg_0010->supervisor);
      setErrCode(msgOut, E_INV_SUP);
   }
   else
   {
      db_setTerminalState(termIndex, TRM_AUTHORIZED);
      db_setTerminalSupervisor(termIndex, msg_0010->supervisor);
   }

   return msgOut;
}

// -----------------------------------------------------------------------------
// TRANSACTION 0020
// -----------------------------------------------------------------------------

static Message* tranFunc_0020(const Message* msgIn)
{
   short termIndex;
   Message* msgOut = NULL;
   static cchar tranCode[] = "0020";

   const Message_0020* msg_0020 =
      (Message_0020*)server_messageBuffer((Message*)msgIn);

   // checks if the originating terminal is in terminal table
   if ((termIndex=getTerminalIndex(msg_0020->terminalID)) == -1)
      return NULL;

   // checks the connection information
   if (!checkTerminalConnectionID(termIndex, server_messageIPaddress(msgIn),
         server_messageConnSeqNo(msgIn)))
      return NULL;

   // prepares the reply to the terminal
   msgOut = getReplyMessageTo(msgIn, tranCode, sizeof(Reply_0020));

   // checks state of terminal
   if (db_terminalState(termIndex) != TRM_AUTHORIZED)
   {
      printError("inv state %d term id %.*s", db_terminalState(termIndex),
         TERM_ID_LEN, msg_0020->terminalID);
      setErrCode(msgOut, E_INV_TERM_STATE);
      return msgOut;
   }

   // checks if the operator code received is valid
   if (!db_findOperator(msg_0020->termOperator))
   {
      printError("operator code %.*s unknown", EMPLOYEE_LEN,
         msg_0020->termOperator);
      setErrCode(msgOut, E_INV_OP);
   }
   else
   {
      db_setTerminalState(termIndex, TRM_OPERATING);
      db_setTerminalOperator(termIndex, msg_0020->termOperator);
   }

   return msgOut;
}

// -----------------------------------------------------------------------------
// TRANSACTION 0030
// -----------------------------------------------------------------------------

static Message* tranFunc_0030(const Message* msgIn)
{
   short termIndex;
   OpFunc* opFunc;
   Message* msgOut = NULL;
   static cchar tranCode[] = "0030";
   const Message_0030* msg_0030 =
      (Message_0030*)server_messageBuffer((Message*)msgIn);

   // checks if the originating terminal is in terminal table
   if ((termIndex=getTerminalIndex(msg_0030->terminalID)) == -1)
      return NULL;

   // checks the connection information
   if (!checkTerminalConnectionID(termIndex, server_messageIPaddress(msgIn),
         server_messageConnSeqNo(msgIn)))
      return NULL;

   // prepares the reply to the terminal
   msgOut = getReplyMessageTo(msgIn, tranCode, sizeof(Reply_0030));

   // checks state of terminal
   if (db_terminalState(termIndex) != TRM_OPERATING)
   {
      printError("inv state %d term id %.*s", db_terminalState(termIndex),
         TERM_ID_LEN, msg_0030->terminalID);
      setErrCode(msgOut, E_INV_TERM_STATE);
      return msgOut;
   }

   // checks if the operator code received is valid
   if (!db_findOperator(msg_0030->termOperator))
   {
      printError("operator code %.*s unknown", EMPLOYEE_LEN,
         msg_0030->termOperator);
      setErrCode(msgOut, E_INV_OP);
      return msgOut;
   }

   // finds the operation processing function
   opFunc = oper_findFunc(msg_0030->operationCode);
   if (!opFunc)
   {
      printError("operation code %.*s unknown", OP_CODE_LEN,
         msg_0030->operationCode);
      setErrCode(msgOut, E_INV_OPCODE);
      return msgOut;
   }

   // now does the operation processing
   opFunc(msgIn, msgOut, termIndex);

   return msgOut;
}

// -----------------------------------------------------------------------------
// TRANSACTION 0040
// -----------------------------------------------------------------------------

static Message* tranFunc_0040(const Message* msgIn)
{
   short termIndex;
   Message* msgOut = NULL;
   static cchar tranCode[] = "0040";

   const Message_0040* msg_0040 =
      (Message_0040*)server_messageBuffer((Message*)msgIn);

   // checks if the originating terminal is in terminal table
   if ((termIndex=getTerminalIndex(msg_0040->terminalID)) == -1)
      return NULL;

   // checks the connection information
   if (!checkTerminalConnectionID(termIndex, server_messageIPaddress(msgIn),
         server_messageConnSeqNo(msgIn)))
      return NULL;

   // prepares the reply to the terminal
   msgOut = getReplyMessageTo(msgIn, tranCode, sizeof(Reply_0040));

   // checks state of terminal
   if (db_terminalState(termIndex) != TRM_OPERATING)
   {
      printError("inv state %d term id %.*s", db_terminalState(termIndex),
         TERM_ID_LEN, msg_0040->terminalID);
      setErrCode(msgOut, E_INV_TERM_STATE);
      return msgOut;
   }

   // checks if the operator code received is valid
   if (!db_checkTerminalOperator(termIndex, msg_0040->termOperator))
   {
      printError("operator code %.*s invalid", EMPLOYEE_LEN,
         msg_0040->termOperator);
      setErrCode(msgOut, E_INV_OP);
   }
   else
      db_setTerminalState(termIndex, TRM_AUTHORIZED);

   return msgOut;
}

// -----------------------------------------------------------------------------
// TRANSACTION 0050
// -----------------------------------------------------------------------------

static Message* tranFunc_0050(const Message* msgIn)
{
   short termIndex;
   Message* msgOut = NULL;
   static cchar tranCode[] = "0050";

   const Message_0050* msg_0050 =
      (Message_0050*)server_messageBuffer((Message*)msgIn);

   // checks if the originating terminal is in terminal table
   if ((termIndex=getTerminalIndex(msg_0050->terminalID)) == -1)
      return NULL;

   // checks the connection information
   if (!checkTerminalConnectionID(termIndex, server_messageIPaddress(msgIn),
         server_messageConnSeqNo(msgIn)))
      return NULL;

   // prepares the reply to the terminal
   msgOut = getReplyMessageTo(msgIn, tranCode, sizeof(Reply_0050));

   // checks state of terminal
   if (db_terminalState(termIndex) != TRM_AUTHORIZED)
   {
      printError("inv state %d term id %.*s", db_terminalState(termIndex),
         TERM_ID_LEN, msg_0050->terminalID);
      setErrCode(msgOut, E_INV_TERM_STATE);
      return msgOut;
   }

   // checks if the supervisor code received is valid
   if (!db_findSupervisor(msg_0050->supervisor))
   {
      printError("supervisor code %.*s unknown", EMPLOYEE_LEN,
         msg_0050->supervisor);
      setErrCode(msgOut, E_INV_SUP);
   }
   else
      db_setTerminalState(termIndex, TRM_CLOSED);

   return msgOut;
}

// -----------------------------------------------------------------------------
// HELPER FUNCTIONS
// -----------------------------------------------------------------------------

// checks if connection received in a message is valid

static bool checkTerminalConnectionID(short termIndex, uint ipAddr,
   ushort connectionSeqNo)
{
   if (!db_checkTerminalConnectionID(termIndex, ipAddr, connectionSeqNo))
   {
      printError("invalid connection id %08X %d, discarding message",
         ipAddr, connectionSeqNo);
      return false;
   }

   return true;
}

// -----------------------------------------------------------------------------

// requests an output message performs its initialization

static Message* getReplyMessageTo(const Message* msgIn, cchar* tranCode,
   ushort replyLen)
{
   Message* msgOut = server_getFreeMessage();

   // requests that this reply be sent to the originating terminal
   server_copyConnectionFromMessage(msgOut, msgIn);

   setErrCode(msgOut, E_OK);
   setTranCode(msgOut, tranCode);

   server_setMessageSize(msgOut, replyLen);

   return msgOut;
}

// -----------------------------------------------------------------------------

// retrieves the terminal index associated with a terminal id

static short getTerminalIndex(cchar* terminalID)
{
   short termIndex = db_findTerminal(terminalID);

   if (termIndex == -1)
      printError("terminal id %.*s not found in terminal table, discarding",
         TERM_ID_LEN, terminalID);

   return termIndex;
}

// -----------------------------------------------------------------------------

// sets the error code field in a (reply) message

static void setErrCode(Message* msg, ushort errCode)
{
   char err[ERR_CODE_LEN+1];
   char* buf = server_messageBuffer(msg);
   ReplyHeader* rh = (ReplyHeader*)buf;

   snprintf(err, sizeof(err), "%.*d", ERR_CODE_LEN, errCode);
   memcpy(rh->errCode, err, ERR_CODE_LEN);
}

// -----------------------------------------------------------------------------

// sets the transaction code field in a (reply) message

static void setTranCode(Message* msg, cchar* code)
{
   char* buf = server_messageBuffer(msg);
   ReplyHeader* rh = (ReplyHeader*)buf;
   memcpy(rh->tranCode,code, TRAN_CODE_LEN);
}

// -----------------------------------------------------------------------------
// the end
