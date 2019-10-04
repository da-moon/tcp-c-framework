/// @source       Server_1.c
/// @description  A "server" used for testing the server framework.

// -----------------------------------------------------------------------------

/** A minimum server running 3 threads of the same code. The threads are in
    fact unrelated to the framework, and don't do any communications at all.

    Test standalone (does not need a client program).
*/

// -----------------------------------------------------------------------------

// common configuration options & declarations (always include first)
#include "config.h"

// application includes
#include "Server.h" /* server_xxx functions */

// -----------------------------------------------------------------------------

// global declarations

// prototypes
static threadfunc threadFunc(void*);

// -----------------------------------------------------------------------------

// the main program

void main(void)
{
   // does whatever initialization it's needed
   server_init();

   // starts 3 threads of the same code
   server_addThreads(3, threadFunc, "example thread");

   // runs the server
   server_run();
}

// -----------------------------------------------------------------------------

// a minimal thread

static threadfunc threadFunc(void* arg)
{
   arg = 0; // only to avoid warnings...

   for (;;)
   {
      // uses "server_printf" (and not plain printf) because of contention issues
      // between threads
      server_printf("Hello from a minimal thread...\n");

      server_sleep(3);
   }

   return 0;
}

// -----------------------------------------------------------------------------
// the end
