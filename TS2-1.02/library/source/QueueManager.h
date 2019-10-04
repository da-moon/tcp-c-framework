/// @source      QueueManager.h
/// @description Public interface of classs QueueManager.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __QUEUE_MANAGER_H__
#define __QUEUE_MANAGER_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

#include "MessageQueue.h"   /* messageQueue_xxx functions */

// -----------------------------------------------------------------------------

// the redundancy here is for doxygen documentation

// predefined message queues
// ~~~~~~~~~~~~~~~~~~~~~~~~~

/// see MessageQueue::freeMessageQueue
extern TS2API MessageQueue* freeMessageQueue;

/// see MessageQueue::inputMessageQueue
extern TS2API MessageQueue* inputMessageQueue;

/// see MessageQueue::outputMessageQueue
extern TS2API MessageQueue* outputMessageQueue;

// public interface
// ~~~~~~~~~~~~~~~~

/// see QueueManager::queueManager_addInputMessage
extern TS2API void queueManager_addInputMessage(const Message*);

/// see QueueManager::queueManager_createConnectionMessages
extern TS2API void queueManager_createConnectionMessages(void);

/// see QueueManager::queueManager_destroyConnectionMessages
extern TS2API void queueManager_destroyConnectionMessages(void);

/// see QueueManager::queueManager_dispatchOutputMessage
extern TS2API void queueManager_dispatchOutputMessage(const Message*);

/// see QueueManager::queueManager_disposeMessage
extern TS2API void queueManager_disposeMessage(Message*);

/// see QueueManager::queueManager_getFreeMessage
extern TS2API Message* queueManager_getFreeMessage(void);

/// see QueueManager::queueManager_init
extern TS2API void queueManager_init(void);

/// see QueueManager::queueManager_setMessageNumbers
extern TS2API void queueManager_setMessageNumbers(uint, uint, uint);

/// see QueueManager::queueManager_waitInputMessage
extern TS2API Message* queueManager_waitInputMessage(void);

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __QUEUE_MANAGER_H__
// -----------------------------------------------------------------------------
