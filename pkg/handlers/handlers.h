#ifndef MESSAGE
#define MESSAGE
#include "../shared/consts.h"
#include <stdio.h>

// for uint type
#include <stdint.h>
// for htons and other methods of the same family
#include <arpa/inet.h>
// For string helper methods
#include "../message/message.h"
#include "wire.h"
#include <stdint.h>

typedef enum {
  // 'A' in hex
  BROADCAST_REQUEST = 0x0041,
  //   'a' in hex
  BROADCAST_REPLY = 0x0061,
  RPL_SEND_SENDER = 0x0200,
  UNKNOWN_TYPE = 0xFFFF
} MessageType;

// reads server reply and multiplexes it to an action to be taken
// based on the reply
void BroadcastProtocolHandleServerReply(int socket);
// reads from stdin and sends a message to server
void BroadcastProtocolSendRequestToServer(int socket);
#endif