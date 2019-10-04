/// @source      Connection.h
/// @description Public interface for class Connection.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __CONNECTION_H__
#define __CONNECTION_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

#include "Message.h"
#include "util/Queue.h"

// -----------------------------------------------------------------------------

/// Auxiliary data structure for Connection.
/// This structure maintains counters associated with the Connection it belongs
/// to, for data already tranferred and for data still waiting to be
/// transferred.
/// @memberof Connection
typedef struct OpControl
{
   ushort nBytesReceived;
   ushort nBytesToReceive;
   ushort nBytesSent;
   ushort nBytesToSend;
} OpControl;

/// The reception state of the Connection
/// @memberof Connection
enum ReceptionState
{
   RECV_STATE0 = 1,  // general receiving state
   RECV_STATE1,      // receiving first part, up to the message body size
   RECV_STATE2       // receiving second part, the message body size and flag
};

// -----------------------------------------------------------------------------

// the redundancy here is for doxygen documentation

// ---------------------------------------------------

// Opaque type
typedef struct Connection Connection;

// ---------------------------------------------------

// constructor
/// see Connection::connection_create
extern TS2API Connection* connection_create(void);

// ---------------------------------------------------

// destructor
/// see Connection::connection_destroy
extern TS2API void connection_destroy(Connection*);

// ---------------------------------------------------

// mutators

/// see Connection::connection_clearAll
extern TS2API void connection_clearAll(Connection*);

/// see Connection::connection_setInputMessage
extern TS2API void connection_setInputMessage(Connection*, Message*);

/// see Connection::connection_setIPaddress
extern TS2API void connection_setIPaddress(Connection*, uint);

/// see Connection::connection_setOutputMessage
extern TS2API void connection_setOutputMessage(Connection*, Message*);

extern TS2API void connection_setPendingOutputMessages(Connection*, Queue*);

/// see Connection::connection_setPort
extern TS2API void connection_setPort(Connection*, ushort);

/// see Connection::connection_setSocket
extern TS2API void connection_setSocket(Connection*, int);

/// see Connection::connection_setIndex
extern TS2API void connection_setIndex(Connection*, ushort);

// ---------------------------------------------------

// transfer state

/// see Connection::connection_inActiveState
extern TS2API bool connection_inActiveState(const Connection*);

/// see Connection::connection_inSendingState
extern TS2API bool connection_inSendingState(const Connection*);

/// see Connection::connection_inReceivingState
extern TS2API bool connection_inReceivingState(const Connection*, uint);

/// see Connection::connection_setActiveState
extern TS2API void connection_setActiveState(Connection*, bool);

/// see Connection::connection_setSendingState
extern TS2API void connection_setSendingState(Connection*, bool);

/// see Connection::connection_setReceivingState
extern TS2API void connection_setReceivingState(Connection*, uint, bool);

// ---------------------------------------------------

// query

/// see Connection::connection_index
extern TS2API ushort   connection_index(const Connection*);

/// see Connection::connection_inputMessage
extern TS2API Message* connection_inputMessage(const Connection*);

/// see Connection::connection_ipAddr
extern TS2API uint     connection_ipAddr(const Connection*);

/// see Connection::connection_ipAddrStr
extern TS2API cchar*    connection_ipAddrStr(const Connection*);

/// see Connection::connection_lastMsgSeqNo
extern TS2API ushort   connection_lastMsgSeqNo(Connection*);

/// see Connection::connection_outputMessage
extern TS2API Message* connection_outputMessage(const Connection*);

/// see Connection::connection_pendingOutputMessages
extern TS2API Queue*    connection_pendingOutputMessages(const Connection*);

/// see Connection::connection_port
extern TS2API ushort   connection_port(const Connection*);

/// see Connection::connection_seqNo
extern TS2API ushort   connection_seqNo(const Connection*);

/// see Connection::connection_socket
extern TS2API uint     connection_socket(const Connection*);

/// see Connection::connection_state
extern TS2API ushort   connection_state(const Connection*);

// ---------------------------------------------------

// trace

/// see Connection::connection_isTraceEnabled
extern TS2API bool connection_isTraceEnabled(const Connection*);

/// see Connection::connection_setTrace
extern TS2API void connection_setTrace(void*, bool);

// ---------------------------------------------------

// lengths
/// see Connection::connection_opControl
extern TS2API OpControl* connection_opControl(const Connection*);

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __CONNECTION_H__
// -----------------------------------------------------------------------------
