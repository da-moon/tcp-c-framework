/// @source      Queue.c
/// @description Implementation of class Queue.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations
#include "../config.h"

// C language includes
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>  // calloc

// framework includes
#include "Queue.h"    /* queue_xxx functions */

// -----------------------------------------------------------------------------

// global declarations

/// Queue node data structure.
/// Node type for the Queue data structure.
typedef struct QueueNode
{
   void* entry;            ///< @private The payload data of the node
   struct QueueNode* next; ///< @private Pointer to next node in the Queue
   uchar isPermanent;      ///< @private never deallocated
} QueueNode;

/// Queue data structure.
/// Implementation of the classic Queue data structure.
struct Queue
{
   QueueNode* head;     ///< @private Pointer to first node of the Queue
   QueueNode* tail;     ///< @private Pointer to first last of the Queue
   ushort nEntries;     ///< @private Number of nodes of the Queue
   ushort nPermNodes;   ///< @private Number of permanent nodes
   QueueNode* permHead; ///< @private Pointer to first free permanent node
   QueueNode* permTail; ///< @private Pointer to last free permanent node
};

// internal helper functions
/// @cond hides_from_doxygen
static void addTail(QueueNode**, QueueNode**, QueueNode*);
static void disposeOfNode(Queue*, QueueNode*);
static QueueNode* getHead(QueueNode**, QueueNode**);
/// @endcond

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Creates a queue.

    @return
    address of new queue

    @memberof Queue
*/

TS2API Queue* queue_create(ushort nPermNodes)
{
   Queue* queue = calloc(1, sizeof(Queue));
   assert(queue);

   queue->nPermNodes = nPermNodes;

   // creates the permanent (pre-allocated) nodes
   while (nPermNodes--)
   {
      QueueNode* node = (QueueNode*)calloc(1, sizeof(QueueNode));
      assert(node);
      node->isPermanent = true;
      addTail(&queue->permHead, &queue->permTail, node);
   }

   return queue;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Adds an entry to a queue.

    @param
    queue : address of queue

    @param [in]
    entry : address of entry to be added

    @memberof Queue
*/

TS2API void queue_addTail(Queue* queue, const void* entry)
{
   QueueNode* newNode;

   assert(queue);

   if (!(newNode=getHead(&queue->permHead, &queue->permTail)))
   {
      newNode = (QueueNode*)calloc(1, sizeof(QueueNode));
      assert(newNode);
   }

   // points the new node to the new entry
   newNode->entry = (void*)entry;

   addTail(&queue->head, &queue->tail, newNode);

   queue->nEntries++;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Destroys a queue.

    @param
    queue : the queue to be destroyed

    @pre
    The queue <b> must </b> be empty

    @memberof Queue
*/

TS2API void queue_destroy(Queue* queue)
{
   assert(queue);
   assert(!queue->nEntries); // queue must be empty!

   while (queue->nPermNodes--)
      free(getHead(&queue->permHead, &queue->permTail));

   free(queue);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Checks if queue is empty.

    @param
    queue : address of queue

    @return
    true (queue is empty) / false (queue is not empty)

    @memberof Queue
*/

TS2API bool queue_isEmpty(const Queue* queue)
{
   assert(queue);
   return !queue->nEntries;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Removes an entry from the queue according to a search criterion.

    @param [in]
    queue : address of queue

    @param [in]
    checkEntry : function to check each entry according a search criterion

    @param [in]
    arg : the argument used by the compare function

    @return
    address of removed entry, or NULL if no entry was removed

    @memberof Queue
*/

TS2API void* queue_remove(Queue* queue, QUEUE_FIND_FUNC* checkEntry, void* arg)
{
   void* entry = NULL;
   QueueNode *cur, *prev = NULL;

   assert(queue);
   cur = queue->head;

   while (cur)
   {
      if (!checkEntry(cur->entry, arg))
      {
         prev = cur;
         cur = cur->next;
         continue;
      }

      // checks, will remove!

      if (prev)
         prev->next = cur->next;
      else // first entry is being  deleted, advances head
         queue->head = cur->next;

      if (!cur->next) // last entry is being deleted, goes back with tail
         queue->tail = prev;

      entry = cur->entry;

      disposeOfNode(queue, cur);

      queue->nEntries--;
      break;
   }

   return entry;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Removes the head from the queue.

    @param [in]
    queue : address of queue

    @return
    address of removed entry, or NULL if queue empty

    @memberof Queue
*/

TS2API void* queue_removeHead(Queue* queue)
{
   void* entry;
   QueueNode* node;

   assert(queue);

   if (!(node = getHead(&queue->head, &queue->tail)))
      return NULL;

   entry = node->entry;

   disposeOfNode(queue, node);

   queue->nEntries--;

   return entry;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Queries the number of entries of a queue.

    @return
    size : number of entries of the queue

    @memberof Queue
*/

TS2API uint queue_size(const Queue* queue)
{
   assert(queue);
   return queue->nEntries;
}

// -----------------------------------------------------------------------------
// INTERNAL HELPER FUNCTIONS
// -----------------------------------------------------------------------------

/** Adds a node to queue.

    @param
    pHead : address of pointer to head of the queue

    @param
    pTail : address of pointer to tail of the queue

    @param
    node : node to be added to the queue

    @private @memberof Queue
*/

static void addTail(QueueNode** pHead, QueueNode** pTail, QueueNode* node)
{
   if (!*pTail)
      *pHead = *pTail = node;
   else
   { // queue is not empty, adjust pointers accordingly
      (*pTail)->next = node;
      *pTail = node;
   }

   // this node is now the last
   node->next = NULL;
}

// -----------------------------------------------------------------------------

/** Disposes of a node.

    If the node is permanent, returns it to the free permanent node queue,
    otherwise deletes it.

    @param
    queue : the queue that owns the node

    @param
    node : the node to be disposed of

    @private @memberof Queue
*/

static void disposeOfNode(Queue* queue, QueueNode* node)
{
   if (node->isPermanent) // returns to free node queue
      addTail(&queue->permHead, &queue->permTail, node);

   else
      free(node);
}

// -----------------------------------------------------------------------------

/** Gets the first node from a queue.

    @param
    pHead : address of pointer to head of the queue

    @param
    pTail : address of pointer to tail of the queue

    @return
    node : first node from the queue

    @private @memberof Queue
*/

static QueueNode* getHead(QueueNode** pHead, QueueNode** pTail)
{
   QueueNode* node = NULL;

   if (*pHead)
   {
      node = *pHead;
      if (!(*pHead=(*pHead)->next))
         *pTail = NULL; // queue is now empty
   }

   return node;
}

// -----------------------------------------------------------------------------
// the end
