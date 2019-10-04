/// @source      List.c
/// @description Implementation of class List.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations (always include first)
#include "../config.h"

// C language includes
#include <assert.h>
#include <stdlib.h>  // calloc

// framework includes
#include "List.h"    /* list_xxx functions */

// -----------------------------------------------------------------------------

// global declarations

/// List node data structure.
/// Node type for the List data structure.
 struct ListNode
{
   void* entry;           ///< @private The payload data of the node
   struct ListNode* prev; ///< @private Pointer to previous node in the List
   struct ListNode* next; ///< @private Pointer to next node in the List
};

/// List data structure.
/// Implementation of the classic List data structure.
struct List
{
   ListNode* head;         ///< @private Pointer to first node of the List
   uint nEntries;          ///< @private Number of nodes of the List
   ListNode* current;      ///< @private Cache used by certain List operations
   ListNode* currentIter;  ///< @private Cache used by certain List operations
};

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Adds an entry to a list.

    @param
    list : address of list

    @param [in]
    entry : address of entry to be added

    @return
    address of new node

    @remarks
    sets current position

    @memberof List
*/

TS2API void* list_addTail(List* list, void* entry)
{
   ListNode *lastNode = NULL, *newNode, *node;

   assert(list);

   newNode = (ListNode*)calloc(1, sizeof(ListNode));
   assert(newNode);

   // points the new node to the new entry
   newNode->entry = entry;

   // finds the last node in the list
   for (node = list->head; node; node = node->next)
      lastNode = node;

   if (!lastNode) // list is empty ?
      list->head = newNode;
   else
   { // list is not empty, adjust pointers accordingly
      lastNode->next = newNode;
      newNode->prev = lastNode;
   }

   list->nEntries++;
   list->current = newNode;

   return newNode;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Creates a list.

    @return
    address of new list

    @memberof List
*/

TS2API List* list_create(void)
{
   List* list = calloc(1, sizeof(List));
   assert(list);

   return list;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Queries the current position of a list.

    @param [in]
    list : list to be queried

    @return
    current position of list

    @memberof List
*/

TS2API void* list_current(List* list)
{
   assert(list);
   return list->current;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Destroys a list.

    @param
    list : the list to be destroyed

    @pre
    The list <b> must </b> be empty

    @memberof List
*/

TS2API void list_destroy(List* list)
{
   assert(list);
   assert(!list->nEntries); // list must be empty!
   free(list);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Finds entry in a list.

    @param
    list : address of list to be searched

    @param [in]
    checkEntry : function to check each entry according a search criterion

    @param [in]
    arg : the argument used by the compare function

    @return
    address of first entry meeting the search criterion, or NULL

    @remarks
    sets current position

    @memberof List
*/

TS2API void* list_find(List* list, LIST_FIND_FUNC* checkEntry, void* arg)
{
   ListNode *node;

   assert(list);
   assert(checkEntry);

   for (node = list->head; node; node = node->next)
       if (checkEntry(node->entry, arg))
       {
          list->current = node; // caches found node
          return node->entry;   // return node contents
       }

   return NULL; // no entry met search criterion
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves (but not removes) the first entry in an iteration.

    @param
    list : address of list to be iterated

    @return
    address of first entry

    @remarks
    sets current position and current iteration position

    @memberof List
*/

TS2API void* list_firstEntry(List* list)
{
   void* entry;

   // checks if list is valid
   assert(list);

   // checks if list is not empty
   if (!list->head)
      return NULL;

   // gets head entry
   entry = list->head->entry;

   // marks current position
   list->current = list->head;
   list->currentIter = list->head;

   // returns head entry
   return entry;
}

// -----------------------------------------------------------------------------

/** Retrieves (but not removes) the next entry in a iteration.

    @return
    address of next connection

    @pre
    can only be called after a previous successful list_firstEntry
    or list_nextEntry

    @remarks
    uses current iteration position, sets current position and current iteration
    position

    @memberof List
*/

TS2API void* list_nextEntry(List* list)
{
   void* entry;

   assert(list);
   if (!list->currentIter)
      return NULL;

   // advances current position
   list->current = list->currentIter = list->currentIter->next;

   // checks for end of list
   if (!list->currentIter)
      return NULL;

   // gets new current entry
   entry = list->currentIter->entry;

   // returns new current entry
   return entry;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Checks if list is empty.

    @param
    list : address of list

    @return
    true (list is empty) / false (list is not empty)

    @memberof List
*/

TS2API bool list_isEmpty(List* list)
{
   assert(list);
   return !(list->nEntries);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Removes an entry from the list according to a search criterion.

    @param
    list : address of list from which to remove the entry

    @param [in]
    checkEntry : function to check each entry according a search criterion

    @param [in]
    arg : the argument used by the compare function

    @return
    address of removed entry, or NULL if no entry was removed

    @remarks
    sets <i>current</i> to next position if non-null, otherwise to previous,
    whatever its value

    @memberof List
*/

TS2API void* list_remove(List* list, LIST_FIND_FUNC* checkEntry, void* arg)
{
   ListNode *node;

   assert(list);
   assert(checkEntry);

   for (node = list->head; node; node = node->next)
       if (checkEntry(node->entry, arg))
          return list_removeNode(list, node);

   return NULL; // no entry met search criterion
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Removes the head from the list.

    @param [in]
    list : address of list

    @return
    address of removed entry, or NULL if list empty

    @remarks
    sets <i>current</i> to position whatever its value

    @memberof List
*/

TS2API void* list_removeHead(List* list)
{
   assert(list);
   return  list_removeNode(list, list->head);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Removes a specified node from the list.

    @param
    list : address of list

    @param [in]
    node : node to be removed

    @return
    address of removed entry, or NULL if list empty

    @remarks
    sets <i>current</i> to next position if non-null, otherwise to previous,
    whatever its value

    @memberof List
*/

TS2API void* list_removeNode(List* list, ListNode* node)
{
   void* entry;

   assert(list);

   // list empty ?
   if (!list->head)
      return NULL;

   assert(node);

   // saves the node contents
   entry = node->entry;

   // adjusts prev pointer
   if (node->prev)
      node->prev->next = node->next;
   else
      list->head = node->next;

   // adjusts next pointer
   if (node->next)
   {
      list->current = node->next;
      node->next->prev = node->prev;
   }
   else
      list->current = node->prev;

   free(node);
   list->nEntries--;

   return entry;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Queries the number of entries of a list.

    @return
    size : number of entries of the list

    @memberof List
*/

TS2API uint list_size(List* list)
{
   assert(list);
   return list->nEntries;
}

// -----------------------------------------------------------------------------
// the end
