/// @source      Client.c
/// @description Implementation of class Client.
/// The client functions are not really part of the framework, but are provided
/// as a convenience for testing the framework.
//
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations
#include "config.h"

// C language includes
#include <assert.h>
#include <stdlib.h> // abort
#include <string.h>

// Windows-specific includes
#if PLATFORM(Windows)
#pragma warning(disable: 4115)
#include <windows.h>
#endif

// Linux-specific includes
#if PLATFORM(Linux)
#include <arpa/inet.h>  // inet_ntoa
 #endif

// framework includes
#include "Client.h"        /* client_xxx functions  */
#include "Socket.h"        /* socket_xxx functions  */
#include "util/Log.h"      /* log_xxx functions     */
#include "util/Timeout.h"  /* timeout_xxx functions */

// -----------------------------------------------------------------------------

// global declarations

/// Client class.
/// This class provides convenience members and methods to write simple client
/// applications.
/// @class Client

/// Identification for the Log file
/// @private @memberof Client
static cchar sourceID[] = "ZCL";

/// Global counter used when sending messages
/// @private @memberof Client
static uint orgSeqNo;

/// Default server IP address in binary (127.1)
/// @private @memberof Client
static uint   serverAddress = 0x7F000001;

/// Default server IP address as string
/// @private @memberof Client
static char   serverAddressStr[50] = "127.1";

/// Default server TCP port
/// @private @memberof Client
static ushort servicePort = 4000;

/// The client socket
/// @private @memberof Client
static int clientSocket;

/// Default server reply timeout in seconds.
/// (0 means immediate, -1 means infinite).
/// @private @memberof Client
static uint replyTimeout = TIMEOUT_WAIT_FOREVER;

/// Default timeout in seconds when sending a message to the server.
/// In principle the client doesn't need to mess with this, but it's here anyway
/// @private @memberof Client
static ushort sendTimeout = 5;

/// State of the connection
/// @private @memberof Client
static bool isConnected;

// ---------------

// State of the client, valid only immediately after a socket operation

/// Valid only after a socket operation 
/// @private @memberof Client
static bool connectionClosed;  

/// Valid only after a socket operation 
/// @private @memberof Client
static bool formatError;

/// Valid only after a socket operation 
/// @private @memberof Client
static int  lastSocketError;

/// Valid only after a recv socket operation 
/// @private @memberof Client
static int  lastErrorType;

// private interface with GenericClient (sync if changed in either place)
// see also client_errorType
enum // for lastErrorType
{
   ERRTYPE_CLOSED = 1  ,  // connection closed by server
   ERRTYPE_SOCKET      ,  // socket error
   ERRTYPE_FORMAT      ,  // invalid format on the wire
   ERRTYPE_SENDTIMEOUT ,  // timeout in send
};

/// Valid only after a socket operation 
/// @private @memberof Client
static bool timeoutOccurred;

// ---------------

/// State of tracing
/// @private @memberof Client
static bool isTracing;

// ---------------

// internal helper functions
/// @cond hides_from_doxygen
static int recvBuffer(int, char*, int, uint, bool);
/// @endcond

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Closes the connection.

    @memberof Client
*/

TS2API void client_closeConnection(void)
{
   log_func(client_closeConnection);
   log_finfo("closing the connection with the server");

   socket_close(clientSocket);
   isConnected = false;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Tries to connect with the server.

    @memberof Client
*/

TS2API bool client_connect(void)
{
   log_func(client_connect);
   log_finfo("trying to connect with the server %s", serverAddressStr);

   isConnected = false;

   if (socket_connect(clientSocket, serverAddress, servicePort))
   {
      log_finfo("ok, connection successful");
      isConnected = true;
      return true;
   }

   lastSocketError = socket_error();

   log_ferror("error %d trying to connect with the server", lastSocketError);
   return false;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the value of the flag connectionClosed. This flag is only
    valid immediately after a socket operation.

    @return
    true (connection is closed) / false (connection is not closed)

    @memberof Client
*/

TS2API bool client_connectionClosed(void)
{
   return connectionClosed;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Creates the client socket.

    @post
    either succeeds or aborts the application

    @memberof Client
*/

TS2API void client_createClientSocket(void)
{
   log_func(client_createClientSocket);
   log_finfo("creating the client socket");

   clientSocket = socket_createClientSocket();

// don't do it for now 
// socket_setNonBlocking(clientSocket);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the value of the error that occurred immediately after a socket
    operation.

    @return
    number of error that occurred immediately after a socket operation

    @memberof Client
*/

TS2API uint client_error(void)
{
   return lastSocketError;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the type of the error that occurred immediately after a socket
    operation.

    @return
    type of the error (only for use of the class GenericClient)

    @memberof Client
*/

TS2API ushort client_errorType(void)
{
   return lastErrorType;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the value of the flag formatError. This flag is only
    valid immediately after a socket operation.

    @return
    true (a format error occurred) / false (a format error not occurred)

    @memberof Client
*/

TS2API bool client_formatError(void)
{
   return formatError;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Initializes the Client façade class.

    @memberof Client
*/

TS2API void client_init(void)
{
   log_func(client_init);
   log_finfo("Client class inicialization");

   socket_init();
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Checks if the cliente connected to the server.

    @return
    true (is connected) / false (is not connected)

    @memberof Client
*/

TS2API bool client_isConnected(void)
{
   return isConnected;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Receives a message from the server.

    @param [in]
    msg : message which will be used to store data received from the server

    @return
    true (reception successful) / false (reception failure)

    @memberof Client
*/

// TODO: should return TIMEOUT only when nothing was received. In case of a
// partial message received, should return error (format error)

TS2API bool client_recvMessage(Message* msg)
{
   int ret;
   Timeout timeout;
   log_func(client_recvMessage);
   static cchar msgclose[] = "now closing connection";

   /// @cond hides_from_doxygen
   #define REM_TIME timeout_remaining(&timeout)
   /// @endcond

   assert(msg);

   timeout_init(&timeout, replyTimeout);

   // tries to receive the message's first part
   log_fdebug("trying to receive %d bytes (first part of message)",
      message_size1());

   ret = recvBuffer(clientSocket, message_start(msg), message_size1(),
      REM_TIME, true /*first part of message*/);

   if (ret != (int)message_size1())
   {
      log_ferror("error when receiving first part of message from server");

      // if timeout and not one byte has arrived then don't close connection
      if (timeoutOccurred && (lastErrorType != ERRTYPE_FORMAT))
         return false;
      goto ERROR_WITH_CLOSE;
   }

   // consistency check
   if (!message_isValidPrefix(msg))
   {
      log_ferror("invalid message prefix [%c] received from server",
         message_prefix(msg));
      goto FORMAT_ERROR_WITH_CLOSE;
   }

   // consistency check
   if (message_size(msg) > message_maxBodySize())
   {
      log_ferror("invalid message size [%d] received from server",
          message_size(msg));
      goto FORMAT_ERROR_WITH_CLOSE;
   }

   // tries to receive the message's second part
   log_fdebug("trying to receive %d bytes (second part of message)",
      message_size2(msg));

   ret = recvBuffer(clientSocket, message_start(msg)+message_size1(),
      message_size2(msg), REM_TIME, false /*not first part of message*/);

   if (ret != (int)message_size2(msg))
   {
      log_ferror("error when receiving seconf part of message from server");
      goto ERROR_WITH_CLOSE;
   }

   // consistency check
   if (!message_isValidSuffix(msg))
   {
      log_ferror("invalid message suffix [%c] received from server",
         message_suffix(msg));
      goto FORMAT_ERROR_WITH_CLOSE;
   }

   log_fdebug("received %d bytes from server, body:%d orgId:%d orgSeq:%d",
      message_size1()+message_size2(msg), message_size(msg),
      message_orgId(msg), message_orgSeqNo(msg));

   return true;

FORMAT_ERROR_WITH_CLOSE:
   formatError = true;
   lastErrorType = ERRTYPE_FORMAT;
ERROR_WITH_CLOSE:
   log_fwarn(msgclose);
   client_closeConnection();
   return false;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Tries to send a message to the server.

    @param [in]
    msg : message to be sent

    @return
    true (operation successful) / false (operation failed)

    @memberof Client
*/

TS2API bool client_sendMessage(Message* msg)
{
   int ret, size;
   log_func(client_sendMessage);

   assert(msg);

   // to make it easier to find this message in the server's log
   message_setOrgId(msg, thread_pid());
   message_setOrgSeqNo(msg, orgSeqNo++);

   size = message_size1() + message_size2(msg);

   // clears indicators
   lastSocketError = 0;
   lastErrorType = 0;
   timeoutOccurred = false;

   // tries to send the message
   log_fdebug("sending %d bytes to server, body:%d orgId:%d orgSeq:%d",
      size, message_size(msg), message_orgId(msg), message_orgSeqNo(msg));

   ret = socket_sendBuffer(clientSocket, message_start(msg), size, sendTimeout,
      isTracing);

   if (ret == size)
   {
      log_fdebug("ok, message sent successfully");
      return true; // ok
   }

   if (ret == E_SOCKET_TIMEOUT)
   {
      log_fwarn("timeout occurred when writing socket");
      timeoutOccurred = true;
      lastErrorType = ERRTYPE_SENDTIMEOUT;
   }

   else // if (ret == SOCKET_ERROR)
   {
      lastSocketError = socket_error();
      log_ferror("error %d when writing socket", lastSocketError);
      lastErrorType = ERRTYPE_SOCKET;
   }

   log_fwarn("now closing connection");
   client_closeConnection();

   return false;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Configures how many seconds to wait for a reply from the server.
    The value 0 means infinite.

    @param [in]
    seconds : how many seconds to wait for a reply from the server.

    @memberof Client
*/

TS2API void client_setReplyTimeout(ushort seconds)
{
   log_func(client_setReplyTimeout);
   log_finfo("reply timeout: %d seconds", seconds);

   replyTimeout = seconds;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Configures the server IP address.

    @param [in]
    ipAddress : IP address of the server

    @memberof Client
*/

TS2API void client_setServerAddress(cchar* ipAddress)
{
   log_func(client_setServerAddress);
   log_finfo("using server address: %s", ipAddress);

   strncpy(serverAddressStr, ipAddress, sizeof(serverAddressStr)-1);
   serverAddress = inet_addr(serverAddressStr);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Configures the server TCP port.

    @param [in]
    port : service port on which the server is waiting.

    @memberof Client
*/

TS2API void client_setServicePort(ushort port)
{
   log_func(client_setServicePort);
   log_finfo("using port: %d", port);

   servicePort = port;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Sets or clears the tracing state.

    @param [in]
    flag : true/false

    @memberof Client
*/

TS2API void client_setTrace(bool flag)
{
   isTracing = flag;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the value of the flag <i> timeoutOccurred </i>.
    This flag is only valid immediately after a socket operation.

    @return
    true (timeout occurred) / false (timeout not occurred)

    @memberof Client
*/

TS2API bool client_timeout(void)
{
   return (timeoutOccurred && (lastErrorType == ERRTYPE_FORMAT));
}

// -----------------------------------------------------------------------------
// INTERNAL HELPER FUNCTIONS
// -----------------------------------------------------------------------------

/** Receives a specific-length buffer from peer application, and checks for
    error.

    @return
    number of bytes received, or error

    @retval
    the number of bytes received, or error

    @retval
     0 : (SOCKET_CLOSED) connection was closed by the server.

    @retval
    -1 : (SOCKET_ERROR) an error occurred.

    @retval
    -2 : (SOCKET_TIMEOUT) not all bytes arrived within the specified time
    interval.

    @private @memberof Client
*/

static int recvBuffer(int socket, char* buf, int len, unsigned nSeconds,
   bool firstPart)
{
   int ret;
   log_func(recvBuffer);

   // clears indicators
   connectionClosed = false;
   formatError = false;
   lastSocketError = 0;
   lastErrorType = 0;
   timeoutOccurred = false;

   ret = socket_recvBuffer(socket, buf, len, nSeconds, isTracing);

   if (ret == len) // most likely result
      goto END;

   if (ret == E_SOCKET_CLOSED) // second most likely result
   {
      log_fwarn("connection closed when reading socket");
      connectionClosed = true;
      lastErrorType = ERRTYPE_CLOSED;
      goto END;
   }

   if (ret == E_SOCKET_TIMEOUT) // third most likely result
   {
      log_fwarn("timeout occurred when reading socket");
      // timeout not considered error if not one byte received,
      // only considered error if occurs int the middle of a message
      if (!firstPart || socket_partialLenReceived())
         lastErrorType = ERRTYPE_FORMAT;
      timeoutOccurred = true;
      goto END;
   }

   if (ret == E_SOCKET_ERROR) // fourth most likely result
   {
      lastSocketError = socket_error();
      log_ferror("error %d when reading socket", lastSocketError);
      lastErrorType = ERRTYPE_SOCKET;
      goto END;
   }

   // should not happen (!)
   abort();

END:
   return ret;
}

// -----------------------------------------------------------------------------
// the end
