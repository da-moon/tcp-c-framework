/// @source      Socket.c
/// @description Implementation of the Socket facility.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations (always include first)
#include "config.h"

// C language includes
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>  // abort

// Windows-specific includes
#if PLATFORM(Windows)
#pragma warning(disable: 4115)
#include <windows.h>
#endif

// Linux-specific includes
#if PLATFORM(Linux)
#include <fcntl.h>
#include <arpa/inet.h>  // inet_ntoa
#include <netinet/in.h> // sockets
#include <signal.h>
// MSVC defines these in stdlib.h
   #define max(a,b) ((a) > (b) ? (a) : (b))
   #define min(a,b) ((a) < (b) ? (a) : (b))
#endif

// framework includes
#include "Socket.h"        /* socket_xxx functions  */
#include "util/Log.h"      /* log_xxx functions     */
#include "util/Thread.h"   /* thread_xxx functions  */
#include "util/Timeout.h"  /* timeout_xxx functions */

// -----------------------------------------------------------------------------

// global declarations

/// Socket class.
/// A class that provides low level access to the Berkeley sockets used in the
/// framework.
/// @class Socket

/// Default server port
/// @private @memberof Socket
enum DefaultSocketServerPort
{
   SOCKET_SERVER_PORT_DEF = 4000 ///< TCP service port
}; 

/// Source identification for the Log file
/// @private @memberof Socket
static cchar sourceID[] = "ZSO";

/// Convenience macro: length of socket address structure
/// @private @memberof Socket
enum SockAddrLen
{
   SOCKADDR_LEN = sizeof(struct sockaddr)
};

// ---------------------

/// Maximum fd being watched by select
/// @private @memberof Socket
static int max_fd = -1;

/// Maximum read fd being watched by select
/// @private @memberof Socket
static int max_fd_read = -1;

/// Maximum write fd being watched by select
/// @private @memberof Socket
static int max_fd_write = -1;

/// Input read fd set to select
/// @private @memberof Socket
static fd_set fds_read_base;

/// Input write fd set to select
/// @private @memberof Socket
static fd_set fds_write_base;

/// Output read fd set from select
/// @private @memberof Socket
static fd_set fds_read_actual;

/// Output write fd set from select
/// @private @memberof Socket
static fd_set fds_write_actual;

// ---------------------

// for use by GenericClient
static int partialLenReceived;

// ---------------------

// internal helper function prototypes
/// @cond hides_from_doxygen
static void bindServerSocket(int, short);
static int  createTCPsocket(void);
static void prepareServerSocket(struct sockaddr_in*, short);
static void reuseEndpoint(int);
static void startListening(int);
static int waitRecvEvent(int, uint);
static int waitSendEvent(int, uint);
/// @endcond

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Starts watching a socket for read events.

    @param [in]
    socket : socket descriptor.

    @memberof Socket
*/

TS2API void socket_addSocketForReading(int socket)
{
   log_func(socket_addSocketForReading);
   log_fdebug("starting to watch a socket for read events");

   max_fd_read = max(max_fd_read, socket);
   max_fd = max(max_fd_read, max_fd);
   FD_SET(socket, &fds_read_base);
   log_fdebug("max_fd now: %d", max_fd);

}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Starts watching a socket for write events.

    @param [in]
    socket : socket descriptor.

    @memberof Socket
*/

TS2API void socket_addSocketForWriting(int socket)
{
   log_func(socket_addSocketForWriting);
   log_fdebug("starting to watch a socket for write events");

   max_fd_write = max(max_fd_write, socket);
   max_fd = max(max_fd_write, max_fd);
   FD_SET(socket, &fds_write_base);
   log_fdebug("max_fd now: %d", max_fd);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Connects a client socket with a server.

    @param [in]
    clientSocket : the client socket to be connected

    @param [in]
    serverAddress : IP address of the server

    @param [in]
    port : the port on which the server waits for connection requests

    @return
    true if successful, false if not

    @memberof Socket
*/

TS2API bool socket_connect(int clientSocket, uint serverAddress, ushort port)
{
   struct sockaddr_in sin; // server socket control structure 

   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = serverAddress;
   sin.sin_port = htons(port);

   return (connect(clientSocket, (struct sockaddr*)&sin, sizeof(sin)) == 0);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Creates a client TCP socket.

    @return
    clientSocket : a socket descriptor

    @memberof Socket
*/

TS2API int socket_createClientSocket(void)
{
   int clientSocket;

   log_func(socket_createClientSocket);
   log_finfo("creating a client socket");

   clientSocket = createTCPsocket();

   return clientSocket;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Creates a server TCP socket.
    Sets up a server socket which will listen for connection requests on a
    specific port, and starts listening for incoming connection requests.

    @param [in]
    port : TCP service port

    @return
    serverSocket : socket descriptor of server socket, already listening.

    @memberof Socket
*/

TS2API int socket_createServerSocket(ushort port)
{
   int serverSocket;

   log_func(socket_createServerSocket);
   log_finfo("creating the server socket");

   log_finfo("creating a TCP socket");
   serverSocket = createTCPsocket();

   log_finfo("setting reuse endpoint option");
   reuseEndpoint(serverSocket);

   log_finfo("binding the socket to the service port");
   bindServerSocket(serverSocket, port);

   log_finfo("starting to listen for connection requests");
   startListening(serverSocket);

   return serverSocket;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Creates a connected pair of local sockets.
    @fn socket_createSocketPair(int* sender, int* receiver)

    @param [out]
    sender : "client" socket.

    @param [out]
    receiver : "server" socket

    @post
    Aborts application on failure.

    @memberof Socket
*/

#if PLATFORM(Linux)
void socket_createSocketPair(int* sender, int* receiver)
{
   int sockets[2];

   log_func(socket_createSocketPair);
   log_finfo("creating a local socket pair for IPC");

   // uses Linux system call
   if (socketpair(AF_LOCAL, SOCK_STREAM, PF_LOCAL, sockets) < 0)
   {
      int err = socket_error();
      log_ffatal("socketpair: error %d", err);
      abort();
   }

   *sender = sockets[0];
   *receiver = sockets[1];
}
#endif

#if PLATFORM(Windows)
TS2API void socket_createSocketPair(int* sender, int* receiver)
{
   int listener;
   struct sockaddr_in addr;
   socklen_t addrlen = sizeof(addr);

   int err, step;

   log_func(socket_createSocketPair);
   log_finfo("creating a local socket pair for IPC");

   log_finfo("creating the listener");
   listener = createTCPsocket();

   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = inet_addr("127.1");
   addr.sin_port = 0; // bind will choose a "random" port

   step = 0;
   // binds the socket to a "random" local port
   log_finfo("binding the listener");
   if (bind(listener, (struct sockaddr*)&addr, SOCKADDR_LEN) < 0)
      goto L_ERROR;

   step = 1;
   // retrieves the "random" local port that was bound
   log_finfo("getting the listener port");
   if (getsockname(listener, (struct sockaddr*)&addr, &addrlen) < 0)
      goto L_ERROR;

   step = 2;
   // starts listening on the bound port
   log_finfo("starting the listener listen for connection requests");
   if (listen(listener, 1) < 0)
      goto L_ERROR;

   step = 3;
   // creates the "sender" socket and connects it to the listener socket above
   log_finfo("creating the sender");
   *sender = createTCPsocket();
   log_finfo("connecting the sender to the listener");
   if (connect(*sender, (struct sockaddr*)&addr, sizeof(addr)) < 0)
      goto L_ERROR;

   step = 4;
   // now the listener socket accepts the connection request from the "sender"
   // and creates the "receiver" socket
   log_finfo("creating the receiver");
   if ((*receiver = accept(listener, 0, 0)) < 0)
      goto L_ERROR;

   // closes the listener, it is not needed anymore
   log_finfo("closing the listener");
   socket_close(listener);
   return;

L_ERROR:
   err = socket_error();
   log_ffatal("socketpair: error %d in step %d", err, step);
   abort();
}
#endif // PLATFORM(Windows)

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Creates a worker TCP socket.
    Waits for and accepts an incoming connection request.

    @param [in]
    serverSocket : descriptor of listening socket.

    @param [out]
    clientIPaddr : IP address of the newly connected client.

    @param [out]
    clientPort : port used by the client

    @return
    New connected socket descriptor, or -1 if error.

    @memberof Socket
*/

TS2API int socket_createWorkerSocket(int serverSocket, uint* clientIPaddr,
   ushort* clientPort)
{
   int wrkSocket;
   struct sockaddr_in sa;
   socklen_t sockaddr_len = SOCKADDR_LEN;

   log_func(socket_createWorkerSocket);
   log_fdebug("creating a worker socket");

   wrkSocket = accept(serverSocket, (struct sockaddr*)&sa, &sockaddr_len);

   if (wrkSocket == -1)
   {
      int err = socket_error();
      log_fwarn("error %d in accept", err);
      return -1;
   }

   // returns the IP address only if asked
   if (clientIPaddr)
      *clientIPaddr = sa.sin_addr.s_addr;

   // returns the port only if asked
   if (clientPort)
      *clientPort = htons(sa.sin_port);

   return wrkSocket;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Initializes the Socket class.
    @fn void socket_init(void)

    Also, platform-specific initialization.

    @memberof Socket
*/

#if PLATFORM(Linux)
void socket_init(void)
{
   log_func(socket_init);
   log_finfo("Socket class inicialization");

   // don't let the system abort the application when it tries to send bytes
   // through a connection already closed by the client
   signal(SIGPIPE, SIG_IGN);
}
#endif

#if PLATFORM(Windows)
TS2API void socket_init(void)
{
   WSADATA wsaData;

   log_func(socket_init);
   log_finfo("Socket class inicialization");

   // mumbo-jumbo needed by Windows
   if (WSAStartup(0x202, &wsaData) == E_SOCKET_ERROR)
   {
      // I guess there's nothing more we can do...
      int err = socket_error();
      log_ffatal("error %d in WSAStartup", err);
      WSACleanup();
      abort();
   }
}
#endif

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Formats an IP address.

    @param [in]
    ipAddr : the IP address to be formatted

    @param [out]
    ipAddrString : buffer where to format the IP address

    @memberof Socket
*/

TS2API void socket_ipAddrToString(uint ipAddr, char* ipAddrString)
{
   uchar* p = (uchar*)&ipAddr;
   assert(ipAddrString);
   sprintf(ipAddrString, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Checks if a socket is ready for reading.

    @param [in]
    socket : socket descriptor

    @memberof Socket
*/

TS2API bool socket_isReadyForReading(int socket)
{
   return (FD_ISSET(socket, &fds_read_actual));
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Checks if a socket is ready for writing.

    @param [in]
    socket : socket descriptor

    @memberof Socket
*/

TS2API bool socket_isReadyForWriting(int socket)
{
   return (FD_ISSET(socket, &fds_write_actual));
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the partial number of bytes received up until a timeout.

    @return
    the number of bytes received up until timeout occured

    @memberof Socket
*/

TS2API int socket_partialLenReceived(void)
{
   return partialLenReceived;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Tries to receive some bytes from a socket.

    @param [in]
    socket : connected socket descritor

    @param [out]
    buffer : reception buffer

    @param [in]
    length : number of bytes to receive

    @return
    number of bytes that were received, or -1 if error

    @memberof Socket
*/

TS2API int socket_recv(int socket, char* buffer, int length)
{
   log_func(socket_recv);
   log_fdebug("reading something from a socket");

   assert(buffer);
   return recv(socket, buffer, length, 0);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Receives an exact number of bytes from the peer application.

    @param [in]
    socket : socket descritor

    @param [out]
    buf : reception buffer

    @param [in]
    len : number of bytes that <i> must </i> be received in this call

    @param [in]
    nSeconds : number of seconds in which the buffer <i> must </i> arrive

    @param [in]
    trace : whether to write a trace of the received bytes

    @return
    number of bytes received, or 0 if connection closed, or -1 if error
    or -2 if timeout

    @memberof Socket
*/

TS2API int socket_recvBuffer(int socket, char* buf, int len, uint nSeconds,
   bool trace)
{
   Timeout timeout;
   int pendingLen = len;
   int nEvents, receivedLen = 0;

   timeout_init(&timeout, nSeconds);

   socket_setNonBlocking(socket, true);

   partialLenReceived = 0;

   // while there's something to receive
   while (pendingLen > 0)
   {
      // waits for some bytes to arrive
      nEvents = waitRecvEvent(socket, timeout_remaining(&timeout));

      if (nEvents == -1) // error
      {
         receivedLen = -1;
         break;
      }

      if (nEvents == 0)
      {
         receivedLen = E_SOCKET_TIMEOUT; // timeout 
         break;
      }

      // tries to receive the remaining data
      len = recv(socket, buf, pendingLen, 0);

      if (len == 0)
      {
         receivedLen =  0; // connection closed
         break;
      }

      if (len < 0) // error ?
      {
         int err = socket_error();
         if (socket_shouldRetry(err) || socket_wouldBlock(err))
            continue;
         receivedLen =  -1; // error
         break;
      }

      // ok, got "len" bytes
      if (trace)
      {
         ushort port;
         struct sockaddr_in sa;
         socklen_t addrlen = sizeof(sa);
         char bufPort[50];
         getsockname(socket, (struct sockaddr*)&sa, &addrlen);
         port = htons(sa.sin_port);
         sprintf(bufPort, "received in port:%d", port);
         log_trace(bufPort, buf, len);
      }

      partialLenReceived += len; // for use by GenericClient

      pendingLen -= len;   // adjusts pending len
      receivedLen += len;  // adjusts received len so far
      buf += len;          // advances reception buffer

   } // while

   socket_setNonBlocking(socket, false);
   return receivedLen;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Stops monitoring a socket for read events.

    @param [in]
    socket : socket descriptor.

    @memberof Socket
*/

TS2API void socket_removeSocketFromReading(int socket)
{
   log_func(socket_removeSocketFromReading);
   log_fdebug("stopping monitoring a socket from reading");

   if (socket == max_fd_read)
   {
      max_fd_read--;
      max_fd = max(max_fd_read, max_fd_write);
      log_fdebug("max_fd now: %d", max_fd);
   }

   FD_CLR(socket, &fds_read_base);
   FD_CLR(socket, &fds_read_actual);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Stops monitoring a socket for write events.

    @param [in]
    socket : socket descriptor.

    @memberof Socket
*/

TS2API void socket_removeSocketFromWriting(int socket)
{
   log_func(socket_removeSocketFromWriting);
   log_fdebug("stopping monitoring a socket from writing");

   if (socket == max_fd_write)
   {
      max_fd_write--;
      max_fd = max(max_fd_read, max_fd_write);
      log_fdebug("max_fd now: %d", max_fd);
   }

   FD_CLR(socket, &fds_write_base);
   FD_CLR(socket, &fds_write_actual);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Tries to send some bytes through a socket.

    @param [in]
    socket : connected socket descritor

    @param [in]
    buffer : bytes to be sent

    @param [in]
    length : number of bytes to send

    @return
    number of bytes that were sent, or -1 if error

    @memberof Socket
*/

TS2API int socket_send(int socket, cchar* buffer, int length)
{
   log_func(socket_send);
   log_fdebug("sending something through a socket");

   assert(buffer);
   return send(socket, buffer, length, 0);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Sends  an exact number of bytes to the peer application.

    @param [in]
    socket : socket descritor

    @param [in]
    msgBuf : message buffer

    @param [in]
    msgLen : number of bytes that <i> must </i> be sent in this call

    @param [in]
    nSeconds : number of seconds to conclude the operation

    @param [in]
    trace : whether to write a trace of the bytes sent

    @return
    number of bytes that were sent, or -1 if error, or -2 if timeout

    @memberof Socket
*/

TS2API int socket_sendBuffer(int socket, cchar* msgBuf, int msgLen, uint nSeconds,
   bool trace)
{
   Timeout timeout;
   int pendingLen = msgLen;
   char* buf = (char*)msgBuf;

   timeout_init(&timeout, nSeconds);

   socket_setNonBlocking(socket, true);

   // tries to send exactly "msgLen" bytes to peer
   while (pendingLen > 0)
   {
      int partialLen;
      int nEvents = waitSendEvent(socket, timeout_remaining(&timeout)); 

      if (nEvents == -1) // error
      {
         msgLen = -1;
         break;
      }

      if (nEvents == 0)
      {
         msgLen = E_SOCKET_TIMEOUT; // timeout 
         break;
      }

      // tries to send the remaining data
      partialLen = send(socket, buf, pendingLen, 0);
      if (partialLen < 0) // error ?
      {
         int err = socket_error();
         if (socket_shouldRetry(err) || socket_wouldBlock(err))
            continue;
         msgLen = -1; // error
         break;
      }

      if (trace)
      {
         ushort port;
         struct sockaddr_in sa;
         socklen_t addrlen = sizeof(sa);
         char bufPort[50];
         getsockname(socket, (struct sockaddr*)&sa, &addrlen);
         port = htons(sa.sin_port);
         sprintf(bufPort, "sent from port:%d", port);
         log_trace(bufPort, buf, partialLen);
      }
      pendingLen -= partialLen;
      buf += partialLen;
   } // while

   socket_setNonBlocking(socket, false);
   return msgLen;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Sets socket to nonblocking mode.
    @fn void socket_setNonBlocking(int socket, bool nonBlocking)

    @param [in]
    socket : descriptor of socket to be put in nonblocking mode

    @param [in]
    nonBlocking : flag that says to enter or leave nonblocking mode

    @post
    aborts application if not successful

    @memberof Socket
*/

#if PLATFORM(Linux)
TS2API void socket_setNonBlocking(int socket, bool nonBlocking)
{
   int flags = fcntl(socket,F_GETFL,0);
   if (nonBlocking)
      flags |= O_NONBLOCK;
   else
      flags &= ~O_NONBLOCK;
   fcntl(socket, F_SETFL, flags);
}
#endif

#if PLATFORM(Windows)
TS2API void socket_setNonBlocking(int socket, bool _mode)
{
   ulong mode = _mode;
   ioctlsocket(socket, FIONBIO, &mode);
}
#endif

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Sets socket not ready for reading, after processing a read event.

    @param [in]
    socket : descriptor of socket to be set no ready for reading

    @memberof Socket
*/

TS2API void socket_setNotReadyForReading(int socket)
{
   FD_CLR(socket, &fds_read_actual);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Sets socket not ready for writing, after processing a write event.

    @param [in]
    socket : descriptor of socket to be set not ready for writing

    @memberof Socket
*/

TS2API void socket_setNotReadyForWriting(int socket)
{
   FD_CLR(socket, &fds_write_actual);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Waits for events from registered sockets.

    @return
    number of events (sockets marked for reading/writing).

    @memberof Socket
*/

TS2API int socket_waitEvents(void)
{
   int nEvts;  // number of events after calling select

   log_func(socket_waitEvents);
   log_fdebug("waiting for events");

   for (;;)
   {
      // updates the fd's to be used by the select function
      fds_read_actual = fds_read_base;
      fds_write_actual = fds_write_base;

      // don't care about third fd_set
      // fds_err_actual = fds_err_base;

      // waits for something to happen
      nEvts = select(max_fd+1, &fds_read_actual, &fds_write_actual, NULL, NULL);

      // error ?
      if (nEvts == -1)
      {
         int err = socket_error();

         if (socket_shouldRetry(err))
         {
            log_ferror("unexpected error %d in select!", err);
            continue;
         }

         log_ffatal("don't know what to do, error %d in select!", err);
         abort();
      } // error

      if (nEvts == 0)
      {
         log_ffatal("don't know what to do, zero events in select!");
         abort();
      } // nEvts == 0

      break; // select was successful
   } // for

   log_fdebug("sommething happened, an event occurred");
   return nEvts;
}

// -----------------------------------------------------------------------------
// INTERNAL HELPER FUNCTIONS
// -----------------------------------------------------------------------------

/** Binds server socket to IP address and service port.

    @param [in]
    serverSocket : server socket descriptor.

    @param [in]
    port : TCP service port

    @post
    Either succeeds or aborts application.

    @private @memberof Socket
*/

static void bindServerSocket(int serverSocket, short port)
{
   struct sockaddr_in sscs; // server socket control structure 

   log_func(bindServerSocket);
   log_finfo("binding a socket to a service port");

   prepareServerSocket(&sscs, port);

   if (bind(serverSocket, (struct sockaddr*)&sscs, SOCKADDR_LEN) < 0)
   {
      // if bind fails (even with reuse address) there's nothing more we can do
      // so we abort the application
      int err = socket_error();
      log_ffatal("error in bind: %d", err);
      abort();
   }
}

// -----------------------------------------------------------------------------

/** Creates a TCP socket.

    @return
    a TCP socket descriptor

    @post
    Either succeeds or aborts application

    @private @memberof Socket
*/

static int createTCPsocket(void)
{
   int tcpSocket;

   log_func(createTCPsocket);
   log_finfo("creating a TCP socket");

   if ((tcpSocket=socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      // if we cannot create a socket there's something very wrong with the
      // system, so we abort the application
      int err = socket_error();
      log_ffatal("* error creating socket: %d", err);
      abort();
   }

   return tcpSocket;
}

// -----------------------------------------------------------------------------

/** Initializes a socket address structure.

    @param [out]
    psscs : server socket control structure

    @param [in]
    port : TCP service port

    @private @memberof Socket
*/

static void prepareServerSocket(struct sockaddr_in* psscs, short port)
{
   psscs->sin_family = AF_INET;
   psscs->sin_addr.s_addr = INADDR_ANY;
   psscs->sin_port = htons(port);
}

// -----------------------------------------------------------------------------

/** Tells <i> bind </i> to reuse endpoint.

    @param [in]
    serverSocket : server socker descriptor

    @post
    Either succeeds or aborts application

    @private @memberof Socket
*/

static void reuseEndpoint(int serverSocket)
{
   int c = 1; // "true"

   log_func(reuseEndpoint);
   log_finfo("setting the reuse endpoint option on a socket");

   if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&c, sizeof c))
   {
      // if we can't set an option then there's something very wrong with the
      // system, I guess there's not much sense running the application
      int err = socket_error();
      log_ffatal("error %d in setsockopt", err);
      abort();
   }
}

// -----------------------------------------------------------------------------

/** Starts listening for connection requests.

    @param [in]
    serverSocket : socket descriptor of server socket

    @post 
    Either succeeds or aborts application.

    @private @memberof Socket
*/

static void startListening(int serverSocket)
{
   log_func(startListening);
   log_finfo("starting to listen for connection requests");

   // backlog = 5 (traditional)
   if (listen(serverSocket, 5) < 0)
   {
      // if the socket can't listen then there's no server application...
      int err = socket_error();
      log_ffatal("error in listen: %d", err);
      abort();
   }
}

// -----------------------------------------------------------------------------

/** Waits until there's some bytes available to receive, or timeout occurs.

    @param [in]
    socket : connected socket descriptor

    @param [in]
    nSeconds : timeout to wait until some bytes are available

    @return
    number of events available (-1, 0, 1)

    @private @memberof Socket
*/

static int waitRecvEvent(int socket, uint nSeconds)
{
   fd_set readfds;
   struct timeval timeout = { nSeconds, 0 };
   struct timeval* pTimeout = (nSeconds == TIMEOUT_WAIT_FOREVER) ? NULL
      : &timeout;

   FD_ZERO(&readfds);
   FD_SET(socket,&readfds);

   return select(socket+1, &readfds, NULL, NULL, pTimeout);
}

// -----------------------------------------------------------------------------

/** Waits until it's possible to send some bytes through the socket without
    blocking.

    @param [in]
    socket : connected socket descriptor

    @param [in]
    nSeconds : timeout to wait until the send event occurs

    @return
    number of events available (-1, 0, 1)

    @private @memberof Socket
*/

static int waitSendEvent(int socket, uint nSeconds)
{
   fd_set writefds;
   struct timeval timeout = { nSeconds, 0 };
   struct timeval* pTimeout = (nSeconds == TIMEOUT_WAIT_FOREVER) ? NULL
      : &timeout;

   FD_ZERO(&writefds);
   FD_SET(socket,&writefds);

   return select(socket+1, NULL, &writefds, NULL, pTimeout);
}

// -----------------------------------------------------------------------------
// the end
