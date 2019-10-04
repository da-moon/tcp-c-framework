/// @source      Time.h
/// @description Public interface of the Time facility.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __TIME_H__
#define __TIME_H__
// -----------------------------------------------------------------------------

#include <time.h>

// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

// the redundancy here is for doxygen documentation

/// see Time::time_formatCurrentDate
extern TS2API void time_formatCurrentDate(char*);

/// see Time::time_formatCurrentShortDatePacked
extern TS2API void time_formatCurrentShortDatePacked(char*);

/// see Time::time_formatCurrentShortDateReversePacked
extern TS2API void time_formatCurrentShortDateReversePacked(char*);

/// see Time::time_formatCurrentTime
extern TS2API void time_formatCurrentTime(char*);

/// see Time::time_formatCurrentTimePacked
extern TS2API void time_formatCurrentTimePacked(char*);

/// see Time::time_formatDate
extern TS2API void time_formatDate(char*, time_t);

/// see Time::time_formatShortDatePacked
extern TS2API void time_formatShortDatePacked(char*, time_t);

/// see Time::time_formatShortDateReversePacked
extern TS2API void time_formatShortDateReversePacked(char*, time_t);

/// see Time::time_formatTime
extern TS2API void time_formatTime(char*, time_t);

/// see Time::time_formatTimePacked
extern TS2API void time_formatTimePacked(char*, time_t);

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __TIME_H__
// -----------------------------------------------------------------------------
