/// @source      Semaf.h
/// @description Public interface for utility Semaf.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __SEMAF_H__
#define __SEMAF_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

// the semaf_t opaque type
typedef struct semaf_t semaf_t;

// the redundancy here is for doxygen documentation

// constructor
/// see Semaf::semaf_create
extern TS2API semaf_t* semaf_create(void);

// operations
// ~~~~~~~~~~

/// see Semaf::semaf_release
extern TS2API void semaf_release(const semaf_t*);

/// see Semaf::semaf_request
extern TS2API void semaf_request(const semaf_t*);

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __SEMAF_H__
// -----------------------------------------------------------------------------
