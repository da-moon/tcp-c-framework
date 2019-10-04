/// @source      Queue.h
/// @description Public interface for class Queue.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __QUEUE_H__
#define __QUEUE_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

// the redundancy here is for doxygen documentation

// the Queue opaque type
typedef struct Queue Queue;

// auxiliary function for searching
typedef TS2API bool QUEUE_FIND_FUNC(void*, void*);

// constructor
extern TS2API Queue* queue_create(ushort); ///< see Queue::queue_create

// destructor
extern TS2API void queue_destroy(Queue*); ///< see Queue::queue_destroy

// query
extern TS2API bool queue_isEmpty(const Queue*); ///< see Queue::queue_isEmpty
extern TS2API uint queue_size(const Queue*);    ///< see Queue::queue_size

// operations
// ~~~~~~~~~~

/// see Queue::queue_addTail
extern TS2API void  queue_addTail(Queue*, const void*);

/// see Queue::queue_removeHead
extern TS2API void* queue_remove(Queue*, QUEUE_FIND_FUNC*, void*);

/// see Queue::queue_removeHead
extern TS2API void* queue_removeHead(Queue*);

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __QUEUE_H__
// -----------------------------------------------------------------------------
