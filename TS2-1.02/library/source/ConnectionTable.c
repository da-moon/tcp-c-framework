/// @source      ConnectionTable.c
/// @description Implementation of class ConnectionTable.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations
#include "config.h"

// C language includes
#include <assert.h>
#include <stdlib.h> // calloc

// framework includes
#include "Connection.h"      /* connection_xxx functions      */
#include "ConnectionTable.h" /* connectionTable_xxx functions */
#include "util/Array.h"      /* array_xxx functions           */
#include "util/List.h"       /* list_xxx functions            */
#include "util/Log.h"        /* log_xxx functions             */

// -----------------------------------------------------------------------------

// global declarations

/// ConnectionTable class.
/// The table of all active @link Connection connections @endlink, managed and
/// owned by the ConnectionManager.
/// @class ConnectionTable

/// Identification for the log file
/// @private @memberof ConnectionTable
static cchar sourceID[] = "ZCT";

/// The ConnectionTable type
/// @private @memberof ConnectionTable
typedef Array ConnectionTable;

/// The ConnectionTable
/// @private @memberof ConnectionTable
static ConnectionTable* connections;

/// Number of active @link Connection Connections @endlink
/// @private @memberof ConnectionTable
static ushort nActiveConnections;

// helper functions
/// @cond hides_from_doxygen
static bool compareIPaddr(void*, void*);
/// @endcond

/// Maximum number of @link Connection Connections @endlink
/// @private @memberof ConnectionTable
enum { N_MAX_CONNECTIONS = FD_SETSIZE - 2 };

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Creates the only and one ConnectionTable.

    @memberof ConnectionTable
*/

TS2API void connectionTable_create(void)
{
   ushort i;
   Connection* connection;

   log_func(connectionTable_create);
   log_finfo("creating the connection table");

   connections = array_create(N_MAX_CONNECTIONS);

   for (i = 0; i < N_MAX_CONNECTIONS; i++)
   {
      connection = connection_create();
      connection_setIndex(connection, i);
      array_set(connections, connection, i);
//    log_fdebug("pendingOutputMessages: %X",
//       connection_pendingOutputMessages(connection));
   }
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Finds the first Connection with a specified IP address.

    @param [in]
    ipAddr : the IP address being looked for

    @return
    address of Connection, or NULL

    @memberof ConnectionTable
*/

TS2API Connection* connectionTable_findConnectionByIP(uint ipAddr)
{
   log_func(connectionTable_findConnectionByIP);
   log_fdebug("searching for a connection using the IP");

   return (Connection*)array_find(connections, compareIPaddr, &ipAddr);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves a Connection given its index.

    @param [in]
    index : index of the Connection in the ConnectionTable

    @return
    address of the Connection, or NULL if the Connection isn't active anymore

    @memberof ConnectionTable
*/

TS2API Connection* connectionTable_getActiveConnection(ushort index)
{
   Connection* connection;

   log_func(connectionTable_getConnection);
   log_fdebug("retrieving a connection");

   if (index >= nActiveConnections)
      return NULL; // connection not active anymore

   connection = array_get(connections, index);

   if (connection_index(connection) != index) // something is wrong!!!
   {
      log_ferror("invalid connection address");
      return NULL;
   }

   return connection;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Gets the first inactive Connection entry.

    @return
    the address of the first inactive Connection entry

    @remark
    assumes the Connection will be made active soon, so adjusts the counter of
    active @link Connection Connections @endlink accordingly

    @memberof ConnectionTable
*/

TS2API Connection* connectionTable_getFreeConnection(void)
{
   log_func(connectionTable_getFreeConnection);
   log_finfo("getting a free connection");

   // check if connection table full
   if (nActiveConnections >= N_MAX_CONNECTIONS)
      return NULL;

   // good, connection table not full
   return array_get(connections, nActiveConnections++);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Removes a Connection from the ConnectionTable.

    The Connection is not really removed from the table, but put outside of the
    range of active @link Connection Connections @endlink.

    @param [in]
    connection : the Connection to be removed

    @memberof ConnectionTable
*/

TS2API void connectionTable_removeConnection(Connection* connection)
{
   ushort index;
   ushort last = nActiveConnections - 1;

   log_func(connectionTable_removeConnection);
   log_finfo("removing a connection from the connection table");

   assert(connection);
   assert(nActiveConnections);

   // checks if the index is valid
   index = connection_index(connection);
   assert(index < N_MAX_CONNECTIONS);
   assert(array_get(connections, index) == connection);

   // only exchange places if NOT last connection
   if (index != last)
   {
      // retrieves current last connection
      Connection* tmp = array_get(connections, last);

      // this must be saved and restored!
      Queue* savPendingOutputMsgs =
         connection_pendingOutputMessages(connection);

      // moves this connection away
      array_clear(connections, last);
      array_set(connections, connection, last);
      connection_clearAll(connection);
      connection_setIndex(connection, last);
      connection_setPendingOutputMessages(connection, savPendingOutputMsgs);

      // brings last connection here
      array_clear(connections, index);
      array_set(connections, tmp, index);
      connection_setIndex(tmp, index);
   }

   // adjusts active connections counter
   nActiveConnections--;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Returns the number of active @link Connection Connections @endlink.

    @return
    number of active @link Connection Connections @endlink

    @memberof ConnectionTable
*/

TS2API ushort connectionTable_nActiveConnections(void)
{
      return nActiveConnections;
}

// -----------------------------------------------------------------------------
// INTERNAL HELPER FUNCTIONS
// -----------------------------------------------------------------------------

/** Compares the IP address of a Connection to a specific IP address.

    @param [in]
    entry : the Connection entry in ConnectionTable

    @param [in]
    arg : the IP address being looked for

    @return
    true (equal) / false (different)

    @private @memberof ConnectionTable
*/

static bool compareIPaddr(void* entry, void* arg)
{
   Connection* connection = (Connection*)entry;
   uint ipAddr = *(uint*)arg;
   return (connection_ipAddr(connection) == ipAddr);
}

// -----------------------------------------------------------------------------
// the end
