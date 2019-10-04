/// @source      ConnectionManager.c
/// @description Implementation of active class ConnectionManager.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations (always include first)
#include "config.h"

// C language includes
#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // malloc

#if PLATFORM(Windows)
#define snprintf _snprintf
#endif

// framework includes
#include "ConnectionManager.h"   /* connectionManager_xxx functions */
#include "ConnectionTable.h"     /* connectionTable_xxx   functions */
#include "Message.h"             /* messageQueue_xxx functions      */
#include "MessageQueue.h"        /* messageQueue_xxx functions      */
#include "QueueManager.h"        /* queueManager_xxx functions      */
#include "Socket.h"              /* socket_xxx functions            */
#include "util/Log.h"            /* log_xxx functions               */
#include "util/Mutex.h"          /* mutex_xxx functions             */
#include "util/Queue.h"          /* queue_xxx functions             */
#include "util/Thread.h"         /* thread_xxx functions            */

// -----------------------------------------------------------------------------

// global declarations

/// ConnectionManager class.
/// This is the central class of the framework. It pumps the socket @link
/// Connection connections @endlink with data in and out, and inter-operates
/// with the application threads via the @link MessageQueue message queues
/// @endlink owned by the QueueManager.
/// @class ConnectionManager

/// Default TCP service port
/// @private @memberof ConnectionManager
enum DefaultTcpServicePort
{
   DEFAULT_TCP_SERVICE_PORT  = 4000
};

/// Identification of this source file in the Log file. 
/// @private @memberof ConnectionManager
static cchar sourceID[] = "ZCM";

/// The Socket which waits for incoming Connection requests. 
/// @private @memberof ConnectionManager
static int listeningSocket;

/// Configuration option: allows multiple connections per client IP ?
/// @private @memberof ConnectionManager
static uint singleConnectionPerIP = ST_FALSE;

/// Tcp service port
/// @private @memberof ConnectionManager
static ushort servicePort = DEFAULT_TCP_SERVICE_PORT;

/// The Socket used by the ConnectionManager to receive notifications by local
/// IPC
/// @private @memberof ConnectionManager
static int ipcReceiverSocket;

/// The Socket used to send notifications via local IPC to the
/// ConnectionManager
/// @private @memberof ConnectionManager
static int ipcSenderSocket;

/// Global origin sequence number used to stamp messages for sending.
/// @private @memberof ConnectionManager
static uint orgSeqNo;

// Command for closing a connection.
/// @cond hide_from_doxygen
typedef struct CmdCloseConnection
{
   void* connection;
   ushort connSeqNo;
} CmdCloseConnection;
/// @endcond

/// Queue of connections scheduled to be closed
/// @private @memberof ConnectionManager
static Queue* closeConnectionQueue;

/// Mutex protecting the queue of connections scheduled to be closed
/// @private @memberof ConnectionManager
static mutex_t*  ccqMutex;

/// convenience macros
#define closeConnection(c) closeConnectionEx(c,1)
#define closeConnectionForReuse(c) closeConnectionEx(c,0)

// internal helper function prototypes
/// @cond hides_doxygen
static void clearPendingOutMsgs(Connection*);
static void closeConnectionEx(Connection*, uint);
static void closeOldConnection(Connection*);
static Connection* connectionFromOutputMessage(Message*);
static void doCloseConnection(void);
static void prepareNewConnection(Connection*, int, uint, ushort);
static void processConnectionTable(int);
static void processLocalIpcEvent(void);
static void processMessagePart1(Connection*);
static void processMessagePart2(Connection*);
static void processNewConnection(void);
static char readCommand(void);
static void receivePartialMessage(Connection*);
static void sendCommand(cchar);
static void sendPartialMessage(Connection*);
static uint socketOperationResult(Connection*, int);
static void startReceivingMessages(Connection*);
static void startSendingMessage(void);
/// @endcond

// callbacks
typedef void CreateCallbackFunc(Connection*);
static CreateCallbackFunc* createCallbackFunc;

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Initializes the ConnectionManager.

    Tasks:
    - creates the listening Socket;
    - registers the listening Socket for use with select;
    - creates the Socket pair used for local IPC;
    - registers the receiver IPC Socket for use with select;
    - creates the application's ConnectionTable.

    @memberof ConnectionManager
*/

TS2API void connectionManager_init(void)
{
   log_func(connectionManager_init);
   log_finfo("ConnectionManager class inicialization");

   log_finfo("creating the server socket");
   listeningSocket = socket_createServerSocket(servicePort);
   socket_setNonBlocking(listeningSocket, true);

   log_finfo("starting monitoring the server socket");
   socket_addSocketForReading(listeningSocket);

   log_finfo("creating the IPC socket pair");
   socket_createSocketPair(&ipcSenderSocket, &ipcReceiverSocket);
// socket_setNonBlocking(ipcSenderSocket, true);
   socket_setNonBlocking(ipcReceiverSocket, true);

   log_finfo("starting monitoring the IPC receiver socket");
   socket_addSocketForReading(ipcReceiverSocket);

   log_finfo("creating the connection table");
   connectionTable_create();

   log_finfo("creating the close connection queue");
   closeConnectionQueue = queue_create(5); // configurable ???

   log_finfo("creating the mutex that protects the close connection queue");
   ccqMutex = mutex_create();
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Notifies the ConnectionManager that a new output Message is available.

    Called by another thread to notify the ConnectionManager that a new output
    Message is available for sending, in the output MessageQueue.

    @memberof ConnectionManager
*/

TS2API void connectionManager_notifyOutputMessage(void)
{
   cchar cmd = CM_SEND_MESSAGE;
   sendCommand(cmd);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Called by another thread to request that the connection manager closes a
    connection.

    The connection is passed in the connection manager command queue.

    @memberof ConnectionManager
*/

TS2API void connectionManager_requestConnectionClosing(const void* _message)
{
   ushort index;
   Connection* connection;
   char cmd = CM_CLOSE_CONNECTION;
   const Message* message = (const Message*)_message;
   CmdCloseConnection* cmdCloseConnection = (CmdCloseConnection*)
      (malloc(sizeof(CmdCloseConnection)));

   log_func(connectionManager_requestConnectionClosing);

   assert(cmdCloseConnection);

   if (!message)
   {
      log_ferror("message address is NULL!");
      return;
   }

   if (!(connection = message_connection(message)))
   {
      log_ferror("connection address is NULL!");
      return;
   }

   index = connection_index(connection);
   if (connectionTable_getActiveConnection(index) != connection)
   {
      log_ferror("invalid connection address");
      return;
   }

   cmdCloseConnection->connection = connection;
   cmdCloseConnection->connSeqNo = message_connSeqNo(message);

   mutex_lock(ccqMutex);
   queue_addTail(closeConnectionQueue, cmdCloseConnection);
   mutex_unlock(ccqMutex);

   sendCommand(cmd);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Main ConnectionManager and library function.

    Tasks:
    - waits for Socket and IPC events
    - creates new connections
    - receives messages from a Connection
    - dispatches the received messages to a worker thread
    - sends messages through a Connection.

    @memberof ConnectionManager
*/

TS2API void connectionManager_run(void)
{
   int nEvts;  // number of outstanding socket events

   log_func(connectionManager_run);
   log_finfo("starting running the connection manager");

   for (;;)
   {
      // waits until a socket or IPC event happens...
      log_fdebug("waiting for events");
      nEvts = socket_waitEvents();

      log_fdebug("found %d events", nEvts);

      // is there a new client requesting connection ?
      if (socket_isReadyForReading(listeningSocket))
      {
         log_finfo("found new connection event");
         processNewConnection();
         nEvts--;
      }

      // is there a new message to send to a client connection ?
      if (nEvts > 0 && socket_isReadyForReading(ipcReceiverSocket))
      {
         log_fdebug("found local IPC event");
         processLocalIpcEvent();
         nEvts--;
      }

      // the normal case, a read or write event for a connection
      if (nEvts > 0) 
         processConnectionTable(nEvts);
   } // for
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Configures a callback function.
    For now it can be used only when a new Connection is created, to configure
    tracing for the Connection, but in principle its use is open.

    @param [in]
    type : type of callback (ignored for now)

    @param [in]
    callback : callback function

    @memberof ConnectionManager
*/

TS2API void connectionManager_setCallback(uint type, void* callback)
{
   createCallbackFunc = (CreateCallbackFunc*)callback;
   type = 0; // ignored for now
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Configures the TCP service port.

    @param [in]
    _servicePort : TCP service port to be used by the server application.

    @memberof ConnectionManager
*/

TS2API void connectionManager_setServicePort(uint _servicePort)
{
   log_func(connectionManager_setServicePort);

   if (_servicePort)
      servicePort = (ushort) _servicePort;

   log_finfo("setting TCP service port: %d", servicePort);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Tells the ConnectionManager if multiple connections from the same IP
    address are allowed.

    If they are not allowed, the current Connection to an IP address is closed
    when a new one from that IP address arrives.

    @memberof ConnectionManager
*/

TS2API void connectionManager_setSingleConnectionMode(void)
{
   log_func(connectionManager_setSingleConnectionMode);
   log_finfo("setting mode for single connection per client IP address");

   singleConnectionPerIP = true;
}

// -----------------------------------------------------------------------------
// INTERNAL HELPER FUNCTIONS
// -----------------------------------------------------------------------------

/** Clears the queue of pending output messages for the Connection. These are
    messages waiting to be sent to the client through the Connection. They are
    now discarded.

    @param
    c : the Connection for which to release the pending output messages

    @private @memberof ConnectionManager
*/

static void clearPendingOutMsgs(Connection* c)
{
   Message* message;
   log_func(clearPendingOutMsgs);

   uint nPendMsgs = 0;
   int socket = connection_socket(c);
   cchar* ipAddrStr = connection_ipAddrStr(c);
   ushort port = connection_port(c);
   uint seqNo = connection_seqNo(c);
   Queue* pendingList = connection_pendingOutputMessages(c);

   if (queue_isEmpty(pendingList))
   {
      log_finfo("no pending output messages for [IP:%s port:%d "
         "socket:%d seq:%d]", ipAddrStr, port, socket, seqNo);
      return;
   }

   log_finfo("discarding pending output messages for connection %s", ipAddrStr);

   for (;;)
   {
      log_finfo("removing one pending output message from the connection");
      message = (Message*) queue_removeHead(pendingList);

      if (!message)
         return;

      log_finfo("discarding the pending output message");
      queueManager_disposeMessage(message);
      nPendMsgs++;
   }

   log_finfo("%d pending output messages were discarded", nPendMsgs);
}

// -----------------------------------------------------------------------------

/** Closes a Connection.

    The resources allocated to the Connection are released, with some
    exceptions.

    If the Connection will be re-created soon, doesn't dispose of the input
    Message and doesn't destroy the additional messages associated with the
    Connection.

    @param
    c : the Connection being closed

    @param
    normalClose : tells if the Connection is being closed for good, or if it is
    being closed but another is being created right away.

    @private @memberof ConnectionManager
*/

static void closeConnectionEx(Connection* c, uint normalClose)
{
   Message* inputMessage = connection_inputMessage(c);
   Message* outputMessage = connection_outputMessage(c);
   int socket = connection_socket(c);
   log_func(closeConnectionEx);

   cchar* type = normalClose ? "" : "stale";

   log_fwarn("closing %sconnection [IP:%s port:%d socket:%d seq:%d]", type,
      connection_ipAddrStr(c), connection_port(c), socket, connection_seqNo(c));

   // closes the socket
   log_finfo("closing socket %d", socket);
   socket_close(socket);

   // stops watching for read events from the socket
   log_finfo("stop watching read events for the socket %d", socket);
   socket_removeSocketFromReading(socket);

   // stops watching for write events from the socket
   log_finfo("stop watching write events for the socket %d", socket);
   socket_removeSocketFromWriting(socket);

   // if the connection was receiving a message, frees the message it was using,
   // but doesn't do it if the connection is being re-created soon
   if (inputMessage)
   {
      if (normalClose)
      {
         log_finfo("disposing of the input message attached to the connection");
         queueManager_disposeMessage(inputMessage);
      }
      // clears pointer anyway, because the connection is being destroyed
      // (the input message must have been saved by the caller)
      connection_setInputMessage(c, NULL); // clears pointer anyway 
   }

   // if the connection was sending a message, frees the message it was using
   if (outputMessage)
   {
      log_finfo("disposing og the output message attached to the connection");
      queueManager_disposeMessage(outputMessage);
      connection_setOutputMessage(c, NULL);
   }

   // if the connection is not going to be re-created soon, destroys
   // per-connection additional messages
   if (normalClose)
   {
      log_finfo("destroying the messages associated with the connection");
      queueManager_destroyConnectionMessages();
   }

   // clears queue of output messages waiting to be sent
   log_finfo("clearing pending output messages for the connection");
   clearPendingOutMsgs(c);

   // inactivates the connection to be removed
   connection_setActiveState(c, false);

   log_finfo("removing the connection from the connection table");
   connectionTable_removeConnection(c);

   // (the connection is not really destroyed, but maintained as an inactive
   // connection in the connection table)
   // log_finfo("destroying the connection object");
   // connection_destroy(c);

   log_finfo("number of connections now: %d",
      connectionTable_nActiveConnections());
}

// -----------------------------------------------------------------------------

/** Closes an old connection.

    A new connection has arrived, its IP addres is already connected, and the
    single connection mode is in force. So closes the old connection.

    @param [in]
    oldConnection : the old connection.

    @private @memberof ConnectionManager
*/

static void closeOldConnection(Connection* oldConnection)
{
   // we really don't save much here, only remove some clutter from the caller

   cchar* ipAddrStr = connection_ipAddrStr(oldConnection);
   ushort port = connection_port(oldConnection);
   int socket = connection_socket(oldConnection);
   uint seqNo = connection_seqNo(oldConnection);

   log_func(closeOldConnection);

   log_finfo("found [IP:%s port:%d socket:%d seq:%d] in connection table",
      ipAddrStr, port, socket, seqNo);

   log_finfo("closing old connection [IP:%s port:%d socket:%d seq:%d]",
      ipAddrStr, port, socket, seqNo);

   closeConnectionForReuse(oldConnection);
}

// -----------------------------------------------------------------------------

/** Retrieves the Connection associated with an output Message.

    @param
    message : the Message being queried.

    @return
    the Connection associated with the output Message

    @private @memberof ConnectionManager
*/

static Connection* connectionFromOutputMessage(Message* message)
{
   ushort err;
   ushort index;
   char ipAddrStr[20];
   Connection* connection;

   log_func(connectionFromOutputMessage);

   if (!(connection=message_connection(message)))
   {
      err = 1;
      goto L_ERROR;
   }

   index = connection_index(connection);
   log_fdebug("connection index=%d", index);
   if (connectionTable_getActiveConnection(index) !=  connection)
   {
      err = 2;
      goto L_ERROR;
   }

   if (!connection_inActiveState(connection))
   {
      err = 3;
      goto L_ERROR;
   }

   return connection;

L_ERROR:
   socket_ipAddrToString(message_ipAddr(message), ipAddrStr);
   log_ferror("error %d in message connection [IP:%s port:? seq:%d]",
      err, ipAddrStr, message_connSeqNo(message));
   return NULL;
}

// -----------------------------------------------------------------------------

/** Closes a Connection by request from another thread.

    @private @memberof ConnectionManager
*/

static void doCloseConnection(void)
{
   ushort connectionSeqNo;
   Connection* connection;
   CmdCloseConnection* cmdCloseConnection;

   log_func(doCloseConnection);

   mutex_lock(ccqMutex);
   cmdCloseConnection = (CmdCloseConnection*)
      queue_removeHead(closeConnectionQueue);
   mutex_unlock(ccqMutex);

   if (!cmdCloseConnection)
   {
      log_ferror("close connection queue is empty!");
      return;
   }

   connection = cmdCloseConnection->connection;
   connectionSeqNo = cmdCloseConnection->connSeqNo;

   if (connection_seqNo(connection) != connectionSeqNo)
   {
      log_ferror("invalid connection sequence number");
      return;
   }

   log_fwarn("closing connection by request of application");
   closeConnection(connection);
}

// -----------------------------------------------------------------------------

/** Fills the new connection structure.

    @param
    connection : address of the new connection structure.

    @param
    socket : the socket descriptor of the new connection

    @param
    ipAddr : the client IP address of the new connection.

    @param
    port : the client port of the new connection.

    @private @memberof ConnectionManager
*/

static void prepareNewConnection(Connection* connection, int socket,
   uint ipAddr, ushort port)
{
   log_func(prepareNewConnection);

   connection_setActiveState(connection, true);

   // initializes the basic information
   connection_setSocket(connection, socket);
   connection_setPort(connection, port);
   connection_setIPaddress(connection, ipAddr);

   // customization of the new connection (for now just sets tracing)
   if (createCallbackFunc)
      createCallbackFunc(connection);

   // saves having to format the IP address over and over...
   socket_ipAddrToString(ipAddr, (char*)connection_ipAddrStr(connection));

   log_finfo("new connection with [IP:%s port:%d socket:%d seq:%d]",
      connection_ipAddrStr(connection), port, socket,
      connection_seqNo(connection));

   log_finfo("number of active connections now: %d",
      connectionTable_nActiveConnections());
}

// -----------------------------------------------------------------------------

/** Processes the Connection table for Socket events.

    @param [in]
    nEvts : number of outstanding Socket events.

    @private @memberof ConnectionManager
*/

static void processConnectionTable(int nEvts)
{
   ushort i;
   uint socket;
   Connection* c;
   char connInfo[50];
   bool destroyed = false;
   uint nActiveConnections;

   log_func(processConnectionTable);
   log_fdebug("processing the connection table");

   nActiveConnections = connectionTable_nActiveConnections();

   log_fdebug("now checking for read/write events, %d connections",
      nActiveConnections);

   for (i = 0; i < nActiveConnections; ++i)
   {
      destroyed = false;
      log_fdebug("checking connection #%d/#%d for events", i+1,
         nActiveConnections);

      // the connection being processed
      c = connectionTable_getActiveConnection(i);
      assert(c);

      // information about the connection being processed
      socket = connection_socket(c);
      snprintf(connInfo, 50, "[IP:%s port:%d socket:%d seq:%d]",
         connection_ipAddrStr(c), connection_port(c), socket,
         connection_seqNo(c));

      // is there something to receive ?
      if (socket_isReadyForReading(socket))
      {
         log_fdebug("found read event for connection %s", connInfo);
         receivePartialMessage(c);
         socket_setNotReadyForReading(socket); // ok, processed read event
         nEvts--; // adjusts counter of pending events
      }
      else
         log_fdebug("NOT found read event for connection %s", connInfo);

      // checks whether the connection was destroyed while being processed
      if (nActiveConnections != connectionTable_nActiveConnections())
      {
         destroyed = true; // connection just processed was destroyed
         nActiveConnections = connectionTable_nActiveConnections();
         log_fwarn("connection %s destroyed in read evt", connInfo);
      }

      // is there something to send ?
      if (socket_isReadyForWriting(socket))
      {
         if (!destroyed)
         {
            log_fdebug("found write event for %s", connInfo);
            sendPartialMessage(c);
         }
         else
            log_fwarn("found write event for %s but connection was destroyed",
               connInfo);
         socket_setNotReadyForWriting(socket); // ok, processed write event
         nEvts--; // adjusts counter of pending events
      }
      else
         log_fdebug("NOT found write event for %s", connInfo);

      // checks again whether the connection was destroyed while being processed
      if (nActiveConnections != connectionTable_nActiveConnections())
      {
         destroyed = true; // connection just processed was destroyed
         nActiveConnections = connectionTable_nActiveConnections();
         log_fwarn("connection destroyed in write evt %s", connInfo);
      }

      if (destroyed)
      {
         log_fwarn("connection destroyed, %d connections now, %d events "//--
            "pending...", nActiveConnections, nEvts);
         i--; // evil!
      }

      if (!nEvts) // a little optimization, stop if all events processed
      {
         log_fdebug("all events processed, i = %d", i);
         break; // breaks the "for"
      }
   } // for

   if (nEvts) // who knows ??? anything can happen...
   {
      log_ferror("error, still %d events pending...", nEvts);
   }
}

// -----------------------------------------------------------------------------

/** Processes a local IPC event.

    Reads a command from the local IPC socket and processes the command read.
    The command can tell the connection manager to either start sending a
    message to a client (the most common case), or in some rare cases to
    close a specific connection.

    @private @memberof ConnectionManager
*/

static void processLocalIpcEvent(void)
{
   char cmd = readCommand();

   if (cmd == CM_SEND_MESSAGE)
      startSendingMessage();

   else if (cmd == CM_CLOSE_CONNECTION)
      doCloseConnection();
}

// -----------------------------------------------------------------------------

/** Analyzes the first part of a received Message.

    Checks if the prefix flag and the Message size are valid, and prepares the
    Connection for the reception of the second part.

    @param [in]
    c : Connection through which the Message is being received.

    @private @memberof ConnectionManager
*/

static void processMessagePart1(Connection* c)
{
   Message* m;
   log_func(processMessagePart1);

   // finished receiving part 1 of the message
   connection_setReceivingState(c, RECV_STATE1, ST_FALSE);

   // gets message that is being received by this connection
   m = connection_inputMessage(c);
   assert(m);

   // consistency check
   if (!message_isValidPrefix(m))
   {
      log_ferror("invalid message prefix [%c] in data received from " //------
         "[IP:%s port:%d socket:%d seq:%d], now closing connection",
          message_prefix(m), connection_ipAddrStr(c), connection_port(c),
          connection_socket(c), connection_seqNo(c));
      closeConnection(c);
      return;
   }

   // consistency check
   if (message_size(m) > message_maxBodySize())
   {
      log_ferror("invalid message body size [%d] in data received from " //---
         "[IP:%s port:%d socket:%d seq:%d], now closing connection",
          message_size(m), connection_ipAddrStr(c), connection_port(c),
          connection_socket(c), connection_seqNo(c));
      closeConnection(c);
      return;
   }

   // now prepares to receive part 2
   log_fdebug("preparing to receive the second part of the message: %d bytes",
      message_size2(m));
   connection_setReceivingState(c, RECV_STATE2, ST_TRUE);
   connection_opControl(c)->nBytesToReceive = message_size2(m);
}

// -----------------------------------------------------------------------------

/** Analyzes the second part of a received Message.

    Checks if the suffix flag is valid, forwards the Message to the input queue,
    notifies a worker thread via the input queue semaphore, and prepares the
    Connection for the reception of another Message.

    @param [in]
    c : Connection through which the Message was received.

    @private @memberof ConnectionManager
*/

static void processMessagePart2(Connection* c)
{
   Message* m;
   log_func(processMessagePart2);

   m = connection_inputMessage(c);
   assert(m);

   // finished receiving part 2
   connection_setReceivingState(c, RECV_STATE2, ST_FALSE);

   if (!message_isValidSuffix(m))
   {
      log_ferror("invalid message suffix [0x%02X] in data received from " //------
         "[IP:%s port:%d socket:%d seq:%d], now closing connection",
         (uchar)message_suffix(m), connection_ipAddrStr(c), connection_port(c),
         connection_socket(c), connection_seqNo(c));
      closeConnection(c);
      return;
   }

   // forwards message to the input message queue and alerts a worker thread
   log_fdebug("dispatches the received message to the input message queue");
   queueManager_addInputMessage(m);

   // starts receiving another message
   log_fdebug("preparing to start receiving another message from the client");

   // gets a new free message
   log_fdebug("gets a new free message");
   connection_setInputMessage(c, m=queueManager_getFreeMessage());

   // makes the message refer to this connection (IP address and seqNo)
   message_setConnection(m, c, connection_ipAddr(c), connection_seqNo(c));

   log_fdebug("starts waiting another message from the client");
   connection_setReceivingState(c, RECV_STATE1, ST_TRUE);
   connection_opControl(c)->nBytesReceived = 0;
   connection_opControl(c)->nBytesToReceive = message_size1();
}

// -----------------------------------------------------------------------------

/** Accepts a Connection request from a client, and creates a new worker Socket.

    If the Connection comes from an already connected IP address and the single
    IP Connection option is set, then closes the current Connection from that IP
    address before creating a new one.

    The new Connection immediately starts receiving messages.

    @private @memberof ConnectionManager
*/

static void processNewConnection(void)
{
   int workerSocket;  // socket descriptor of newly-connected client
   uint clientIPaddr; // IP address of newly-connected client
   ushort clientPort; // port of newly-connected client

   uint reuse = 0;               // used for just a bit of optimization 
   Message* inputMessage = NULL; // used for just a bit of optimization

   Connection *newConnection, *tmpConnection;

   log_func(processNewConnection);

   // accepts the connection request
   log_finfo("accepting new connection");
   workerSocket = socket_createWorkerSocket(listeningSocket, &clientIPaddr,
      &clientPort);

   // checks if connection was really accepted
   if (workerSocket == -1)
   {
      log_ferror("failed accept of new connection");
      return;
   }

   socket_setNonBlocking(workerSocket, true);

   // a bit of optimization
   if (singleConnectionPerIP)
   {
      if ((tmpConnection = connectionTable_findConnectionByIP(clientIPaddr)))
      {
         // saves inputMessage, it is going to be reused soon
         inputMessage = connection_inputMessage(tmpConnection);
         closeOldConnection(tmpConnection);
         reuse = 1; // flag, so we know what hapenned
      }
   }

   log_finfo("acquiring a free connection");
   if (!(newConnection = connectionTable_getFreeConnection()))
   {
      // if reuse we won't enter here because we closed the old connection
      log_ferror("run out of connections!!!");
      socket_close(workerSocket);
      return;
   }

   // ok, we got ourselves a free connection now
   prepareNewConnection(newConnection, workerSocket, clientIPaddr, clientPort);

   if (reuse)  // reuses Message structure that is already allocated
   {
      log_finfo("reusing old input message for new connection");
      connection_setInputMessage(newConnection, inputMessage);
   }
   else // if not reusing, then must create the associated messages
   {
      log_finfo("creating connection messages for new connection");
      queueManager_createConnectionMessages();
   }

   // starts receiving messages from the new connection.
   log_finfo("starting receiving messages for the new connection");
   startReceivingMessages(newConnection);
}

// -----------------------------------------------------------------------------

/** Reads a command sent by another thread through the local IPC socket.

    @return
    the command received by the connection manager

    @private @memberof ConnectionManager
*/

static char readCommand(void)
{
   char cmd;
   int result;

   log_func(readCommand);

   result = socket_recv(ipcReceiverSocket, &cmd, 1);

   if (result == 1)
      return cmd; // ok

   if (result == -1)
   {
      int err = socket_error();
      log_ferror("error %d when reading reading local IPC socket!", err);
      return -1;
   }

   if (result == 0)
   {
      log_ferror("false alarm when reading local IPC event!");
      return 0;
   }

   return cmd;
}

// -----------------------------------------------------------------------------

/** Receives part of a Message.

    Receives part of a Message, in response to a "read event" generated by the
    Connection Socket.

    @param
    c : the Connection through which the Message is being received.

    @private @memberof ConnectionManager
*/

static void receivePartialMessage(Connection* c)
{
   int length;
   Message* m;
   OpControl* o = connection_opControl(c);

   log_func(receivePartialMessage);

   // checks if the connection is really in the receiving state
   if (!connection_inReceivingState(c, RECV_STATE0))
   {
      log_ferror("unexpected message from [IP:%s port:%d socket:%d seq:%d], "//-
         "closing connection", connection_ipAddrStr(c), connection_port(c),
         connection_socket(c), connection_seqNo(c));
      closeConnection(c);
      return;
   }

   // message being received
   m = connection_inputMessage(c);
   assert(m);

   // tries to receive something
   log_fdebug("trying to receive %d bytes (already received: %d)",
      o->nBytesToReceive, o->nBytesReceived);
   length = socket_recv(
      connection_socket(c),                 // socket
      message_start(m) + o->nBytesReceived, // buffer to receive
      o->nBytesToReceive);                  // length to receive

   // checks if all went well
   log_fdebug("checking the socket operation result");
   if (socketOperationResult(c, length) != ST_OK)
      return;

   // ok, some bytes arrived

   if (connection_isTraceEnabled(c) || log_level() >= LOG_LEVEL_TRACE)
   {
      char buf[100];
      snprintf(buf, 99, "received from [IP:%s port:%d socket:%d seq:%d]"//------
         " orgId:%d orgSeq:%d", connection_ipAddrStr(c),
         connection_port(c), connection_socket(c), connection_seqNo(c),
         message_orgId(m), message_orgSeqNo(m));
      log_trace(buf, message_start(m) + o->nBytesReceived, length);
   }

   log_fdebug("received %d bytes", length);
   o->nBytesToReceive -= length;
   o->nBytesReceived += length;
   log_fdebug("new counters: nBytesReceived=%d nBytesToReceive=%d",
      o->nBytesReceived, o->nBytesToReceive);

   // checks if all bytes of this part of the message were received, if not
   // then go back to receive the remaining bytes
   log_fdebug("checking if the message is complete");
   if (o->nBytesToReceive)
   {
      log_fdebug("still didn't receive all");
      return;
   }

   // a complete part was received

   // has part 1 of message been received ?
   log_fdebug("checking which part of the message was received");
   if (connection_inReceivingState(c, RECV_STATE1))
   {
      log_fdebug("received the first part of a message");
      processMessagePart1(c);
   }

   // has part 2 of message been received ?
   else if (connection_inReceivingState(c, RECV_STATE2))
   {
      log_fdebug("received the second part of a message");
      processMessagePart2(c);
   }

   else
   {
      // well, I guess we should not get here!!!
      log_ferror(
         "invalid connection state [#%04X] in client [IP:%s port:%d socket:%d"//
         " seq:%d], now closing connection", connection_state(c),
         connection_ipAddrStr(c), connection_port(c), connection_socket(c),
         connection_seqNo(c));
      closeConnection(c);
   }
}

// -----------------------------------------------------------------------------

/** Sends a command to the connection manager through the local IPC socket.

    @param
    cmd : buffer containing q commando to the connection manager

    @private @memberof ConnectionManager
*/

static void sendCommand(cchar cmd)
{
   int result;

   log_func(sendCommand);

   result = socket_send(ipcSenderSocket, &cmd, 1);

   if (result == -1)
   {
      int err = socket_error();
      log_ferror("ipSender: error %d, don't know what to do!", err);
   }
   else if (result != 1)
      log_ferror("ipcSender: couldn't send command, don't know what to do!");
}

// -----------------------------------------------------------------------------

/** Sends part of a Message.

    Tries to send some more of the Message currently being sent, in response to
    a "write event" generated by the Connection Socket.

    @param [in]
    c : Connection through which the Message is being sent.

    @private @memberof ConnectionManager
*/

static void sendPartialMessage(Connection* c)
{
   int length;
   Message* m;
   log_func(sendPartialMessage);

   OpControl* o = connection_opControl(c);

   // checks if the connection is really in the sending state
   log_fdebug("checking if the connection is really sending something");
   if (!connection_inSendingState(c))
   {
      log_ferror("connection to [IP:%s port:%d socket:%d seq:%d] not sending,"//
         " closing connection", connection_ipAddrStr(c), connection_port(c),
         connection_socket(c), connection_seqNo(c));
      closeConnection(c);
   }

   // message being sent
   m = connection_outputMessage(c);
   assert(m);

LOOP:

   // tries to send something
   log_fdebug("trying to send something through the socket");
   length = socket_send(connection_socket(c), // socket
      message_start(m) + o->nBytesSent,       // buffer to send
      o->nBytesToSend);                       // length to send

   // checks if all went well
   log_fdebug("checking the socket operation result");
   if (socketOperationResult(c, length) != ST_OK)
      return;

   // ok, some bytes were sent

   if (connection_isTraceEnabled(c) || log_level() >= LOG_LEVEL_TRACE)
   {
      char buf[100];
      snprintf(buf, 99,  "sent to [IP:%s port:%d socket:%d seq:%d] orgId:%d"//--
         " orgSeq:%d", connection_ipAddrStr(c), connection_port(c),
         connection_socket(c), connection_seqNo(c), message_orgId(m),
         message_orgSeqNo(m));
      log_trace(buf, message_start(m) + o->nBytesSent, length);
   }

   log_fdebug("sent %d bytes", length);
   o->nBytesToSend -= length;
   o->nBytesSent += length;
   log_fdebug("new counters: nBytesSent=%d nBytesToSend=%d", o->nBytesSent,
      o->nBytesToSend);

   // checks if the message was compeletely sent, if no then goes back to send
   // the remaining bytes
   if (o->nBytesToSend)
   {
      log_fdebug("still didn't send the whole message");
      return;
   }

   // ok, the complete message was sent
   log_fdebug("ok, a complete message was sent through the connection");

   // dispose of the message
   log_fdebug("disposing of the output message");
   queueManager_disposeMessage(m);
   connection_setOutputMessage(c, NULL);

   // see if there's more messages queued up waiting to be sent
   log_fdebug("are there any more messages to send through this connection?");

   if (!(m = (Message*)queue_removeHead(connection_pendingOutputMessages(c))))
   {
      // nothing more to be sent
      log_fdebug("for now, no more messages to send");
      log_fdebug("stops monitoring the socket for write events");
      connection_setSendingState(c, ST_FALSE);
      socket_removeSocketFromWriting(connection_socket(c));
      return;
   }

   // ok, there's another message to send

   // prepares message to be sent
   message_setOrgId(m, thread_pid());
   message_setOrgSeqNo(m, orgSeqNo++);

   log_fdebug("there's already a pending output message, starts sending it");
   connection_setOutputMessage(c, m);
   o->nBytesToSend = message_size1() + message_size2(m);
   o->nBytesSent = 0;
   goto LOOP; // do it again

   // that's it :)
}

// -----------------------------------------------------------------------------

/** Analyzes the result of a Socket operation (send/receive).

    @param
    c : Connection through which the operation was performed

    @param
    length : number of bytes transferred by the operation

    @return
    a flag that tells if the operation was successful or not (ST_OK, ST_ERROR)

    @private @memberof ConnectionManager
*/

static uint socketOperationResult(Connection* c, int length)
{
   int err;
   log_func(socketOperationResult);

   if (length > 0)
   {
      log_fdebug("socket operation transferred %d bytes, result ok", length);
      return ST_OK;
   }

   if (length == 0)
   {
      log_fwarn("connection [IP:%s port:%d socket:%d seq:%d] closed by the"//---
         " client", connection_ipAddrStr(c), connection_port(c),
         connection_socket(c), connection_seqNo(c));
      closeConnection(c);
      return ST_ERROR;
   }

   // length < 0
   log_ferror("error in socket operation");

   err = socket_error();

   if (socket_shouldRetry(err))
   {
      log_fwarn("retrying operation on client [IP:%s port:%d socket:%d"//-------
         " seq:%d]!", connection_ipAddrStr(c), connection_port(c),
         connection_socket(c), connection_seqNo(c));
      return ST_ERROR;
   }

   if (socket_wouldBlock(err))
   {
      log_fwarn(" operation on client [IP:%s port:%d socket:%d seq:%d] would"//-
         " block!", connection_ipAddrStr(c), connection_port(c),
         connection_socket(c), connection_seqNo(c));
      return ST_ERROR;
   }

   // unrecoverable error
   log_ferror("error %d in operation with client [IP:%s port:%d socket:%d"//----
      " seq:%d], closing connection!", err, connection_ipAddrStr(c),
      connection_port(c), connection_socket(c), connection_seqNo(c));

   closeConnection(c);
   return ST_ERROR;
}

// -----------------------------------------------------------------------------

/** Starts receiving @link Message Messages @endlink from a newly open
    Connection.

    @param
    c : the Connection through which the reception is being performed.

    @private @memberof ConnectionManager
*/

static void startReceivingMessages(Connection* c)
{
   Message* m;
   log_func(startReceivingMessages);

   log_finfo("new connection starting receiving messages");

   log_finfo("checking to see if it needs a new free input message");

   // gets input message if not currently owning one
   if (!(m = connection_inputMessage(c)))
   {
      log_finfo("requesting a new free input message");
      m = queueManager_getFreeMessage();
      connection_setInputMessage(c, m);
   }

   // makes the message refer to this connection
   message_setConnection(m, c, connection_ipAddr(c), connection_seqNo(c));
   message_setSeqNo(m, connection_lastMsgSeqNo(c));

   // marks new connection as "receiving PART1"
   connection_setReceivingState(c, RECV_STATE0, ST_TRUE);
   connection_setReceivingState(c, RECV_STATE1, ST_TRUE);
   connection_opControl(c)->nBytesReceived  = 0;
   connection_opControl(c)->nBytesToReceive = message_size1();
   log_fdebug("expecting to receive %d bytes", message_size1());

   // starts watching the connection
   log_finfo("starting monitoring the socket for read events");
   socket_addSocketForReading(connection_socket(c));
}

// -----------------------------------------------------------------------------

/** Starts sending a Message, in response to a local IPC event.

    @private @memberof ConnectionManager
*/

static void startSendingMessage(void)
{
   Message* m;
   Connection* c;
   char ipAddrStr[20];

   log_func(startSendingMessage);

   log_fdebug("starting sending a message");

// // flushes local IPC connection
// log_fdebug("flushing the local IPC connection");
// { char b; assert(socket_recv(ipcReceiverSocket, &b, 1) == 1); }

   // message that is to be sent
   log_fdebug("retrieving the output message to be sent");
   if (!(m = messageQueue_get(outputMessageQueue)))
   {
      log_fwarn("strange, output message queue unexpectedly empty!");
      return;
   }

   // formats ip addr for reporting
   socket_ipAddrToString(message_ipAddr(m), ipAddrStr);

   // searches for connection that is to send the message
   log_fdebug("searching connection to be used to send the message");
   if (!(c = connectionFromOutputMessage(m)))
   {
      log_ferror("discarding output message, connection [IP:%s port:? seq:%d]",
         ipAddrStr, message_connSeqNo(m));
      queueManager_disposeMessage(m);
      return;
   }

   // connection already busy sending another message ?
   log_fdebug("checking to see if the connection is already busy for send");
   if (connection_inSendingState(c))
   {
      log_fwarn("output message to client [IP:%s port:%d socket:%d seq:%d],"//--
         " busy", ipAddrStr, connection_port(c), connection_socket(c),
         message_connSeqNo(m));
      // queue up the message, to be sent later
      log_finfo("queuing output message to be sent later");
      queue_addTail(connection_pendingOutputMessages(c), m);
      return;
   }

   // sanity check
   if (connection_outputMessage(c)) // possible leak 
      log_ferror("connection to client [IP:%s port:%d socket:%d seq:%d]"
         " already has outMsg!", ipAddrStr, connection_port(c),
         connection_socket(c), message_connSeqNo(m));

   // prepares message to be sent
   message_setOrgId(m, thread_pid());
   message_setOrgSeqNo(m, orgSeqNo++);

   // prepares connection to send the message
   connection_setOutputMessage(c, m);
   connection_setSendingState(c, ST_TRUE);
   connection_opControl(c)->nBytesSent = 0;
   connection_opControl(c)->nBytesToSend = message_size1() + message_size2(m);
   socket_addSocketForWriting(connection_socket(c));

   // tries sending something now...
   log_fdebug("starting to send the message");
   sendPartialMessage(c);
}

// -----------------------------------------------------------------------------
// the end
