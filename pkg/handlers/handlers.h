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
  ECHO_REQUEST = 0x0042,
  //   'a' in hex
  ECHO_REPLY = 0x0062,

  UNKNOWN_TYPE = 0xFFFF
} MessageType;
// RequestHandler - this is the main method
// that reads messages from queue and based on
// their protocol, it would redirect them to the approporiate
// handler
void *ServerRequestHandler(void *arg);

// EchoProtocolSendRequestToServer - reads from stdin and
// sends a message to server . it uses ECHO_REQUEST
// as the protocol value
void EchoProtocolSendRequestToServer(int socket);
void EchoProtocolServerHandler(int socket, Message message);
#endif