/// @source      MessageQueue.h
/// @description Public interface of class MessageQueue.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

#include "Message.h"    /* message_xxx functions  */

// -----------------------------------------------------------------------------

/// MessageQueue opaque type
typedef struct MessageQueue MessageQueue;

// -----------------------------------------------------------------------------

// the redundancy here is for doxygen documentation

// constructor
// ~~~~~~~~~~~

/// see MessageQueue::messageQueue_create
extern TS2API MessageQueue* messageQueue_create(uint, ushort);

// mutators
// ~~~~~~~~

/// see MessageQueue::messageQueue_add
extern TS2API void messageQueue_add(MessageQueue*, const Message*);

/// see MessageQueue::messageQueue_destroyMessages
extern TS2API uint messageQueue_destroyMessages(MessageQueue*, uint, ushort);

/// see MessageQueue::messageQueue_get
extern TS2API Message* messageQueue_get(MessageQueue*);

// query

/// see MessageQueue::messageQueue_isEmpty
extern TS2API uint messageQueue_isEmpty(const MessageQueue*);

/// see MessageQueue::messageQueue_count
extern TS2API uint messageQueue_count(const MessageQueue*);

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __MESSAGE_QUEUE_H__
// -----------------------------------------------------------------------------
