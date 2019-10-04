/// @source      Array.h
/// @description Public interface for Array utility.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __ARRAY_H__
#define __ARRAY_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

// the redundancy here is for doxygen documentation

// the Array opaque type
typedef struct Array Array;

// auxiliary function for searching
typedef TS2API uint ARRAY_FIND_FUNC(void*, void*);

/// see Array::array_add
extern TS2API int    array_add(Array*, void*);

/// see Array::array_clear
extern TS2API void   array_clear(Array*, uint);

/// see Array::array_create
extern TS2API Array* array_create(uint);

/// see Array::array_get
extern TS2API void*  array_get(Array*, uint);

/// see Array::array_set
extern TS2API void*  array_set(Array*, const void*, uint);

/// see Array::array_size
extern TS2API uint array_size(Array*);

/// see Array::array_find
extern TS2API int array_find(Array*, ARRAY_FIND_FUNC*, void*);

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __ARRAY_H__
// -----------------------------------------------------------------------------
