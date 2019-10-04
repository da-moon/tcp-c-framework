/// @source      ConnectionTable.h
/// @description Public interface for class ConnectionTable.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __CONNECTION_TABLE_H__
#define __CONNECTION_TABLE_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

#include "Connection.h"

// -----------------------------------------------------------------------------

// the redundancy here is for doxygen documentation

/// see Connection::connectionTable_create
extern TS2API void        connectionTable_create(void);

/// see Connection::connectionTable_findConnectionByIP
extern TS2API Connection* connectionTable_findConnectionByIP(uint);

/// see Connection::connectionTable_getActiveConnection
extern TS2API Connection* connectionTable_getActiveConnection(ushort);

/// see Connection::connectionTable_getFreeConnection
extern TS2API Connection* connectionTable_getFreeConnection(void);

/// see Connection::connectionTable_nActiveConnections
extern TS2API ushort      connectionTable_nActiveConnections(void);

/// see Connection::connectionTable_removeConnection
extern TS2API void        connectionTable_removeConnection(Connection*);

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __CONNECTION_TABLE_H__
// -----------------------------------------------------------------------------
