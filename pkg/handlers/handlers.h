#ifndef HANDLERS
#define HANDLERS
// #include "../message/message.h"
#include "../multiplexer/multiplexer.h"
// #include "../queue/queue.h"
#include "../shared/consts.h"

#include <stdio.h>

// for uint type
#include <stdint.h>
// for htons and other methods of the same family
#include <arpa/inet.h>
// For string helper methods
#include <string.h>

#include "../message/message.h"
#include "wire.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// https://www.ibm.com/support/knowledgecenter/en/SSVSD8_8.4.1/com.ibm.websphere.dtx.dsgnstud.doc/references/r_design_studio_intro_Hex_Decimal_and_Symbol_Values.htm

typedef enum {
  // 'A' in hex
  BROADCAST_REQUEST = 0x0041,
  //   'a' in hex
  BROADCAST_REPLY = 0x0061,
  // 'B' in hex
  ECHO_REQUEST = 0x0042,
  ECHO_REPLY = 0x0062,
  UNKNOWN_TYPE = 0xFFFF
} MessageType;
// RequestHandler - this is the main method
// that reads messages from queue and based on
// their protocol, it would redirect them to the approporiate
// handler
void *ServerRequestHandler(void *arg);

// BroadcastProtocolHandleServerReply -
// reads server reply and multiplexes it to an action to be taken
// based on the reply
void BroadcastProtocolHandleServerReply(int socket);
// BroadcastProtocolSendRequestToServer -
// reads from stdin and sends a message to server
// it uses BROADCAST_REQUEST as the protocol value
void BroadcastProtocolSendRequestToServer(int socket);
// The "consumer" -- waits for the Queue to
// have messages then takes them out and broadcasts to all clients
// effectively making a chat server.
void BroadcastProtocolServerHandler(char *result, Message *message);
// EchoProtocolHandleServerReply - behaves the same way
// as BroadcastProtocolHandleServerReply ... it
// just prints back the recieved server message
void EchoProtocolHandleServerReply(int socket);
// EchoProtocolSendRequestToServer - reads from stdin and
// sends a message to server . it uses ECHO_REQUEST
// as the protocol value
void EchoProtocolSendRequestToServer(int socket);
void EchoProtocolServerHandler(char *result, Message *message);
#endif