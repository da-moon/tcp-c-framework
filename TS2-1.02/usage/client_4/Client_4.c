/// @source       Client_4.c
/// @description  A client used for the purposes of testing the server framework.

// -----------------------------------------------------------------------------

/** A client application that simulates a simplistic POS server.

    The client functions are not really part of the framework, but are provided
    as a convenience for testing the framework.

   Test with Server_4.
*/

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations (always include first)
#include "config.h"

// C language includes
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h> // srand
#include <string.h>

// application includes
#include "Client.h"       /* client_xxx framework functions  */
#include "Client_4.h"     /* main application include        */
#include "Database.h"     /* db_xxx functions                */
#include "Transactions.h" /* transactions layouts            */

// -----------------------------------------------------------------------------

// global declarations

// for identification purposes in the log file
static cchar sourceID[] = "CLI";

// control variables
static bool beginShift;    // begins shift or closes terminal ?
static bool sendOperation; // sends operation or ends shift ?

// prototypes
static bool checkConnection(void);
static bool checkReplyHeader(Message*, cchar*e, ushort);
static void createMessage(Message*);
static void createMessageHeader(Message*, cchar*);
static void createMsg_0000(Message*);
static void createMsg_0010(Message*);
static void createMsg_0020(Message*);
static void createMsg_0030(Message*);
static void createMsg_0040(Message*);
static void createMsg_0050(Message*);
static cchar* format(cchar*, va_list);
static void processReply(Message*);
static void processReply_0000(Message*);
static void processReply_0010(Message*);
static void processReply_0020(Message*);
static void processReply_0030(Message*);
static void processReply_0040(Message*);
static void processReply_0050(Message*);
static uint recvReply(Message*);
static bool sendMessage(Message*);
static void synchronizeWithServer(Message*);
static void wait(void);

// -----------------------------------------------------------------------------

// the main program

int main(void)
{
   Message *msg, *replyMsg;

   // how long to wait for server replies
   client_setReplyTimeout(5);

   // the port on which ther server waits for connection requests
   client_setServicePort(SERVER_PORT);

   // the IP address on which the server is running
   client_setServerAddress(SERVER_ADDR);

   // how much detail is written to the log file
   client_setLogLevel(LOG_LEVEL_DEBUG);

   // does whatever initialization the framework needs
   client_init();

   // ------------------------

   // reads configuration and restores state of terminal
   db_init();

   // requests message buffer for sending and receiving messages
   msg = client_createMessage();
   replyMsg = client_createMessage();

   srand((uint)time(NULL)); // seeds the pseudo-random generator
   wait(); // randomizes first transaction

   //  runs the client

   if (db_state() != TRM_SWITCHED_ON) // terminal switched off unexpectedly,
      synchronizeWithServer(msg);     // or crashed, or whatever

   for (;;)
   {
      // creates the message to be sent
      createMessage(msg);

      // ensures the connection with the server is alive
      if (!checkConnection())
         continue;

      // sends the message to the server
      if (!sendMessage(msg))
         continue; // on errors don't wait for replies

      // waits for the reply from the server
      if (!recvReply(replyMsg))
         continue; // on errors don't process replies

      // acts according to the reply sent by the server
      processReply(replyMsg);

      // waits some time before sending another transaction
      wait();
   }

   return 0;
}

// -----------------------------------------------------------------------------

// ensures the connection with the server is alive

static bool checkConnection(void)
{
   uint cnt = 0;

   if (client_isConnected())
      return true;

   // if the connection is not alive, any transaction waiting to be sent will be
   // aborted

   client_createClientSocket();

// client_setTrace(true); // do we want low level tracing ?

   printInfo("trying to connect to the server");

   while (!client_connect())
   {
      printError("connection failed, waiting 5 secs to try again (%d)", cnt++);
      client_sleep(5);
   }

   return false;
}

// -----------------------------------------------------------------------------

static bool checkReplyHeader(Message* msg, cchar* tranCode, ushort replySize)
{
   uint size = client_messageSize(msg);

   if (size != replySize)
   {
      printError("invalid size %d in reply to transaction %.*s", size,
         TRAN_CODE_LEN, tranCode);
      return false;
   }

   if (client_compareToMessage(tranCode, msg, TRAN_CODE_OFFSET, TRAN_CODE_LEN))
   {
      char buf[TRAN_CODE_LEN];
      client_copyFromMessage(buf, msg, TRAN_CODE_OFFSET, TRAN_CODE_LEN);
      printError("invalid reply %.*s to transaction %.*s",
         TRAN_CODE_LEN, buf, TRAN_CODE_LEN, tranCode);
      return false;
   }

   if (client_compareToMessage("000", msg, ERR_CODE_OFFSET, ERR_CODE_LEN))
   {
      char buf[ERR_CODE_LEN];
      client_copyFromMessage(buf, msg, ERR_CODE_OFFSET, ERR_CODE_LEN);
      printError("error %.*s in reply to transaction %.*s",
         ERR_CODE_LEN, buf, TRAN_CODE_LEN, tranCode);
      return false;
   }

   printInfo("received successful reply to transaction %.*s",
      TRAN_CODE_LEN, tranCode);

   return true;
}

// -----------------------------------------------------------------------------

static void createMessage(Message* msg)
{
   uchar state = db_state();

   switch (state)
   {
      case TRM_SWITCHED_ON:
         createMsg_0000(msg);
         break;

      case TRM_RECOGNIZED:
         createMsg_0010(msg);
         break;

      case TRM_AUTHORIZED:
         if (beginShift)
            createMsg_0020(msg); //  begins shift
         else
            createMsg_0050(msg); // closes terminal
         break;

      case TRM_OPERATING:
         if (sendOperation)
            createMsg_0030(msg);  // sends operation
         else
            createMsg_0040(msg); // ends shift
         break;

      case TRM_CLOSED:
         printFatal("terminal not allowed to work today anymore!");
         abort();
         break;

      default:
         printFatal("terminal in invalid state %d, exiting!", state);
         abort();
         break;
   } // switch
}

// -----------------------------------------------------------------------------

static void createMessageHeader(Message* msg, cchar* tranCode)
{
   client_copyToMessage(msg, tranCode, TRAN_CODE_OFFSET, TRAN_CODE_LEN);
   client_copyToMessage(msg, db_terminalID(), TERM_ID_OFFSET, TERM_ID_LEN);
}

// -----------------------------------------------------------------------------

static void createMsg_0000(Message* msg)
{
   client_setMessageSize(msg, sizeof(Message_0000));
   createMessageHeader(msg, "0000");
   client_copyToMessage(msg, db_serialNumber(), SER_NUM_OFFSET, SER_NUM_LEN);
}

// -----------------------------------------------------------------------------

static void createMsg_0010(Message* msg)
{
   client_setMessageSize(msg, sizeof(Message_0010));
   createMessageHeader(msg, "0010");
   client_copyToMessage(msg, db_supervisor(), SUPERV_OFFSET, EMPLOYEE_LEN);
}

// -----------------------------------------------------------------------------

static void createMsg_0020(Message* msg)
{
   client_setMessageSize(msg, sizeof(Message_0020));
   createMessageHeader(msg, "0020");
   client_copyToMessage(msg, db_operator(), TERM_OPER_OFFSET, EMPLOYEE_LEN);
}

// -----------------------------------------------------------------------------

static void createMsg_0030(Message* msg)
{
   char buf[20];
   int i, operation, operationSize;

   // chooses operation
   operation = rand() % (MAX_OP_CODE+1);
   operationSize = rand() % (client_maxMessageSize() - sizeof(Message_0030));

   client_setMessageSize(msg, sizeof(Message_0030)+operationSize);

   createMessageHeader(msg, "0030");
   client_copyToMessage(msg, db_operator(), TERM_OPER_OFFSET, EMPLOYEE_LEN);

   sprintf(buf, "%0*d", OP_CODE_LEN, operation);
   client_copyToMessage(msg, buf, OP_CODE_OFFSET, OP_CODE_LEN);

   sprintf(buf, "%0*d", OP_DATA_LEN, operationSize);
   client_copyToMessage(msg, buf, OP_DATALEN_OFFSET, OP_DATA_LEN);

   for (i = 0; i < operationSize; i++)
   {
      char ch = (char)(0x20 + (rand() % (0x7F - 0x20)));
      client_setMessageByte(msg, OP_DATALEN_OFFSET+OP_DATA_LEN+i, ch);
   }

}

// -----------------------------------------------------------------------------

static void createMsg_0040(Message* msg)
{
   client_setMessageSize(msg, sizeof(Message_0040));
   createMessageHeader(msg, "0040");
   client_copyToMessage(msg, db_operator(), TERM_OPER_OFFSET, EMPLOYEE_LEN);
}

// -----------------------------------------------------------------------------

static void createMsg_0050(Message* msg)
{
   client_setMessageSize(msg, sizeof(Message_0050));
   createMessageHeader(msg, "0050");
   client_copyToMessage(msg, db_supervisor(), SUPERV_OFFSET, EMPLOYEE_LEN);
}

// -----------------------------------------------------------------------------

// convenience printf-like function for formatting purposes

static cchar* format(cchar* fmt, va_list ap)
{
   static char buf[256];

   buf[0] = '*';                    // (1 byte)  00
   buf[1] = ' ';                    // (1 byte)  01
   client_formatCurrentTime(buf+2); // (8 bytes) 02 03 04 05 06 07 08 09
   buf[10] = ' ';                   // (1 byte)  10

   vsnprintf(buf+11, sizeof(buf)-11, fmt, ap);

   return buf;
}

// -----------------------------------------------------------------------------
// PUBLIC FUNCION
// -----------------------------------------------------------------------------

// printf-like function that writes on stdout and the log file

void printError(cchar* fmt, ...)
{
   cchar * buf;
   va_list ap;

   va_start(ap, fmt);
   buf = format(fmt, ap);
   va_end(ap);

   printf("%s\n", buf);
   fflush(stdout);

   client_logError(buf+11);
}

// -----------------------------------------------------------------------------
// PUBLIC FUNCION
// -----------------------------------------------------------------------------

// printf-like function that writes on stdout an the log file

void printFatal(cchar* fmt, ...)
{
   cchar * buf;
   va_list ap;

   va_start(ap, fmt);
   buf = format(fmt, ap);
   va_end(ap);

   printf("%s\n", buf);
   fflush(stdout);

   client_logFatal(buf+11);
}

// -----------------------------------------------------------------------------
// PUBLIC FUNCION
// -----------------------------------------------------------------------------

// printf-like function that writes on stdout an the log file

void printInfo(cchar* fmt, ...)
{
   cchar * buf;
   va_list ap;

   va_start(ap, fmt);
   buf = format(fmt, ap);
   va_end(ap);

   printf("%s\n", buf);
   fflush(stdout);

   client_logInfo(buf+11);
}

// -----------------------------------------------------------------------------
// PUBLIC FUNCION
// -----------------------------------------------------------------------------

// printf-like function that writes on stdout an the log file

void printWarn(cchar* fmt, ...)
{
   cchar * buf;
   va_list ap;

   va_start(ap, fmt);
   buf = format(fmt, ap);
   va_end(ap);

   printf("%s\n", buf);
   fflush(stdout);

   client_logWarn(buf+11);
}

// -----------------------------------------------------------------------------

// acts according to the reply sent by the server

static void processReply(Message* reply)
{
   uchar state = db_state();

   switch (state)
   {
      case TRM_SWITCHED_ON:
         processReply_0000(reply);
         break;

      case TRM_RECOGNIZED:
         processReply_0010(reply);
         break;

      case TRM_AUTHORIZED:
         if (beginShift)
            processReply_0020(reply); //  begins shift
         else
            processReply_0050(reply); // closes terminal
         break;

      case TRM_OPERATING:
         if (sendOperation)
            processReply_0030(reply);  // sends operation
         else
            processReply_0040(reply); // ends shift
         break;

      default:
         printFatal("processReply: terminal in invalid state %d, exiting!",
            state);
         abort();
         break;
   } // switch
}

// -----------------------------------------------------------------------------

static void processReply_0000(Message* msg)
{
   if (!checkReplyHeader(msg, "0000", sizeof(Reply_0000)))
      return;

   db_updateState(TRM_RECOGNIZED);

   printInfo("terminal entered state RECOGNIZED");
}

// -----------------------------------------------------------------------------

static void processReply_0010(Message* msg)
{
   if (!checkReplyHeader(msg, "0010", sizeof(Reply_0010)))
      return;

   db_updateState(TRM_AUTHORIZED);

   printInfo("terminal entered state AUTHORIZED");
}

// -----------------------------------------------------------------------------

static void processReply_0020(Message* msg)
{
   if (!checkReplyHeader(msg, "0020", sizeof(Reply_0020)))
      return;

   db_updateState(TRM_OPERATING);

   printInfo("terminal entered state OPERATING");
}

// -----------------------------------------------------------------------------

static void processReply_0030(Message* msg)
{
   if (!checkReplyHeader(msg, "0030", sizeof(Reply_0030)))
      return;

   printInfo("terminal continuing in the state OPERATING");
}

// -----------------------------------------------------------------------------

static void processReply_0040(Message* msg)
{
   if (!checkReplyHeader(msg, "0040", sizeof(Reply_0040)))
      return;

   db_updateState(TRM_AUTHORIZED);

   printInfo("terminal left state OPERATING, entered state AUTHORIZED");
}

// -----------------------------------------------------------------------------

static void processReply_0050(Message* msg)
{
   if (!checkReplyHeader(msg, "0050", sizeof(Reply_0050)))
      return;

   db_updateState(TRM_CLOSED);

   printInfo("terminal left state AUTHORIZED, entered state CLOSED");
}

// -----------------------------------------------------------------------------

// receives the reply from the server

static uint recvReply(Message* msg)
{
   printInfo("receiving reply from the server");

   if (client_recvMessage(msg)) // true is good
   {
      printInfo("received reply with orgId:%d and orgSeq:%d",
         client_messageOrgId(msg), client_messageOrgSeqNo(msg));

      // warning, log file can grow too much!
      // client_logTrace("message received", client_messageBuffer(msg),
      //   client_messageSize(msg));

      return message_size(msg);
   }

   // if timeout occurs, connection is not automatically closed, so closes it
   if (client_timeout()) // timeout ?
   {
      printError("timeout receiving reply from the server, closing connection");
      client_closeConnection();
      return 0;
   }

   // from here on, the connection is sure to have been closed

   if (client_connectionClosed()) // connection closed by server ?
   {
      printError("connection closed when receiving reply from the server");
      return 0;
   }

   if (client_formatError()) // format error on the wire ?
   {
      printError("invalid format in reply received from the server");
      return 0;
   }

   // other errors
   printError("error %d while receiving reply from the server", client_error());

   return 0;
}

// -----------------------------------------------------------------------------

// sends a message to the server (the size is embedded in the message buffer)

static bool sendMessage(Message* msg)
{
   printInfo("sending message %.*s to the server", TRAN_CODE_LEN,
      client_messageBuffer(msg));

   // warning, log file can grow too much!
   // client_logTrace("message being sent", client_messageBuffer(msg),
   //    client_messageSize(msg));

   if (client_sendMessage(msg)) // true is good
   {
      printInfo("ok, the message was sent");
      return true;
   }

   if (client_timeout()) // probably server application is clogged, or stuck,
   {                     // or has crashed, or whatever
      printError("timeout when sending a message to the server");
      client_closeConnection();
      return false;
   }

   // other errors, connection is automatically closed
   printError("error %d when sending a message to the server", client_error());

   return false;
}

// -----------------------------------------------------------------------------

static void synchronizeWithServer(Message* msg)
{
   int secs;

   printWarn("synchronizing terminal with server, current state: %c",
      db_state());

   for (;;)
   {
      // creates the synchronization message
      createMsg_0000(msg);

      // ensures the connection with the server is alive
      if (!checkConnection())
         goto NEXT;

      // sends the message to the server

      // warning, log file can grow too much!
      // client_logTrace("message being sent", client_messageBuffer(msg),
      //   client_messageSize(msg));
      if (!sendMessage(msg))
         goto NEXT;

      // waits for the reply from the server
      if (!recvReply(msg))
         goto NEXT;

      // warning, log file can grow too much!
      // client_logTrace("message received", client_messageBuffer(msg),
      //   client_messageSize(msg));

      // acts according to the reply sent by the server
      if (!checkReplyHeader(msg, "0000", sizeof(Reply_0000)))
         goto NEXT;

      printInfo("terminal synchronized with server");
      return;

      // waits some time before trying again
   NEXT:
      secs = rand() % 4;
      if (secs)
      {
         printInfo("waiting %d seconds before creating the message", secs);
         client_sleep(secs);
      }
   } // for
}

// -----------------------------------------------------------------------------

// waits some seconds before sending the next transaction

static void wait(void)
{
   ushort maxSleepTime = db_maxSleepTime();
   ushort pSendOper = db_pSendOper();
   ushort pBeginShift = db_pBeginShift();

   // waits some time between message creation (between 0 and 3 seconds)
   int secs = rand() % maxSleepTime;
   if (secs)
   {
      printInfo("waiting %d seconds before creating the message", secs);
      client_sleep(secs);
   }

   // now, if the terminal is in the OPERATING state, chooses (with a low
   // probability) whether to end the shift
   if (db_state() == TRM_OPERATING)
   {
      int p = (rand() % pSendOper);
      printInfo("pSendOper=%d p=%d", pSendOper, p);
//    sendOperation = (rand() % pSendOper) < (pSendOper-1);
      sendOperation = p < (pSendOper-1);
   }

   // now, if the terminal is in the AUTHORIZED state, chooses (with medium
   // probability) whether to close the terminal
   if (db_state() == TRM_AUTHORIZED)
   {
      int p = (rand() % pBeginShift);
      printInfo("pBeginShift=%d p=%d", pBeginShift, p);
//    beginShift = (rand() % pBeginShift) < (pBeginShift-1);
      beginShift = p < (pBeginShift-1);
   }
}

// -----------------------------------------------------------------------------
// the end
