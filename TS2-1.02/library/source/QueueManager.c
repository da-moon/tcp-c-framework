/// @source      QueueManager.c
/// @description Implementation of class QueueManager.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations (always include first)
#include "config.h"

// C language includes
#include <assert.h>

// framework includes
#include "ConnectionManager.h" /* connectionManager_xxx functions */
#include "Message.h"           /* messageQueue_xxx functions      */
#include "MessageQueue.h"      /* messageQueue_xxx functions      */
#include "QueueManager.h"      /* queueManager_xxx functions      */
#include "util/Log.h"          /* log_xxx functions               */
#include "util/Semaf.h"        /* semaf_xxx functions             */

// -----------------------------------------------------------------------------

// global declarations

/// QueueManager class.
/// This class owns the three message queues that exists in the framework: the
/// input message queue, the output message queue, and the free message queue.
/// Please see MessageQueue for a detailed description. This class also works
/// as an interface between the ConnectionManager and the application
/// threads that make up the application.
/// @class QueueManager

/// Identification of this source file in the Log file
/// @private @memberof QueueManager
static const char sourceID[] = "ZQM";

/// Total number of existing @link Message Messages @endlink
/// @private @memberof QueueManager
static uint nCurrMsgs;

/// Total number of existing @link Message Messages @endlink with Connection
/// lifetime
/// @private @memberof QueueManager
static uint nCurrConnMsgs;

/// Total number of @link Message Messages @endlink with Connection lifetime
/// pending destruction
/// @private @memberof QueueManager
static uint nZombieConnMsgs;

/// Total number of existing @link Message Messages @endlink with discardable
/// lifetime
/// @private @memberof QueueManager
static uint nCurrDiscMsgs;

// the predefined MessageQueues used by the framework

/// Queue of @link Message Messages @endlink free for use
/// @private @memberof QueueManager
MessageQueue* freeMessageQueue;

/// Queue of @link Message Messages @endlink busy being used for receiving data
/// from a Connection
/// @private @memberof QueueManager
MessageQueue* inputMessageQueue;

/// Queue of @link Message Messages @endlink busy being used for sending data
/// through a Connection
/// @private @memberof QueueManager
MessageQueue* outputMessageQueue;

/// semaphore that awakens a worker thread when a new Message is added to
/// the input MessageQueue
/// @private @memberof QueueManager
static semaf_t* inputQueueSemaf;

// ---------------

// policies for the number of messages

/// Policies for Message allocation
/// @private @memberof QueueManager
enum MessageAllocationPolicy
{
   /// Default number of permanent message. These messages are tipically created
   /// at program startup. They lessen the initial impact when the rates of
   /// prodution of input and output messages are somewhat imbalanced. These
   /// messages are never destroyed.
   MSG_N_PERM_MSG = 10 ,

   /// Default number of per-connection message. These messages are tipically
   /// created at connection startup. /// The reasoning is this: one message for
   /// input, and one message for processing and output. They are destroyed when
   /// a connection is closed.
   MSG_N_CONN_MSG = 2 ,

   /// Default emergency growth factor, based on the total number of messages. 
   /// When there's no available message for use, these message are created on
   /// demand. They are destroyed as soon as they are used. Their frequent use
   /// point to a failure in the understanding of the application's behavior.
   MSG_EM_GROWTH_FACTOR = 10 ,
};

/// Number of permanente @link Message Messages @endlink created at program
/// startup
/// @private @memberof QueueManager
static uint nPermMsgs  = MSG_N_PERM_MSG;

/// Number of Connection @link Message Messages @endlink created at each
/// Connection startup
/// @private @memberof QueueManager
static uint nConnMsgs  = MSG_N_CONN_MSG;

/// Percent of discardable @link Message Messages @endlink to be created when
/// all existing @link Message Messages @endlink are being used and a new Message
/// is needed
/// @private @memberof QueueManager
static uint pctGrowth  = MSG_EM_GROWTH_FACTOR;

// ---------------

/// Flags identifying message lifetime
/// @private @memberof QueueManager
enum MessageLifetime
{
   /// Permanent lifetime. These messages are created at program startup, and
   /// are never destroyed.
   MSG_LIFE_PERM = 0x0001,

   /// Connection lifetime. These messages are created at a connection startup,
   /// and are destroyed at a connection shutdown.
   MSG_LIFE_CONN = 0x0002,

   /// Discardable lifetime. These messages are created when needed, on demand,
   /// and are destroyed as soon as they are used.
   MSG_LIFE_DISC = 0x0004
};

/// @cond hides_from_doxygen

/// Checks if a Message has discardable lifetime
#define messageIsDiscardable(m)   (message_flags(m) & MSG_LIFE_DISC)

/// Checks if a Message has Connection lifetime
#define messageIsForConnection(m) (message_flags(m) & MSG_LIFE_CONN)

/// Checks if a Message has permanent lifetime
#define messageIsPermanent(m)     (message_flags(m) & MSG_LIFE_PERM)

/// @endcond

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Adds a Message to the input MessageQueue and alerts a worker thread.

    @param [in]
    message : the Message that will be added to the input MessageQueue

    @memberof QueueManager
*/

TS2API void queueManager_addInputMessage(const Message* message)
{
   log_func(queueManager_addInputMessage);
   log_fdebug("adding a message to the input message queue");

   // adds a message to the input message queue
   messageQueue_add(inputMessageQueue, message);

   // alerts a worker thread
   log_fdebug("awaking a worker thread");
   semaf_release(inputQueueSemaf);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Creates @link Message Messages @endlink with Connection lifetime.

    @memberof QueueManager
*/

TS2API void queueManager_createConnectionMessages(void)
{
   Message* message;
   uint nMessages = nConnMsgs;

   log_func(queueManager_createConnectionMessages);
   log_finfo("creating %d connection messages", nMessages);

   while (nMessages--)
   {
      log_finfo("creating a connection message");
      message = message_create(MSG_LIFE_CONN);
      log_finfo("adding a connection message (%05X) to the free message queue",
         message);
      messageQueue_add(freeMessageQueue, message);
      nCurrMsgs++;
      nCurrConnMsgs++;
   }

   log_finfo("now %d connection messages, %d total messages",
      nCurrConnMsgs, nCurrMsgs);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Destroys @link Message Messages @endlink with Connection lifetime.

    @memberof QueueManager
*/

TS2API void queueManager_destroyConnectionMessages(void)
{
   uint nRemoved, nNotRemoved = 0;

   log_func(queueManager_destroyConnectionMessages);
   log_finfo("destroying %d connection messages", nConnMsgs);

   nRemoved = messageQueue_destroyMessages(freeMessageQueue, nConnMsgs,
      MSG_LIFE_CONN);

   assert(nRemoved <= nCurrConnMsgs);

   if (nRemoved != nConnMsgs)
   {
      nNotRemoved = nConnMsgs - nRemoved;
      nZombieConnMsgs += nNotRemoved;
      log_fwarn("%d connection messages pending destruction!", nNotRemoved);
   }

   nCurrMsgs -= nRemoved;
   nCurrConnMsgs -= nRemoved;
   log_finfo("remaining %d total messages, %d connection messages", nCurrMsgs,
      nCurrConnMsgs);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Adds a Message to the output MessageQueue and alerts the ConnectionManager.

    @param [in]
    message : the Message that will be added to the output MessageQueue

    @memberof QueueManager
*/

TS2API void queueManager_dispatchOutputMessage(const Message* message)
{
   log_func(queueManager_dispatchOutputMessage);
   log_fdebug("dispatching an output message to the connection manager");

   // adds a message to the input message queue
   log_fdebug("adding the output message to the output message queue");
   messageQueue_add(outputMessageQueue, message);

   // alerts the connection manager
   log_fdebug("notifying the connection manager");
   connectionManager_notifyOutputMessage();
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Destroys a Message if it is discardable, otherwise returns it back to the
    the free MessageQueue.

    @param
    message : the message to be disposed of.

    @memberof QueueManager
*/

TS2API void queueManager_disposeMessage(Message* message)
{
   log_func(queueManager_disposeMessage);
   log_fdebug("disposing of a message");

   assert(message);

   if (messageIsDiscardable(message))
   {
      // destroys a message that have discardable lifetime
      log_finfo("destroying a discardable message");
      message_destroy(message);
      assert(nCurrDiscMsgs);  // just to be sure...
      nCurrDiscMsgs--;
      nCurrMsgs--;
      return;
   }

   if (messageIsForConnection(message) && nZombieConnMsgs)
   {
      // destroys a connection zombie message
      log_finfo("destroying a connection zombie message");
      message_destroy(message);
      assert(nCurrConnMsgs);  // just to be sure...
      if (!--nZombieConnMsgs)
         log_finfo("no more zombie messages");
      nCurrConnMsgs--;
      nCurrMsgs--;
      return;
   }

   // returns the message to the queue of available messages

  log_fdebug("returning the message to the input message queue");
  messageQueue_add(freeMessageQueue, message);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Provides a free Message for the caller.

    @return
    address of free Message available for use

    @memberof QueueManager
*/

TS2API Message* queueManager_getFreeMessage(void)
{
   uint nNewMessages;
   Message* spareMessage; // messages that may be created for future use
   Message* message = messageQueue_get(freeMessageQueue);

   log_func(queueManager_getFreeMessage);
   log_fdebug("procuring a free message");

   // if there's a free message available, return it
   if (message)
      return message;

   // new messages to be created, as a percent number of current messages
   nNewMessages = (pctGrowth*nCurrMsgs) / 100;
   if (!nNewMessages)
      nNewMessages++; // creates at least 1 message
   log_finfo("creating %d emergency discardable messages", nNewMessages);

   // creates the first discardable message, that will be returned
   // to the caller
   log_finfo("creating the first emergency discardable messages");
   message = message_create(MSG_LIFE_DISC);

   // creates additional discardable messages
   for (;;)
   {
      // adjust counters, accounting for the first message created above
      nCurrMsgs++;
      nCurrDiscMsgs++;
      if (!--nNewMessages)
         break;

      log_finfo("creating the spare emergency discardable messages");
      spareMessage = message_create(MSG_LIFE_DISC);
      messageQueue_add(freeMessageQueue, spareMessage);
   }

   return message;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Initializes the QueueManager.

    Initializes configuration parameters, create predefined @link MessageQueue
    MessageQueues @endlink, creates the configured number of permanent @link
    Message Messages @endlink, and creates the thread dispatch semaphore.

    @memberof QueueManager
*/

TS2API void queueManager_init(void)
{
   log_func(queueManager_init);
   log_finfo("QueueManager class initialization");

   // creates _nPermMsg free messages with permanent lifetime
   log_finfo("creating the free message queue with %d permanent messages",
      nPermMsgs);
   freeMessageQueue = messageQueue_create(nPermMsgs, MSG_LIFE_PERM);
   nCurrMsgs = nPermMsgs;

   // creates empty queues, will be populated as the application runs
   log_finfo("creating the empty free message queues for input and output");
   inputMessageQueue = messageQueue_create(0, 0);
   outputMessageQueue = messageQueue_create(0, 0);

   // creates the input message queue dispatch semaphore
   log_finfo("creating the semaphore for dispatch of the input message queue");
   inputQueueSemaf = semaf_create();
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Configures the QueueManager.

    Configures the number of the 3 types of @link Message Messages @endlink :
    permanent, per connection, and dynamic (discardable).

    @param
    _nPermMsgs : the number of permanent @link Message Messages @endlink created
    at the framework startup

    @param 
    _nConnMsgs : the number of @link Message Messages @endlink created each time
    time a new Connection is created, and destroyed each time a new Connection
    is destroyed

    @param 
    _pctGrowth : the percentage of discardable @link Message Messages @endlink
    to be created when all existing @link Message Messages @endlink are being
    used and a new Message is needed

    @memberof QueueManager
*/

TS2API void queueManager_setMessageNumbers(uint _nPermMsgs, uint _nConnMsgs,
   uint _pctGrowth)
{
   log_func(queueManager_setMessageNumbers);

   nPermMsgs = _nPermMsgs; // number of permanent lifetime messages
   nConnMsgs = _nConnMsgs; // number of connection lifetime messages
   pctGrowth = _pctGrowth; // percent of growth for discardable messages

   log_finfo("setting number for message types: perm=%d con=%d discGrowth%=%d",
      nPermMsgs, nConnMsgs, pctGrowth);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Waits for an input Message. Called by a worker thread.

    @return
    the Message removed from the input MessageQueue, ready to be processed

    @memberof QueueManager
*/

TS2API Message* queueManager_waitInputMessage(void)
{
   Message* message;

   log_func(queueManager_waitInputMessage);
   log_fdebug("waiting for an input message");

   semaf_request(inputQueueSemaf);
   message = messageQueue_get(inputMessageQueue);
   assert(message); // just in case
   return message;
}

// -----------------------------------------------------------------------------
// the end
