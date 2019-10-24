#ifndef MESSAGE
#define MESSAGE
#include "../shared/consts.h"

// for uint type
#include <stdint.h>
// for htons and other methods of the same family
#include <arpa/inet.h>
// For string helper methods
#include "../message/message.h"
#include <stdint.h>

typedef enum {
  // 'A' in hex
  REQ_SEND_MSG = 0x0041,
  //   'a' in hex
  RPL_SEND_MSG = 0x0061,
  RPL_SEND_SENDER = 0x0200,
  UNKNOWN_TYPE = 0xFFFF
} MessageType;
// checks to see if the message is based on a valid protocol
// in which we have defined handlers
int IsValidProtocol(const unsigned char *buf);
// Encode
// SendMessageOverTheWire - sends the message to a target after marshalling
int SendMessageOverTheWire(int target_socket, const uint16_t protocol,
                           const char *src);
#endif