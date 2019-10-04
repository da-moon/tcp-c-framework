/// @source      Semaf.c
/// @description Implementation of utility Semaf.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations (always include first)
#include "../config.h"

// C language includes
#include <assert.h>
#include <stdlib.h> // calloc

#if PLATFORM(Windows)
#include <windows.h>
#endif

#if PLATFORM(Linux)
#include <semaphore.h>
#endif

// framework support
#include "Semaf.h"   /* semaf_xxx functions */

// -----------------------------------------------------------------------------

// global declarations

/// Semaf class.
/// This class that encapsulates a platform-specific semaphore facility.
/// @class Semaf

// linux-specific declarations
#if PLATFORM(Linux)
struct semaf_t
{
   /// Native Linux mutex handle.
   sem_t handle;
};
#endif

// Windows-specific declarations
#if PLATFORM(Windows)
struct semaf_t
{
   /// Native Windows semaf handle.
   HANDLE handle;
};
#endif

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Creates a Semaf instance.
    @fn semaf_t* semaf_create(void)
   
    @return
    address of the Semaf just created.

    @memberof Semaf
*/

#if PLATFORM(Linux)
semaf_t* semaf_create(void)
{
   int result;

   // allocates semaf memory
   semaf_t* semaf = (semaf_t*)calloc(1, sizeof(semaf_t));
   assert(semaf);

   // initializes semaf
   result = sem_init(&semaf->handle, 0 /*in-process*/, 0 /*resources*/);
   assert(result == 0);

   return semaf;
}
#endif


#if PLATFORM(Windows)
TS2API semaf_t* semaf_create(void)
{
   semaf_t* semaf = (semaf_t*)calloc(1, sizeof(semaf_t));

   // memory allocation successful ?
   assert(semaf);

   semaf->handle = CreateSemaphore(
      NULL,    // LPSECURITY_ATTRIBUTES
      0,       // initial count
      INT_MAX, // maximum count
      NULL     // name
      );

   assert(semaf->handle);

   return semaf;
}
#endif

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Increments Semaf resource count.
    @fn void semaf_release(semaf_t* semaf)

    @param [in]
    semaf : address of semaphore.

    @memberof Semaf
*/

#if PLATFORM(Linux)
void semaf_release(const semaf_t* semaf)
{
   int result = sem_post(&(((semaf_t*)semaf)->handle));
   assert(result == 0);
}
#endif

#if PLATFORM(Windows)
void semaf_release(const semaf_t* semaf)
{
   DWORD result = ReleaseSemaphore(semaf->handle, 1, NULL);
   assert(result);
}
#endif

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Waits for semaphore to be available.
    @fn void semaf_request(semaf_t* semaf)

    @param [in]
    semaf : address of semaphore.

    @memberof Semaf
*/

#if PLATFORM(Linux)
void semaf_request(const semaf_t* semaf)
{
   int result = sem_wait(&(((semaf_t*)semaf)->handle));
   assert(result == 0);
}
#endif

#if PLATFORM(Windows)
void semaf_request(const semaf_t* semaf)
{
   DWORD result = WaitForSingleObject(semaf->handle, INFINITE);
   assert(result == WAIT_OBJECT_0);
}
#endif

// -----------------------------------------------------------------------------
// the end
