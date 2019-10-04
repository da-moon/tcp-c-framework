/// @source      GenericClient.c
/// @description Implementation of class GenericClient.
/// The client functions are not really part of the framework, but are provided
/// as a convenience for testing the framework.
//
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations
#include "config.h" // always include first

// C language includes
#include <assert.h>
#include <stdlib.h> // abort
// #include <string.h>

// framework includes
#include "Client.h"        /* client_xxx functions  */
#include "GenericClient.h" /* genCli_xxx functions  */
#include "Socket.h"        /* socket_xxx functions  */

// -----------------------------------------------------------------------------

// global declarations

/// GenericClient class.
/// This class provides members and methods that make it easier to write simple
/// client applications.
/// @class GenericClient
/// @memberof Client
typedef struct GenericClient
{
   ushort state;
   ushort command;
   ushort event;
   int error;
   ushort errorType;
   Message* message; // comms buffer
   char* operation;
   bool shouldRetryConnect;
} GenericClient;

/// Actual event returned to the generic client application.
/// @private @memberof Client
static GenericClient genCli;

/// Configuration flag: keep retrying when connect fails
/// @private @memberof Client

/// States of the generic client.
/// @private @memberof Client
enum GenericClientState
{
   CLI_ST_NOT_CONNECTED  ,
   CLI_ST_CONNECTED_IDLE ,
};

/// Commands for the generic client.
/// @private @memberof Client
enum GenericClientCommand
{
   CLI_CMD_CLOSE    ,
   CLI_CMD_CONNECT  ,
   CLI_CMD_RECV     ,
   CLI_CMD_SEND     ,
};

// ---------------

// internal helper functions
/// @cond hides_from_doxygen
static void run(void);
static void runConnectedIdle(void);
static void runNotConnected(void);
static void runRecvMessage(void);
static void runSendMessage(void);
/// @endcond

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Prepares the generic client for the CLOSE command.

    @memberof Client
*/

TS2API void genCli_close(void)
{
   genCli.command = CLI_CMD_CLOSE;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Prepares the generic client for the CONNECT command.

    @memberof Client
*/

TS2API void genCli_connect(void)
{
   genCli.operation = "CONNECT";
   genCli.command = CLI_CMD_CONNECT;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the error that occurred last.

    @memberof Client
*/

TS2API ushort genCli_error(void)
{
   return genCli.error;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the type of the error that occurred last.

    @memberof Client
*/

TS2API ushort genCli_errorType(void)
{
   return genCli.errorType;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the event that occurred last.

    @memberof Client
*/

TS2API ushort genCli_event(void)
{
   return genCli.event;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** initializes the generic client.

    Performs the client library initialization, creates the generic client's
      working message

    @memberof Client
*/

TS2API void genCli_init(void)
{
   client_init();

   client_createClientSocket();

   genCli.message = message_create(0);

   genCli.state = CLI_ST_NOT_CONNECTED;
   genCli.command = CLI_CMD_CONNECT;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the message associated with an event.

    @memberof Client
*/

TS2API Message* genCli_message(void)
{
   return genCli.message;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the name of the last operation performed (for error reporting).

    @return
    then name of the last operation: CONNECT, RECV, SEND

    @memberof Client
*/

TS2API cchar* genCli_operation(void)
{
   return genCli.operation;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Prepares the generic client for the RECV command.

    @memberof Client
*/

TS2API void genCli_recv(void)
{
   genCli.operation = "RECV";
   genCli.command = CLI_CMD_RECV;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Prepares the generic client for the SEND command.

    @memberof Client
*/

TS2API void genCli_send(void)
{
   genCli.operation = "SEND";
   genCli.command = CLI_CMD_SEND;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Configures the option for automatic retry when connecting to server.

    @memberof Client
*/

TS2API void genCli_setRetryConnect(bool option)
{
   genCli.shouldRetryConnect = option;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Runs the generic client application and returns an event to the appilcation.

    @return
    a pointer to a GenericClient instance.

    @memberof Client
*/

TS2API ushort genCli_waitEvent(void)
{
   run();
   return genCli.event;
}

// -----------------------------------------------------------------------------
// INTERNAL HELPER FUNCTIONS
// -----------------------------------------------------------------------------

/** Runs the generic client application.

    @private @memberof Client
*/

static void run(void)
{
   switch (genCli.state)
   {
      case CLI_ST_NOT_CONNECTED:
         runNotConnected();
         break;

      case CLI_ST_CONNECTED_IDLE:
         runConnectedIdle();
         break;

      default: // ???
         abort();
         break;
   }
}

// -----------------------------------------------------------------------------

/** Runs the generic client application with command CLOSE.

    private @memberof Client
*/

static void runClose(void)
{
   client_closeConnection();

   genCli.state = CLI_ST_NOT_CONNECTED;

   genCli.event = CLI_EVT_CONNECTION_DESTROYED;
   genCli.error = 0;
   genCli.errorType = CLI_ERR_FORCED_CLOSE;
}

// -----------------------------------------------------------------------------

/** Runs the generic client application in the state CONNECTED_IDLE.

    @private @memberof Client
*/

static void runConnectedIdle(void)
{
   switch (genCli.command)
   {
      case CLI_CMD_SEND:
         runSendMessage();
         break;

      case CLI_CMD_RECV:
         runRecvMessage();
         break;

      case CLI_CMD_CLOSE:
         runClose();
         break;

      default:
         genCli.event = CLI_EVT_INVALID_COMMAND;
         break;
   } // switch
}

// -----------------------------------------------------------------------------

/** Runs the generic client application in the state NOT_CONNECTED.

    @private @memberof Client
*/

static void runNotConnected(void)
{
   if (genCli.command != CLI_CMD_CONNECT)
   {
      genCli.event = CLI_EVT_INVALID_COMMAND;
      return;
   }

   for (;;)
   {
      if (client_connect())
      {
         genCli.state = CLI_ST_CONNECTED_IDLE;
         genCli.event = CLI_EVT_CONNECTION_CREATED;
         return;
      }

      if (!genCli.shouldRetryConnect)
      {
         genCli.event = CLI_EVT_CONNECT_ERROR;
         return;
      }

      client_sleep(1); // try again, but sleep 1 second, to avoid spinning
   } // for
}

// -----------------------------------------------------------------------------

/** Runs the generic client application with command RECV.

    private @memberof Client
*/

static void runRecvMessage(void)
{
   if (client_recvMessage(genCli.message))
      genCli.event = CLI_EVT_RECV_COMPLETE;

   else if (client_timeout())
      genCli.event = CLI_EVT_RECV_TIMEOUT;

   else
   {
      // the connection has been closed
      genCli.state = CLI_ST_NOT_CONNECTED;
      genCli.event = CLI_EVT_CONNECTION_DESTROYED;
      genCli.error = client_error();
      genCli.errorType = client_errorType();
   }
}

// -----------------------------------------------------------------------------

/** Runs the generic client application with command SEND.

    @private @memberof Client
*/

static void runSendMessage(void)
{
   if (client_sendMessage(genCli.message))
   {
      genCli.event = CLI_EVT_SEND_COMPLETE;
   }
   else
   {
      // the connection has been closed
      genCli.state = CLI_ST_NOT_CONNECTED;
      genCli.event = CLI_EVT_CONNECTION_DESTROYED;
      genCli.error = client_error();
      genCli.errorType = client_errorType();
   }
}

// -----------------------------------------------------------------------------
// the end
