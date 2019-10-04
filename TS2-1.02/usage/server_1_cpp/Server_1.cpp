/// @source       Server_1.cpp
/// @description  A "server" used for testing the server framework.

// -----------------------------------------------------------------------------

/** A minimum server running 3 threads of the same code. The threads are in
    fact unrelated to the framework, and don't do any communications at all.

    Test standalone (does not need a client program).
*/

// -----------------------------------------------------------------------------

// common configuration options & declarations (always include first)
#include "config.h"

// C++ language includes
// ...

// framework includes
#include "Server.h" /* server_xxx functions */

// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

// global declarations

// prototypes
threadfunc threadFunc(void*);

// -----------------------------------------------------------------------------
} // close anonymous namespace
// -----------------------------------------------------------------------------

// the main program

int main()
{
   // does whatever initialization it's needed
   server_init();

   // starts 3 threads of the same code
   server_addThreads(3, threadFunc, (void*)"example thread");

   // runs the server
   server_run();
}

// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

// a minimal thread

threadfunc threadFunc(void*)
{
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
} // close anonymous namespace
// -----------------------------------------------------------------------------
// the end
