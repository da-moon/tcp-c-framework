/// @source      Connection.c
/// @description Implementation of class Connection.
//  See licensing information in the file README.TXT.

// -----------------------------------------------------------------------------

// includes

// common configuration options & declarations (always include first)
#include "config.h"

// C language includes
#include <assert.h>
#include <stdlib.h> // calloc
#include <string.h>

// framework includes
#include "Connection.h"     /* connection_xxx functions */
#include "ConnectionImpl.h" /* connection_xxx functions */
#include "util/Queue.h"     /* queue_xxx functions      */

// -----------------------------------------------------------------------------

// global declarations

/// Unique sequence number assigned to each Connection
/// @private @memberof Connection
static ushort currentSeqNo;

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Clears the memory of a Connection instance.

    @param
    connection : the Connection being cleared

    @memberof Connection
*/

TS2API void connection_clearAll(Connection* connection)
{
   assert(connection);
   memset(connection, 0, sizeof(Connection));
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Creates a Connection object.

    @return
    the Connection object just created

    @memberof Connection
*/

TS2API Connection* connection_create(void)
{
   Connection* connection = (Connection*)calloc(1, sizeof(Connection)); 
   assert(connection);

   connection->pendingOutputMessages = queue_create(3); // configurable ???
   connection->connectionSeqNo = currentSeqNo++;

   return connection;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Destroys the Connection object.

    @param
    connection : address of Connection object being destroyed

    @memberof Connection
*/

TS2API void connection_destroy(Connection* connection)
{
   assert(connection);

   // other resources must already have been freed before
   assert(!connection->inputMessage && !connection->outputMessage);

   // queue of outstanding output messages must be empty
   assert(queue_isEmpty(connection->pendingOutputMessages));

   // releases used memory
   queue_destroy(connection->pendingOutputMessages);

   free(connection);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the value of the Connection index in the ConnectionTable.

    @param
    connection : Connection being queried

    @return
    the index of this Connection in the ConnectionTable

    @memberof Connection
*/

TS2API ushort connection_index(const Connection* connection)
{
   assert(connection);
   return connection->index;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the address of the Message being currently received by the
    Connection.

    @param
    connection : Connection being queried

    @return
    address of the Message being currently received by the Connection

    @memberof Connection
*/

TS2API Message* connection_inputMessage(const Connection* connection)
{
   assert(connection);
   return connection->inputMessage;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Queries the active state of the Connection.

    @param
    connection : the Connection that is being queried

    @return
    flag : true (active), false (not active)

    @memberof Connection
*/

TS2API bool connection_inActiveState(const Connection* connection)
{
   assert(connection);
   return (connection->state & CS_ACTIVE_BIT);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Queries the receiving state of the Connection.

    @param
    connection : the Connection that is being queried

    @param [in]
    substate : substate that is being queried

    @return
    flag : true (receiving), false (not receiving)

    @memberof Connection
*/

TS2API bool connection_inReceivingState(const Connection* connection, uint substate)
{
   assert(connection);

   if (substate == RECV_STATE0)
      return (connection->state & CS_RECEIVING_BIT);

   if (substate == RECV_STATE1)
      return (connection->state & RS_RECEIVING_PART1_BIT);

   // if (substate == RECV_STATE2)
   return (connection->state & RS_RECEIVING_PART2_BIT);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Queries the sending state of the Connection.

    @param
    connection : the Connection that is being queried

    @return
    flag : true (sending), false (not sending)

    @memberof Connection
*/

TS2API bool connection_inSendingState(const Connection* connection)
{
   assert(connection);
   return (connection->state & CS_SENDING_BIT);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the client IP address associated with the Connection.

    @param
    connection : Connection being queried

    @return
    client IP address associated with the Connection

    @memberof Connection
*/

TS2API uint connection_ipAddr(const Connection* connection)
{
   assert(connection);
   return connection->clientIPaddr;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the formatted client IP address associated with the Connection.

    @param
    connection : Connection being queried

    @return
    formatted client IP address associated with the Connection

    @memberof Connection
*/

TS2API cchar* connection_ipAddrStr(const Connection* connection)
{
   assert(connection);
   return connection->clientIPaddrStr;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the trace state of the Connection.

    @param
    connection : Connection being queried

    @return
    trace state on: true (enabled) / false (disabled)

    @memberof Connection
*/

TS2API bool connection_isTraceEnabled(const Connection* connection)
{
   assert(connection);
   return (connection->state & CS_TRACE_ENABLED_BIT);
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the address of the structure with the operation progress counters.
    This structure has the number of bytes already received, already sent,
    pending reception and pending being sent.

    @param
    connection : Connection being queried

    @return
    address of the structure with the operation progress counters

    @memberof Connection
*/

TS2API OpControl* connection_opControl(const Connection* connection)
{
   assert(connection);
   return (OpControl*)&connection->nBytesReceived;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the address of the Message being currently sent by the Connection.

    @param
    connection : Connection being queried

    @return
    address of the Message being currently sent by the Connection

    @memberof Connection
*/

TS2API Message* connection_outputMessage(const Connection* connection)
{
   assert(connection);
   return connection->outputMessage;
}
// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the queue of pending output @link Message Messages @endlink for a
    Connection.

    @param
    connection : Connection being queried

    @return
    queue of pending output @link Message Messages @endlink for the Connection

    @memberof Connection
*/

TS2API Queue* connection_pendingOutputMessages(const Connection* connection)
{
   assert(connection);
   return connection->pendingOutputMessages;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the client port associated with the Connection.

    @param
    connection : Connection being queried

    @return
    client port associated with the Connection

    @memberof Connection
*/

TS2API ushort connection_port(const Connection* connection)
{
   assert(connection);
   return connection->clientPort;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the Connection sequence number.

    @param
    connection : Connection being queried

    @return
    The Connection sequence number

    @memberof Connection
*/

TS2API ushort connection_seqNo(const Connection* connection)
{
   assert(connection);
   return connection->connectionSeqNo;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Updates the active state of the Connection.

    @param
    connection : the Connection that is being updated

    @param [in]
    active : true (set active) / false (set inactive)

    @memberof Connection
*/

TS2API void connection_setActiveState(Connection* connection, bool active)
{
   assert(connection);

   if (active)
      connection->state |= CS_ACTIVE_BIT;
   else
      connection->state &= ~CS_ACTIVE_BIT;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Assigns a value to the Connection index.

    @param
    connection : the Connection that is being assigned to

    @param [in]
    index : the index to be assigned to the Connection index

    @memberof Connection
*/

TS2API void connection_setIndex(Connection* connection, ushort index)
{
   assert(connection);
   connection->index = index;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Assigns a value to the input Message of the Connection.

    @param
    connection : the Connection that is being assigned to

    @param [in]
    message : the Message to be assigned to the Connection input Message

    @memberof Connection
*/

TS2API void connection_setInputMessage(Connection* connection, Message* message)
{
   assert(connection);
   connection->inputMessage = message;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Assigns a value to the Connection IP address.

    @param
    connection : the Connection that is being assigned to

    @param [in]
    ipAddr : IP address to be assigned to the Connection IP address

    @memberof Connection
*/

TS2API void connection_setIPaddress(Connection* connection, uint ipAddr)
{
   assert(connection);
   connection->clientIPaddr = ipAddr;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves and increments the sequence number of the last Message received
    by this Connection.

    @param
    connection : the Connection that is being both queried and updated

    @return
    the last Message sequence number, before being incremented

    @memberof Connection
*/

TS2API ushort connection_lastMsgSeqNo(Connection* connection)
{
   assert(connection);
   return connection->lastReceivedMsgSeqNo++;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Assigns a value to the output Message of the Connection.

    @param
    connection : the Connection that is being assigned to

    @param [in]
    message : the Message to be assigned to the Connection output Message

    @memberof Connection
*/

TS2API void connection_setOutputMessage(Connection* connection, Message* message)
{
   assert(connection);
   connection->outputMessage = message;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Assigns a value to the queue of pending output messages of the Connection.

    @param
    c: the Connection that is being assigned to

    @param [in]
    p: the queue of output messages

    @memberof Connection
*/

TS2API void connection_setPendingOutputMessages(Connection* c, Queue* p)
{
   assert(c);
   c->pendingOutputMessages = p;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Assigns a value to the Connection port.

    @param
    connection : the Connection that is being assigned to

    @param [in]
    port : the value of the port to be assigned to the Connection port

    @memberof Connection
*/

TS2API void connection_setPort(Connection* connection, ushort port)
{
   assert(connection);
   connection->clientPort = port;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Adjusts the receiving state of the Connection.

    @param
    connection : the Connection that is being modified

    @param [in]
    substate : substate that is being affected

    @param [in]
    isReceiving : true, false

    @memberof Connection
*/

TS2API void connection_setReceivingState(Connection* connection, uint substate,
   bool isReceiving)
{
   assert(connection);

   if (isReceiving)
   {
      if (substate == RECV_STATE0)
         connection->state |= CS_RECEIVING_BIT;
      else if (substate == RECV_STATE1)
         connection->state |= RS_RECEIVING_PART1_BIT;
      else // if (substate == RECV_STATE2)
         connection->state |= RS_RECEIVING_PART2_BIT;
   }
   else // !isReceiving
   {
      if (substate == RECV_STATE0)
         connection->state &= ~CS_RECEIVING_BIT;
      else if (substate == RECV_STATE1)
         connection->state &= ~RS_RECEIVING_PART1_BIT;
      else // if (substate == RECV_STATE2)
         connection->state &= ~RS_RECEIVING_PART2_BIT;
   }
}
// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Adjusts the sending state of the Connection.

    @param
    connection : the Connection that is being modified

    @param [in]
    isSending : true, false

    @memberof Connection
*/

TS2API void connection_setSendingState(Connection* connection, bool isSending)
{
   assert(connection);

   if (isSending)
      connection->state |= CS_SENDING_BIT;
   else
      connection->state &= ~CS_SENDING_BIT;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Assigns a value to the Connection socket.

    @param
    connection : the Connection that is being assigned to

    @param [in]
    socket : the value of the socket being assigned to the Connection socket

    @memberof Connection
*/

TS2API void connection_setSocket(Connection* connection, int socket)
{
   assert(connection);
   connection->socket = socket;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Enables or disables tracing for the Connection.

    @param
    _connection : the Connection that is being modified

    @param [in]
    enable : true (enables) / false (disables)

    @memberof Connection
*/

TS2API void connection_setTrace(void* _connection, bool enable)
{
   Connection* connection = (Connection*)_connection;

   assert(connection);

   if (enable)
      connection->state |= CS_TRACE_ENABLED_BIT;
   else
      connection->state &= ~CS_TRACE_ENABLED_BIT;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the Connection socket.

    @param
    connection : the Connection being queried

    @return
    the Connection socket

    @memberof Connection
*/

TS2API uint connection_socket(const Connection* connection)
{
   assert(connection);
   return connection->socket;
}

// -----------------------------------------------------------------------------
// PUBLIC INTERFACE
// -----------------------------------------------------------------------------

/** Retrieves the Connection state.

    @param
    connection : the Connection being queried

    @return
    the Connection state

    @memberof Connection
*/

TS2API ushort connection_state(const Connection* connection)
{
   assert(connection);
   return connection->state;
}

// -----------------------------------------------------------------------------
// the end
