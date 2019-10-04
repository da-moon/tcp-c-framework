/// @source       Operations.cpp
/// @description  Simulated POS operations.

// -----------------------------------------------------------------------------

// common configuration options & declarations (always include first)
#include "config.h"

// C++ language includes
// ...

// C language includes
#include <string.h>

// application includes
#include "Server.h"       /* server_xxx functions */
#include "Server_4.h"     /* this application     */
#include "Operations.h"   /* oper_xxx functions   */
#include "Database.h"     /* db_xxx functions     */
#include "Transactions.h" /* oper_xxx functions   */

// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

// global declarations

// for identification purposes in the log file
const char sourceID[] = "OPE";

// operation processing functions
void opFunc_000(const Message* in, Message* out, short term);
void opFunc_001(const Message* in, Message* out, short term);
void opFunc_002(const Message* in, Message* out, short term);
void opFunc_003(const Message* in, Message* out, short term);
void opFunc_004(const Message* in, Message* out, short term);

// the operation table
Operation operations[] =
{
   { "000", opFunc_000 } , // sale of product type 1
   { "001", opFunc_001 } , // slip payment
   { "002", opFunc_002 } , // etc
   { "003", opFunc_003 } , // etc
   { "004", opFunc_004 } , // etc
   { 0, 0 }
};

// -----------------------------------------------------------------------------
} // close anonymous namespace
// -----------------------------------------------------------------------------
/// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

// finds the processing function associated with a specific operation

OpFunc* oper_findFunc(cchar* opCode)
{
   Operation* operation = &operations[0];

   while (operation->code)
      if (!memcmp(opCode, operation->code, OP_CODE_LEN))
         return operation->opFunc;
      else
         ++operation;

   // not found
   return NULL;
}

// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------
// OPERATION 000
// -----------------------------------------------------------------------------

void opFunc_000(const Message* /*msgIn*/, Message* /*msgOut*/,
   short termIndex)
{
   printInfo("processing operation 000 for terminal %.*s", TERM_ID_LEN,
      db_terminalID(termIndex));
// msgIn = msgOut = 0; // avoids warning
}

// -----------------------------------------------------------------------------
// OPERATION 001
// -----------------------------------------------------------------------------

void opFunc_001(const Message* /*msgIn*/, Message* /*msgOut*/,
   short termIndex)
{
   printInfo("processing operation 001 for terminal %.*s", TERM_ID_LEN,
      db_terminalID(termIndex));
// msgIn = msgOut = 0; // avoids warning
}

// -----------------------------------------------------------------------------
// OPERATION 002
// -----------------------------------------------------------------------------

void opFunc_002(const Message* /*msgIn*/, Message* /*msgOut*/,
   short termIndex)
{
   printInfo("processing operation 002 for terminal %.*s", TERM_ID_LEN,
      db_terminalID(termIndex));
// msgIn = msgOut = 0; // avoids warning
}

// -----------------------------------------------------------------------------
// OPERATION 003
// -----------------------------------------------------------------------------

void opFunc_003(const Message* /*msgIn*/, Message* /*msgOut*/,
   short termIndex)
{
   printInfo("processing operation 003 for terminal %.*s", TERM_ID_LEN,
      db_terminalID(termIndex));
// msgIn = msgOut = 0; // avoids warning
}

// -----------------------------------------------------------------------------
// OPERATION 004
// -----------------------------------------------------------------------------

void opFunc_004(const Message* /*msgIn*/, Message* /*msgOut*/,
   short termIndex)
{
   printInfo("processing operation 004 for terminal %.*s", TERM_ID_LEN,
      db_terminalID(termIndex));
// msgIn = msgOut = 0; // avoids warning
}

// -----------------------------------------------------------------------------
} // close anonymous namespace
// -----------------------------------------------------------------------------
// the end
