/// @source      Server.h
/// @description Public interface for active class Server.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __SERVER_H__
#define __SERVER_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

// basic operations

/// see Server::server_init
extern TS2API void server_init(void);

/// see Server::server_run
extern TS2API void server_run(void);

// -----------------------------------------------------------------------------
// includes needed by the defines below
#include "Connection.h"        /* connection_xxx functions        */
#include "ConnectionManager.h" /* connectionManager_xxx functions */
#include "Message.h"           /* message_xxx functions           */
#include "QueueManager.h"      /* queueManager_xxx functions      */
#include "util/Log.h"          /* log_xxx functions               */
#include "util/Mutex.h"        /* mutex_xxx functions             */
#include "util/Thread.h"       /* thread_xxx functions            */
#include "util/Time.h"         /* time_xxx functions              */

// -----------------------------------------------------------------------------

// the redundancy here is for doxygen documentation

// -----------------------------------------------------------------------------

// class Server is a "façade" for the framework/library
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// what goes here is more or less discretionary

// version
// ~~~~~~~

/// @brief version of the framework
#define server_getVersion()   TS2_VERSION

// Thread support
// ~~~~~~~~~~~~~~

/// see Thread::thread_addThreads
#define server_addThreads thread_addThreads

// Message support
// ~~~~~~~~~~~~~~~

/// see Message::message_byte
#define server_messageByte             message_byte

/// see Message::message_cloneFields
/// One field: offset, size
#define server_cloneMessageFields message_cloneFields

/// see Message::message_compareToMessage
#define server_compareToMessage message_compareToMessage

/// see Message::message_copyConnection
#define server_copyConnectionFromMessage  message_copyConnection

/// see Message::message_copyFields.
/// One field: dstOffset, srcOffset, size.
#define server_copyMessageFields message_copyFields

/// see Message::message_copyFromMessage
#define server_copyFromMessage message_copyFromMessage

/// see Message::message_copyToMessage
#define server_copyToMessage message_copyToMessage

/// see Message::message_fill
#define server_messageFill message_fill

/// see QueueManager::queueManager_dispatchOutputMessage
#define server_dispatchOutputMessage      queueManager_dispatchOutputMessage 

/// see QueueManager::queueManager_disposeMessage
#define server_disposeMessage             queueManager_disposeMessage

/// see QueueManager::queueManager_getFreeMessage
#define server_getFreeMessage             queueManager_getFreeMessage

/// see Message::message_buffer
#define server_messageBuffer              message_buffer

/// see Message::message_connSeqNo
#define server_messageConnSeqNo           message_connSeqNo

/// see Message::message_ipAddr
#define server_messageIPaddress           message_ipAddr

/// see Message::message_size
#define server_messageSize                message_size

/// see Message::message_orgId
#define server_messageOrgId               message_orgId

/// see Message::message_orgSeqNo
#define server_messageOrgSeqNo            message_orgSeqNo

/// see Message::message_setByte
#define server_messageSetByte             message_setByte

/// see QueueManager::queueManager_setMessageNumber
#define server_setMessageNumber           queueManager_setMessageNumber

/// see Message::message_setSize
#define server_setMessageSize             message_setSize

/// see Message::message_setOrgId
#define server_setMessageOrgId            message_setOrgId

/// see Message::message_setOrgSeqNo
#define server_setMessageOrgSeqNo         message_setOrgSeqNo

/// see Message::message_setMaxSize
#define server_setMaxMessageSize          message_setMaxSize

/// see QueueManager::queueManager_waitInputMessage
#define server_waitInputMessage           queueManager_waitInputMessage


// Connection support
// ~~~~~~~~~~~~~~~~~~

/// see Connection::connectionTable_getConnection
#define server_getConnection connectionTable_getConnection

// /// see QueueManager::queueManager_waitInputMessage
// #define server_getConnectionFromMessage  TODO

// /// see QueueManager::queueManager_waitInputMessage
// #define server_closeConnection TODO

// ConnectionManager support
// ~~~~~~~~~~~~~~~~~~~~~~~~~

/// see ConnectionManager::connectionManager_setSingleConnectionMode
#define server_singleConnectionMode connectionManager_setSingleConnectionMode

/// see ConnectionManager::connectionManager_setServicePort
#define server_setServicePort       connectionManager_setServicePort

/// see ConnectionManager::connectionManager_setCallback
#define server_setCallback          connectionManager_setCallback

// Log support
// ~~~~~~~~~~~

/// see Log::log_error
#define server_logError    log_error

/// see Log::log_fatal
#define server_logFatal    log_fatal

/// see Log::log_info
#define server_logInfo     log_info

/// see Log::log_warn
#define server_logWarn     log_warn

/// see Log::log_debug
#define server_logDebug    log_debug

/// see Log::log_func
#define server_logFunc     log_func

/// see Connection::connection_setTrace
#define server_setTrace    connection_setTrace

/// see Log::log_trace
#define server_logTrace    log_trace

/// see Log::log_setDir
#define server_setLogDir   log_setDir

/// see Log::log_ferror
#define server_fLogError   log_ferror

/// see Log::log_ffatal
#define server_fLogFatal   log_ffatal

/// see Log::log_finfo
#define server_fLogInfo    log_finfo

/// see Log::log_fwarn
#define server_fLogWarn    log_fwarn

/// see Log::log_fdebug
#define server_fLogDebug   log_fdebug

/// see Log::log_setLogLevel
#define server_setLogLevel log_setLevel

// convenience
// ~~~~~~~~~~~

/// see Thread::thread_printf
#define server_printf   thread_printf

/// common sleep function
#define server_sleep(x) IF_LINUX(sleep((x))) IF_WINDOWS(Sleep(1000*(x)))

/// see Time::time_formatCurrentTime
#define server_formatCurrentTime time_formatCurrentTime

// Mutex support
// ~~~~~~~~~~~~~

/// see Mutex::mutex_create
#define server_createMutex  mutex_create

/// see Mutex::mutex_lock
#define server_lockMutex    mutex_lock

/// see Mutex::mutex_unlock
#define server_unlockMutex  mutex_unlock

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __SERVER_H__
// -----------------------------------------------------------------------------
