#ifndef MESSAGE
#define MESSAGE
#include <arpa/inet.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

// For string helper methods
#include "../shared/consts.h"
typedef struct {
  // used to keep track of the socket that sent the message
  int message_sender;
  //   protocol character
  uint16_t protocol;
  //   message body size
  int size;
  //   message body
  char *body;
} Message;
// https://www.ibm.com/support/knowledgecenter/en/SSVSD8_8.4.1/com.ibm.websphere.dtx.dsgnstud.doc/references/r_design_studio_intro_Hex_Decimal_and_Symbol_Values.htm
typedef enum {
  // 'A' in hex
  REQ_SEND_MSG = 0x0041,
  //   'a' in hex
  RPL_SEND_MSG = 0x0061,
  RPL_SEND_SENDER = 0x0200,
  UNKNOWN_TYPE = 0xFFFF
} MessageType;

// returns a new message struct
Message *UnmarshallMessage(int message_sender, const char *marshalled_message);
int MarshallMessage(unsigned char *dest, const uint16_t protocol,
                    const char *content);
int SendMessageOverTheWire(int target_socket, const uint16_t protocol,
                           const char *src);
int IsValidProtocol(const unsigned char *buf);
// ExtractMessageBodySize - returns size of data that is packed inside
// messagExtractMessageProtocol
int ExtractMessageBodySize(const unsigned char *buf);
// CalculatePayloadSize - calculates total payload size
// after appending headers
int CalculatePayloadSize(const unsigned char *src);
// returns a hex value representing message time
uint16_t ExtractMessageProtocol(const unsigned char *buf);
// int ExtractMessageBody(unsigned char *dest, const unsigned char *src);
const char *ExtractMessageBody(const unsigned char *src);
#endif