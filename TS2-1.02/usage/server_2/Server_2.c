/// @source       Server_2.c
/// @description  An echo server used for testing the server framework.

// -----------------------------------------------------------------------------

/** A minimum TCP server running 5 threads of the same code. The threads
    simply echo back the received messages.

    Test with Client_1.
*/

// -----------------------------------------------------------------------------

// common configuration options & declarations (always include first)
#include "config.h"

// C language includes
#include <stdarg.h>
#include <stdio.h>

// application includes
#include "Server.h" /* server_xxx functions */

// -----------------------------------------------------------------------------

// global declarations

// convenience functions
static void printInfo(cchar*, ...);

// the thread code to be run
static threadfunc threadFunc(void*);

// sets tracing for a connection
static void setTrace(void*);

// for identification purposes in the log file
static cchar sourceID[] = "SSS";

// -----------------------------------------------------------------------------

// the main program

void main(void)
{

   // the port on which ther server waits for connection requests
   server_setServicePort(12345);

   // how much detail is written to the log file
   server_setLogLevel(LOG_LEVEL_DEBUG);

   // configures callback to customize new connections
   // (for now only used to enable tracing)
   server_setCallback(0, setTrace);

   // does whatever initialization it's needed
   server_init();

   // starts 5 threads of the same code
   server_addThreads(5, threadFunc, "example thread");

   // runs the server
   server_run();
}

// -----------------------------------------------------------------------------

// A typical thread to be run by the server framework.

static threadfunc threadFunc(void* arg)
{
   Message *msg;

   arg = 0; // only to avoid warnings...

   for (;;) // the thread main loop
   {
      printInfo("waiting for messages (thread #%d)", thread_selfSeqNo());

      // waits for a message from a client
      msg = server_waitInputMessage();

      // only written to the log file if the log level is right
      // (can be use for application debugging)
      server_logDebug("received message with orgId=%d orgSeq=%d (thread #%d)",
         server_messageOrgId(msg), server_messageOrgSeqNo(msg),
         thread_selfSeqNo());

      // only written to the log file if the log level is right
      server_logDebug("replying message (thread #%d)", thread_selfSeqNo());

      printInfo("ok, replying (thread #%d)", thread_selfSeqNo());

      // echoes back the received message to the client
      server_dispatchOutputMessage(msg);
   }

   // yes, that's it!
   return 0;
}

// -----------------------------------------------------------------------------

// convenience printf-like function for formatting purposes

static void format(char * buf, ushort bufLen, cchar* fmt, va_list ap)
{
   buf[0] = '*';                    // (1 byte)  00
   buf[1] = ' ';                    // (1 byte)  01
   server_formatCurrentTime(buf+2); // (8 bytes) 02 03 04 05 06 07 08 09
   buf[10] = ' ';                   // (1 byte)  10

   vsnprintf(buf+11, bufLen-11, fmt, ap);
}

// -----------------------------------------------------------------------------

// printf-like function that writes on stdout an the log file

static void printInfo(cchar* fmt, ...)
{
   char buf[256];
   va_list ap;

   va_start(ap, fmt);
   format(buf, 256, fmt, ap);
   va_end(ap);

   buf[0] = '*';                    // (1 byte)  00
   buf[1] = ' ';                    // (1 byte)  01
   server_formatCurrentTime(buf+2); // (8 bytes) 02 03 04 05 06 07 08 09
   buf[10] = ' ';                   // (1 byte)  10

   // uses "server_printf" (and not plain printf) because of contention issues
   // between threads
   server_printf("%s\n", buf);

   // writes the log file
   server_logInfo(buf+11);
}

// -----------------------------------------------------------------------------

// #if 0
static void setTrace(void* connection)
{
   // for tracing new connections, enable line below
   // (could be used to inspect new connections for selectively enable tracing)

   server_setTrace(connection, true);

   connection = 0; // just to avoid warnings...
}
// #endif

// -----------------------------------------------------------------------------
// the end
