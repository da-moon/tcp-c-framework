/// @source      ConnectionManager.h
/// @description Public interface of class ConnectionManager.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __CONNECTION_MANAGER_H__
#define __CONNECTION_MANAGER_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

// the redundancy here is for doxygen documentation

// setup
/// see ConnectionManager::connectionManager_init
extern TS2API void connectionManager_init(void);

// operation
/// see ConnectionManager::connectionManager_run
void TS2API connectionManager_run(void);

// utility
/// see ConnectionManager::connectionManager_notifyOutputMessage
extern TS2API void connectionManager_notifyOutputMessage(void);

// configuration

/// see ConnectionManager::connectionManager_setSingleConnectionMode
extern TS2API void connectionManager_setSingleConnectionMode(void);

/// see ConnectionManager::connectionManager_setServicePort
extern TS2API void connectionManager_setServicePort(uint);

/// see ConnectionManager::connectionManager_setCallback
extern TS2API void connectionManager_setCallback(uint, void*);

/// command sent to the connection manager (internal use)
/// @memberof ConnectionManager
enum ConnectionManagerCommands
{
   CM_SEND_MESSAGE = '0'      ,
   CM_CLOSE_CONNECTION = '0'  ,
};

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __CONNECTION_MANAGER_H__
// -----------------------------------------------------------------------------
