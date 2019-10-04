/// @source       Server_4.cpp
/// @description  A simulated POS terminal server.

// -----------------------------------------------------------------------------

/** A simulated server for a POS terminal.

    See the file README.TXT for a description of the application protocol and
    program logic.

   Teste with Client_4.
*/

// -----------------------------------------------------------------------------

// common configuration options & declarations (always include first)
#include "config.h"

// C++ language includes

// C language includes
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// application includes
#include "Server.h"       /* server_xxx frameworkfunctions */
#include "Server_4.h"     /* main application include      */
#include "Database.h"     /* db_xxx functions              */
#include "Transactions.h" /* tran_xxx functions            */

// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

// global declarations

// for identification purposes in the log file
const char sourceID[] = "AAA";

// threads to be run by the framework
threadfunc threadFunc(void*);

// sets low level tracing for a connection
// void setTrace(void*);

// convenience formatting function
cchar* format(cchar*, va_list);

// -----------------------------------------------------------------------------
} // close anonymous namespace
// -----------------------------------------------------------------------------

/// The main program function.

int main()
{
   // phase 1: configuration
   server_setServicePort(SERVER_PORT);

// server_setLogLevel(LOG_LEVEL_DEBUG);

// // enable tracing
// server_setCallback(0, setTrace);

   // phase 2: initialization
   server_init();

   // application "database" initialization
   db_init();

   // phase 3: adding 3 threads
   server_addThreads(3, threadFunc, 0);

   // phase 4: running
   server_run();

   return 0;
}

// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

/// The application thread to be run by the server framework.

threadfunc threadFunc(void* /*arg*/)
{
   uint size;
   char* bufIn;
   TranFunc* tranFunc;
   Message *msgIn, *msgOut;

// arg = 0; // only to avoid warnings...

   for (;;) // thread main loop
   {
      printInfo("waiting for a message from a client");

      // waits for a message from a client
      msgIn = server_waitInputMessage();

      // ok, message received
      bufIn = server_messageBuffer(msgIn);
      size = server_messageSize(msgIn);

      // warning, log file can grow to enormous size
      // server_logTrace("received message", bufIn, size);

      // checks if size is tentatively valid
      if (size < sizeof(MessageHeader))
      {
         printError("transaction size %d is too short, discarding", size);
         goto NEXT;
      }

      // finds transaction processing function
      tranFunc = tran_findFunc(bufIn);
      if (!tranFunc)
      {
         printError("transaction %.4s unknown, discarding", bufIn);
         goto NEXT;
      }

      // now does the transaction processing

      db_lockDatabase();
      msgOut = tranFunc(msgIn);
      db_unlockDatabase();

      if (!msgOut)
      {
         printInfo("no reply will be sent for transaction %.4s", bufIn);
         goto NEXT;
      }

      // makes the reply available to be sent to the client
      // warning, log file can grow to enormous size
      // server_logTrace("reply to be sent", server_messageBuffer(msgOut),
      //   server_messageSize(msgOut));
      server_dispatchOutputMessage(msgOut);

   NEXT:
      // releases the input message
      server_disposeMessage(msgIn);
   }

   return 0;
}

// -----------------------------------------------------------------------------

// convenience printf-like function for formatting purposes

cchar* format(cchar* fmt, va_list ap)
{
   static char buf[256];

   buf[0] = '*';                    // (1 byte)  00
   buf[1] = ' ';                    // (1 byte)  01
   server_formatCurrentTime(buf+2); // (8 bytes) 02 03 04 05 06 07 08 09
   buf[10] = ' ';                   // (1 byte)  10

   vsnprintf(buf+11, sizeof(buf)-11, fmt, ap);

   return buf;
}

// -----------------------------------------------------------------------------
} // close anonymous namespace
// -----------------------------------------------------------------------------

// printf-like function that writes on stdout and the log file

void printError(cchar* fmt, ...)
{
   cchar * buf;
   va_list ap;

   va_start(ap, fmt);
   buf = format(fmt, ap);
   va_end(ap);

   server_printf("%s\n", buf);
   server_logError(buf+11);
}

// -----------------------------------------------------------------------------

// printf-like function that writes on stdout and the log file

void printFatal(cchar* fmt, ...)
{
   cchar * buf;
   va_list ap;

   va_start(ap, fmt);
   buf = format(fmt, ap);
   va_end(ap);

   server_printf("%s\n", buf);
   server_logFatal(buf+11);
}

// -----------------------------------------------------------------------------

// printf-like function that writes on stdout an the log file

void printInfo(cchar* fmt, ...)
{
   cchar * buf;
   va_list ap;

   va_start(ap, fmt);
   buf = format(fmt, ap);
   va_end(ap);

   server_printf("%s\n", buf);
   server_logInfo(buf+11);
}

// -----------------------------------------------------------------------------

// printf-like function that writes on stdout an the log file

void printWarn(cchar* fmt, ...)
{
   cchar * buf;
   va_list ap;

   va_start(ap, fmt);
   buf = format(fmt, ap);
   va_end(ap);

   server_printf("%s\n", buf);
   server_logWarn(buf+11);
}

// -----------------------------------------------------------------------------

// void setTrace(void* connection)
// {
//    // enable tracing for new connections
//    server_setTrace(connection, true);
// 
//    connection = 0; // just to avoid warnings...
// }

// -----------------------------------------------------------------------------
// the end
