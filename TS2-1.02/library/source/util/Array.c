/// @source      Array.c
/// @description Implementation of Array utility
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations
#include "../config.h"

// C language includes
#include <assert.h>
#include <stdlib.h>  // calloc

// framework includes
#include "Array.h"    /* array_xxx functions */

// -----------------------------------------------------------------------------

// global declarations

/// Array data structure

/// Implementation of the classic array data structure.
struct Array
{
   void** entryArrayPtr; ///< @private Pointer to array of entry pointers 
   uint nEntries;  ///< @private Number of entries
};

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Adds an entry to an array.

    @param
       array : address of array control structure

    @param [in]
       entry : address of entry to be added

    @return
       index of new entry, or -1 if overflow

    @remarks
       assumes the entry is a non-null pointer

    @memberof Array
*/

TS2API int array_add(Array* array, void* entry)
{
   uint i;

   assert(array);

   for (i = 0; i < array->nEntries; i++)
       if (!array->entryArrayPtr[i])   // found empty slot
          break;

   if (i >= array->nEntries)
      return -1; // overflow!

   array->entryArrayPtr[i] = entry;
   return i;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Clears an entry in an array.

    @param
       array : address of array control structure

    @param [in]
       index : index of entry in array

    @return
       address of entry

    @memberof Array
*/

TS2API void array_clear(Array* array, uint index)
{
   assert(array);
   assert(index < array->nEntries);

   array->entryArrayPtr[index] = NULL;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Creates an array.

    @param [in]
       nEntries : number os entries in the array

    @return
       Address of a new array control structure

    @memberof Array
*/

TS2API Array* array_create(uint nEntries)
{
   // creates the array control structure
   Array* array = calloc(1, sizeof(Array));
   assert(array);

   // creates the entries
   array->entryArrayPtr = (void**)calloc(nEntries, sizeof(void*));
   assert(array->entryArrayPtr);

   // initializes the number of entries
   array->nEntries = nEntries;

   return array;
}
// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Finds an entry in array.

    @param
       array : address of array control structure

    @param [in]
       checkEntry : function to check each entry according a search criterion

    @param [in]
    arg : the argument used by the compare function

    @return
       index of first entry meeting search criterion, -1 if no entry qualifies

    @memberof Array
*/

TS2API int array_find(Array* array, ARRAY_FIND_FUNC* checkEntry, void* arg)
{
   uint i;

   assert(array);
   assert(checkEntry);

   for (i = 0; i < array->nEntries; i++)
       if (array->entryArrayPtr[i] && checkEntry(array->entryArrayPtr[i], arg))
          return i;

   return -1; // no entry met search criterion
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves an entry from an array.

    @param
       array : address of array control structure

    @param [in]
       index : index of entry in array

    @return
       address of entry

    @memberof Array
*/

TS2API void* array_get(Array* array, uint index)
{
   assert(array);
   assert(index < array->nEntries);

   return array->entryArrayPtr[index];
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Sets an entry in an array.

    @param
       array : address of array control structure

    @param [in]
       entry : entry to be set in the array

    @param [in]
       index : index of entry in array

    @return
       address of entry

    @memberof Array
*/

TS2API void* array_set(Array* array, const void* entry, uint index)
{
   assert(array);
   assert(index < array->nEntries);

   // entry must NOT be already used!
   assert(!(array->entryArrayPtr[index]));

   array->entryArrayPtr[index] = (void*)entry;

   return array->entryArrayPtr[index];
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Queries the number of entries in an array.

    @param
       array : address of array control structure

    @return
       size : number of entries in the array

    @memberof Array
*/

TS2API uint array_size(Array* array)
{
   assert(array);
   return array->nEntries;
}

// -----------------------------------------------------------------------------
// the end
