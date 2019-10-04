/// @source      Timeout.c
/// @description Implementation of the Timeout facility.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations
#include "../config.h"

// C language includes
#include <time.h>

// framework includes
#include "Timeout.h" /* timeout_xxx functions */

// -----------------------------------------------------------------------------

// global declarations

/// Identification of this source file in the Log file. 
/// @private @memberof Timeout
static cchar sourceID[] = "ZTO";

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Initializes a Timeout structure.

    @param
    timeout : the Timeout structure to be initialized.

    @param [in]
    nSeconds : number of seconds until timeout occurs. If 0 timeout always
    occurs, if (unsigned)-1 timeout never occurs.

    @memberof Timeout
*/

TS2API void timeout_init(Timeout* timeout, uint nSeconds)
{
   timeout->start = time(NULL);
   timeout->nSeconds = nSeconds;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Queries remaining time (in seconds) in a Timeout structure.

    @param [in]
    timeout : the Timeout structure to be queried

    @return
    number of seconds until timeout occurs

    @memberof Timeout
*/

TS2API uint timeout_remaining(const Timeout* timeout)
{
   time_t now;
   uint elapsed;

   if (!timeout->nSeconds)
      return 0;

   if (timeout->nSeconds == TIMEOUT_WAIT_FOREVER)
      return TIMEOUT_WAIT_FOREVER;

   now = time(NULL);
   elapsed = (uint)(now - timeout->start);

   if (elapsed >= timeout->nSeconds) // expired ?
      return 0;

   return timeout->nSeconds - elapsed;
}

// -----------------------------------------------------------------------------
// the end
