/// @source      List.h
/// @description Public interface for class List.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __LIST_H__
#define __LIST_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

// the redundancy here is for doxygen documentation

// the List opaque type
typedef struct List List;
typedef struct ListNode ListNode;

// auxiliary function for searching
typedef TS2API bool LIST_FIND_FUNC(void*, void*);

// constructor
/// see List::list_create
extern TS2API List* list_create(void);

// destructor
/// see List::list_destroy
extern TS2API void list_destroy(List*);

// mutators
// ~~~~~~~~

/// see List::list_addTail
extern TS2API void* list_addTail(List*, void*);

/// see List::list_remove
extern TS2API void* list_remove(List*, LIST_FIND_FUNC*, void*);

/// see List::list_removeHead
extern TS2API void* list_removeHead(List*);

/// see List::list_removeNode
extern TS2API void* list_removeNode(List*, ListNode*);

// iterators
// ~~~~~~~~~

/// see List::list_firstEntry
extern TS2API void* list_firstEntry(List*);

/// see List::list_nextEntry
extern TS2API void* list_nextEntry(List*);

// query
// ~~~~~

/// see List::list_current
extern TS2API void*list_current(List*);

/// see List::list_isEmpty
extern TS2API bool list_isEmpty(List*);

/// see List::list_size
extern TS2API uint list_size(List*);


// search
/// see List::list_find
extern TS2API void* list_find(List*, LIST_FIND_FUNC*, void*);

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __LIST_H__
// -----------------------------------------------------------------------------
