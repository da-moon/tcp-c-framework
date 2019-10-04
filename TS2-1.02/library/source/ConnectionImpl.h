/// @source      ConnectionImpl.h
/// @description Private interface for class Connection.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------
#ifndef __CONNECTION_IMPL_H__
#define __CONNECTION_IMPL_H__
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------

#include "Message.h"
#include "util/Queue.h"

// -----------------------------------------------------------------------------

/// Connection class.
/// A high-level class that encapsulates a Socket connection, providing many
/// information members and control methods for the use of the framework.

struct Connection /* packed, no alignment holes assumed */
{
   int socket;    ///< @private Socket descriptor of Connection
   ushort state;  ///< @private State of Connection, see enum ConnectionState

   // client identification
   ushort clientPort;        ///< @private TCP port of client on remote machine
   uint clientIPaddr;        ///< @private IP Address of remote machine
   char clientIPaddrStr[20]; ///< @private IP Address above in C string format

   ushort connectionSeqNo; ///< @private Chronological sequence number of
                           /// Connection 

   void * upPtr; ///< @private Pointer to entry in List container 
   
   ushort index; ///< @private Index of entry in Array container

   ushort lastReceivedMsgSeqNo; ///< @private Sequence number of last received
                                /// Message by the Connection

   // ----------------
   
   Message* inputMessage; ///< @private Message currently being received through
                          /// the Connection 

   Message* outputMessage; ///< @private Message currently being sent through the
                           /// Connection 

   // ----------------

   // transfer control
   
   ushort nBytesReceived; ///< @private Total number of already received bytes
                          /// for current Message 

   ushort nBytesToReceive; ///< @private Number of bytes of pending reception for
                           /// current Message 

   ushort nBytesSent; ///< @private Number of already sent bytes for currenta
                      /// Message

   ushort nBytesToSend; ///< @private Number of bytes waiting to be sent for
                        /// current Message 

   // ----------------

   Queue* pendingOutputMessages; ///< @private Queue of @link Message Messages
                                 /// @endlink waiting to be sent through the
                                 /// Connection 
};

/// The Connection state
/// @private @memberof Connection
enum ConnectionStateBits
{
   CS_ACTIVE_BIT        = 0x0001 , // connection is active
   CS_RECEIVING_BIT     = 0x0002 , // connection is receiving
   CS_SENDING_BIT       = 0x0004 , // connection is sending
   CS_TRACE_ENABLED_BIT = 0x0008 , // connection is being traced
};

/// The reception state of the Connection.
/// @private @memberof Connection
enum ReceptionStateBits
{
   RS_RECEIVING_PART1_BIT = 0x8000 , // receiving first part of a message
   RS_RECEIVING_PART2_BIT = 0x4000 , // receiving second part of a message
};
 
// -----------------------------------------------------------------------------
#ifdef __cplusplus
} // #ifdef __cplusplus
#endif
// -----------------------------------------------------------------------------
#endif // __CONNECTION_IMPL_H__
// -----------------------------------------------------------------------------
