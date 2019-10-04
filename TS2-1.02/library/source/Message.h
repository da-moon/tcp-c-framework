/// @source      Message.h
/// @description Public interface of class Message.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __MESSAGE_H__
#define __MESSAGE_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

// Message opaque type.
typedef struct Message Message;

// forward declaration
struct Connection;
#define MsgConn struct Connection

// the redundancy here is for doxygen documentation

// constructor
//  ~~~~~~~~~~~

/// see Message::message_create
extern TS2API Message* message_create(ushort);

// destructor
//  ~~~~~~~~~

/// see Message::message_destroy
extern TS2API void message_destroy(Message*);

// query

/// see Message::message_connection
extern TS2API MsgConn* message_connection(const Message*);

/// see Message::message_connSeqNo
extern TS2API ushort message_connSeqNo(const Message*);

/// see Message::message_flags
extern TS2API ushort message_flags(const Message*);

/// see Message::message_ipAddr
extern TS2API uint   message_ipAddr(const Message*);

/// see Message::message_isValidPrefix
extern TS2API bool   message_isValidPrefix(const Message*);

/// see Message::message_isValidSuffix
extern TS2API bool   message_isValidSuffix(const Message*);

/// see Message::message_maxBodySize
extern TS2API ushort message_maxBodySize(void);

/// see Message::message_prefix
extern TS2API char   message_prefix(const Message*);

/// see Message::message_seqNo
extern TS2API ushort message_seqNo(const Message*);

/// see Message::message_orgId
extern TS2API uint   message_orgId(const Message*);

/// see Message::message_orgSeqNo
extern TS2API ushort message_orgSeqNo(const Message*);

/// see Message::message_size
extern TS2API uint   message_size(const Message*);

/// see Message::message_size1
extern TS2API uint   message_size1(void);

/// see Message::message_size2
extern TS2API uint   message_size2(const Message*);

/// see Message::message_suffix
extern TS2API char   message_suffix(Message*);

// configuration
/// see Message::message_setMaxSize
extern TS2API void message_setMaxSize(uint);

// mutators
// ~~~~~~~~

/// see Message::message_copyConnection
extern TS2API void message_copyConnection(Message* dst, const Message* src);

/// see Message::message_setConnection
extern TS2API void message_setConnection(Message* dst, const MsgConn*,
   uint ipAddr, uint connSeqNo);

/// see Message::message_setConnSeqNo
extern TS2API void message_setConnSeqNo(Message*, uint connSeqNo);

/// see Message::message_setIPaddress
extern TS2API void message_setIPaddress(Message* dst, uint ipAddr);

/// see Message::message_setOrgId
extern TS2API void message_setOrgId(Message* dst, uint orgId);

/// see Message::message_setOrgSeqNo
extern TS2API void message_setOrgSeqNo(Message* dst, ushort orgSeqNo);

/// see Message::message_setSeqNo
extern TS2API void message_setSeqNo(Message* dst, ushort seqNo);

/// see Message::message_setSize
extern TS2API void message_setSize(const Message*, ushort size);

// accessors
// ~~~~~~~~~

/// see Message::message_buffer
extern TS2API char* message_buffer(Message*); // address of application data 

/// see Message::message_start
extern TS2API char* message_start(Message*); // address of on-the-wire data

// buffer operations
// ~~~~~~~~~~~~~~~~~

/// see Message::message_byte
extern TS2API uchar message_byte(const Message*, ushort msgOffset);

/// see Message::message_cloneFields.
/// One field: offset, size.
extern TS2API void message_cloneFields(Message* dst, const Message* src,
   ushort nFields, ... );

/// see Message::message_compareToMessage
extern TS2API int message_compareToMessage(cchar* src1, const Message* src2,
   ushort msgOffset, ushort size);

/// see Message::message_copyFields.
/// One field: dstOffset, srcOffset, size.
extern TS2API void message_copyFields(Message* dst, const Message* src,
   ushort nFields, ... );

/// see Message::message_copyFromMessage
extern TS2API void message_copyFromMessage(char* dst, const Message* src,
   ushort msgOffset, ushort size);

/// see Message::message_copyToMessage
extern TS2API void message_copyToMessage(const Message* dst, cchar* src,
   ushort msgOffset, ushort size);

/// see Message::message_fill
extern TS2API void message_fill(const Message* dst, char fill,
   ushort msgOffset, ushort size);

/// see Message::message_setByte
extern TS2API void message_setByte(const Message* dst, ushort msgOffset,
   uchar byte);

// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __MESSAGE_H__
// -----------------------------------------------------------------------------
