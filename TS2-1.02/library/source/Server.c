/// @source      Server.c
/// @description Implementation of active class Server.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

//  Application Description

/** @mainpage A Small TCP %Server Framework

    A framework for writing small to medium TCP servers, written in the
    C-language with an object oriented approach, using the Berkeley sockets
    interface, and intended to be cross-platform.  @par

    The framework uses non-blocking sockets to perform all communications
    functions, and all connections are managed by only one thread, therefore
    providing scalability. @par

    When writing a server using this framework, the business logic is provided
    by the applications, in the form of a dynamically configurable number of
    threads that are decoupled from the actual connections. This makes it easy
    to write applications, and allows for easy tuning of the applications
    performance.@par

    Buffer management is also simplified, therefore minimizing a number of
    errors related to memory management. It is possible to configure static
    buffers, buffers associated to a connection, and ad hoc buffers, allocated
    if the other schemes fail. Also, copying is minimized: the only needed copy
    is the one from the operating system buffers to the framework buffers. @par

    The messages exchanged on the wire use a counter and a flag, to provide
    for record boundaries and additional reliability. @par

    A number of convenience functions are also provided, to aid in the writing
    of client applications, although the main goal of the framework is writing
    servers. @par

    It is assumed that the servers written with the framework will generally be
    servers tied to specific business needs, and are meant for in house
    applications running on private networks. These servers are not meant to be
    general purpose servers running on the internet, like web servers and such.

    Below is an informal structure class UML diagram, depicting the overall
    classes that comprise the framework. @image html classes.png

    The heart of the framework is a ConnectionManager object. It owns a
    ConnectionTable instance which owns many Connection objects. Each connection
    in turn owns a Socket instance which does the actual communications work.
    Each connection also owns either one or two Message instances, one for
    receiving messages from and other for sending messages to the connected
    client.@par

    The connection manager, as its name says, manages the TCP connections. For
    each connection it keeps track of the messages being sent and received: how
    many bytes were already transferred, and how many are still outstanding. It
    also manages new connection requests, and handles the errors spawned by all
    socket operations, closing connections in error if needed. @par

    The framework's general flow of operation is illustrated by the sequence
    diagram below. @image html sequence.png

    Every application Thread sits in a loop, waiting for input messages to
    become available for processing. The waiting is done by calling the @link
    QueueManager::queueManager_waitInputMessage waitInputMessage @endlink method
    of the queue manager. When the connection manager finishes assembling an
    input message it signals the queue manager by invoking the @link
    QueueManager::queueManager_addInputMessage addInputMessage @endlink method
    of the queue manager. This method adds the complete message to the input
    message queue and awakens an application thread to process the input
    message. Notice that the message may not be processed immediately: this
    depends on the availability of an idle application thread. @par

    The application thread, after processing an input message, may generate an
    output message intended to be sent to the client application. The
    application thread makes the output message available to the framework by
    calling the @link QueueManager::queueManager_dispatchOutputMessage
    dispatchOutputMessage @endlink method of the queue manager. Upon receiving
    this call the queue manager adds the output message to its output message
    queue, and notifies the connection manager of the existence of the new output
    message by calling the @link 
    ConnectionManager::connectionManager_notifyOutputMessage notifyOutputMessage
    @endlink method  of the connection manager. The connection manager then
    removes the output message from the output message queue, and schedules its
    transmission by the connection associated with the message. @par

    Notice that the application thread may reuse the input message as an output
    message. Or it can dispose of the input message by calling the
    @link QueueManager::queueManager_disposeMessage disposeMessage @endlink
    method of the queue manager (wich returns the message to the free message
    queue) and then request a free message to use as output message, by calling
    the @link QueueManager::queueManager_getFreeMessage getFreeMessage @endlink
    method of the queue manager. It's up to the application thread how it
    manages its messages, as long as it does something with them: either calls
    dispatchOutputMessage or disposeMessage on any message it owns, before
    blocking again after processing an input message. Notice also that a Message
    object always belongs to another object: either to a MessageQueue, or to a
    Thread, or to a Connection.

    There are three other classes in the class diagram that need to be
    explained.  @par

    The Server class is a façade class for convenient access to the most
    important methods for writing server applications. All the methods in the
    Server class are in fact #%defines of methods belonging to another classes.
    @par

    The Client class is also a façade, like the Server class, but for writing
    client applications. Although the framework's aim is to make it easy to
    write server applications, all its infra-stucture can be easily taken
    advantage of, to also write client applications. The Client class has a
    bunch of #%defines to aid its role as a façade class, but also adds some
    functionality of its own, in the form of specific methods for writing client
    applications. @par

    Finally, the Log class writes to a log file a huge amount of information
    about what the framework does when an application that uses the framework
    is run. This log facility has the usual levels of severity: information,
    warning, error, fatal, debugging. It can also be used to trace the
    communications buffers, and can easily be called from inside an application.
    @par

    For more complete documentation about the framework, it's easy to change
    the "doxygen" configuration file to get a lot more information. (Doxygen is
    the utility used to generate the documentation for this project.)
    Specifically, two useful options that can be activated are "CALL_GRAPH" and
    "CALLER_GRAPH". The output generated by these options are highly useful, but
    for large projects it can mess up with the layout of the output shown in the
    browser, because of its huge size.
*/

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations
#include "config.h"

// C language includes
#include <assert.h>
#include <stdlib.h> // calloc

// application includes
#include "Server.h"        /* server_xxx functions */
#include "Socket.h"        /* socket_xxx functions */
#include "util/Log.h"      /* log_xxx functions    */
#include "util/Thread.h"   /* thread_xxx functions */

// -----------------------------------------------------------------------------

// global declarations

/// Server class.
/// A façade class through which the funcionality of the framework can be used
/// when writing a TCP server.
/// @class Server

// Identification for the log file
/// @private @memberof Server
static cchar sourceID[] = "ZSR";

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Initializes the Server class.

    @memberof Server
*/

TS2API void server_init(void)
{
   log_func(server_init);
   log_finfo("Server class inicialization");

   socket_init();
   thread_init();
   queueManager_init();
   connectionManager_init();
}

// -----------------------------------------------------------------------------

/** Runs the framework.

    @memberof Server
*/

TS2API void server_run(void)
{
   log_func(server_run);
   log_finfo("running the server");

   connectionManager_run();
}

// -----------------------------------------------------------------------------
// the end
