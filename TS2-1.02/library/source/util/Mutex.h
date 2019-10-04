/// @source      Mutex.h
/// @description Public interface for Mutex utility.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __MUTEX_H__
#define __MUTEX_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

// the redundancy here is for doxygen documentation

// the mutex_t opaque type
typedef struct mutex_t mutex_t;

// constructor
/// see Mutex::mutex_create
extern TS2API mutex_t* mutex_create(void);

// operations
// ~~~~~~~~~~

/// see Mutex::mutex_lock
extern TS2API void mutex_lock(const mutex_t*);

/// see Mutex::mutex_unlock
extern TS2API void mutex_unlock(const mutex_t*);

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __MUTEX_H__
// -----------------------------------------------------------------------------
