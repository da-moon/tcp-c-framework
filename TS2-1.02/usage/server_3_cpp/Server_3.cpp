/// @source       Server_3.cpp
// @description   An echo server used for testing the server framework.

// -----------------------------------------------------------------------------

/** A minimum TCP server running 2 types of threads.

    The communications threads do a fake processing of the received messages
    and send back the replies to the client applications.

    The other threads from time to time write a message on the console.

    The server is configurable from the command-line.

    The threads also writes information to the log file.

    (Aside from some bells and whistles, this program is the same as Server_2.)

    Test with Client_1.
*/

// -----------------------------------------------------------------------------

// common configuration options & declarations (always include first)
#include "config.h"

// C++ language includes
// ...

// C language includes
#include <stdio.h>
#include <stdlib.h> // atoi, exit
#include <string.h>

#if PLATFORM(Windows)
#define snprintf _snprintf
#endif

// framework includes
#include "Server.h" /* server_xxx functions */

// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

// global declarations

// for identification purposes in the log file
const char sourceID[] = "AAA";

// command-line options
cchar* options[] =
{
   "-loglevel",
   "-msgsize",
   "-port",
   "-threads1",
   "-threads2",
   0
};

// threads to be run by the framework
threadfunc threadFunc1(void*);
threadfunc threadFunc2(void*);

// helper functions
uint arg_u(int, char*[], const char*, uint);
void checkCommandLineOptions(int, char**);
uint numeric(cchar*);
void usage();

// -----------------------------------------------------------------------------
} // close anonymous namespace
// -----------------------------------------------------------------------------

// the main program function

int main(int argc, char* argv[])
{
   // -------------------------------------------------------

   // program parameters (0 means use default)

   // thread "types" are made up for this application, for the library there are
   // no distinctions

   // how many "type 1" threads to run
   uint nThreads1 = arg_u(argc, argv, "-threads1", 0);

   // how many "type 2" threads to run
   uint nThreads2 = arg_u(argc, argv, "-threads2", 0);

   // maximum message size supported
   uint maxMsgSize = arg_u(argc, argv, "-msgsize", 0);

   // server TCP service port
   uint servicePort = arg_u(argc, argv, "-port", 0);

   // log level
   uint logLevel = arg_u(argc, argv, "-loglevel", 0);

   // -------------------------------------------------------

   checkCommandLineOptions(argc, argv);

   printf("*\n* TS2 Libray Version: %s\n", server_getVersion());

   printf("*\n* options:\n");
   printf("*    -loglevel: %d\n", logLevel);
   printf("*    -msgsize: %d\n",  maxMsgSize);
   printf("*    -port: %d\n",     servicePort);
   printf("*    -threads1: %d\n", nThreads1);
   printf("*    -threads2: %d\n", nThreads2);
   printf("*\n");

   // phases of the application

   // phase 1: configuration (optional, otherwise uses default)

   server_logInfo("setting the maximum message size");
   server_setMaxMessageSize(maxMsgSize);

   server_logInfo("setting the service port");
   server_setServicePort(servicePort);

   server_logInfo("setting the log level");
   server_setLogLevel(logLevel);

   // phase 2: initialization
   server_logInfo("initializing the server");
   server_init();

   // phase 3: add some threads

   server_logInfo("adding %d threads os type 1", nThreads1);
   server_addThreads(nThreads1, threadFunc1, (void*)"thread type 1");

   server_logInfo("adding %d threads os type 2", nThreads2);
   server_addThreads(nThreads2, threadFunc2, (void*)"thread type 2");
   // etc

   // phase 4: run!
   server_logInfo("running the server!");
   server_run();

   // and that's all!
   exit(0);
}

// -----------------------------------------------------------------------------
namespace { // open anonymous namespace
// -----------------------------------------------------------------------------

/** Retrieves a program option.
*/

uint arg_u(int argc, char* argv[], const char* argStr, uint def)
{
   int i;

   for (i = 1; i < argc - 1; i++)
   {
      if (!strcmp(argStr, argv[i])) // found option string (ex.: "-xxx")
      {
         if (!numeric(argv[i+1]))
         {
            printf("*\n* non-numeric characters in option %s: [%s]\n*\n",
               argv[i], argv[i+1]);
            usage();
         }
         return (uint)atoi(argv[i+1]);
      }
   }

   // didn't find argument, returns default value
   return def;
}

// -----------------------------------------------------------------------------

/** Checks if all command-line options are valid.
*/

void checkCommandLineOptions(int argc, char** argv)
{
   int i;

   printf("*\n* checking command-line parameters\n");

   if (argc == 1)
   {
      printf("* no parameters!\n");
      return;
   }

   if (!(argc % 2))
   {
      printf("* probably missing value for a parameter!\n");
      usage();
   }

   for (i = 1; i < argc-1; i++)
   {
      int j;
      for (j = 0; options[j]; j++)
      {
          // printf("* checking: argv[%d]=[%s]\t\toptions[%d]=[%s]\n",
          //    i, argv[i], j, options[j]);

          if (!strcmp(argv[i], options[j])) // found option string (ex.: "-xxx")
          {
             printf("* ok, found %s\n", options[j]);
             // skips the option's value and goes to the next option
             i++;
             break;
          }
      } // for j

      if (!options[j])
      {
        // didn't find the option
         printf("*\n* unrecognized option: '%s'\n*\n", argv[i]);
         usage();
      }
   } // for i
}

// -----------------------------------------------------------------------------

/** Checks if a string is all numeric.
*/

uint numeric(cchar* str)
{
   return (strspn(str, "0123456789") == strlen(str));
}

// -----------------------------------------------------------------------------

/** A typical thread to be run by the server framework.
*/

threadfunc threadFunc1(void*)
{
   uint size, count = 0;
   char *bufIn, *bufOut;
   Message *msgIn, *msgOut;

   for (;;)
   {
      server_logInfo("(threadFunc1) waiting for a message from a client");

      // waits for a message from a client
      msgIn = server_waitInputMessage();

      // ok, message received
      bufIn = server_messageBuffer(msgIn);
      size = server_messageSize(msgIn);

      // requests an (output) message to be sent as reply to the client
      msgOut = server_getFreeMessage();
      bufOut = server_messageBuffer(msgOut);

      // processes the (input) message received from the client
      server_printf("* message: length=%02d buf=[%.20s]\n", size, bufIn);
      // blah blah blah

      // creates the (output) message to be sent as reply to the client
      // (uses the same bytes and size, just so the client can check them)
      memcpy(bufOut, bufIn, size);
      server_setMessageSize(msgOut, size);

      if (!(++count % 10))
         server_logInfo("%d messages processed now", count);

      // copies connection information from the input message to the output
      // message (this is needed so that the framework knows to which client
      // to send the output message)
      server_copyConnectionFromMessage(msgOut, msgIn);

      // releases the input message, it's not needed anymore
      // (this wouldn't be needed if the input message was reused)
      server_disposeMessage(msgIn);

      // makes the message available to be sent to the client
      server_dispatchOutputMessage(msgOut);
   }

   // that's it
   return 0;
}

// -----------------------------------------------------------------------------

/** Another thread to be run by the server framework.

    This thread provides a service that is unrelated to the framework, i.e,
    it does not use messages, connections, etc.
*/

threadfunc threadFunc2(void* _parm)
{
   char buf[256];
   char* parm = (char*)_parm;

   uint counter = 0;

   for (;;)
   {
      snprintf(buf, 256, "parm=[%s] threadID=%04X threadSeqNo=%02d "//----------
         "threadCounter=%d", parm, thread_selfID(), thread_selfSeqNo(),
          counter++);
      server_printf("* %s\n",buf);
      server_logInfo(buf);
      server_sleep(3);
   }

   // that's it
   return 0;
}

// -----------------------------------------------------------------------------

/** Directions for using this program.
*/

void usage()
{
   printf("* Program usage:\n*\n");
   printf("*    server_3\n"
      "*           [-threads1 <value>]\n"
      "*           [-threads2 <value>]\n"
      "*           [-port] <value>\n"
      "*           [-msgsize]\n*\n");

   exit(0);
}

// -----------------------------------------------------------------------------
} // close anonymous namespace
// -----------------------------------------------------------------------------
// the end
