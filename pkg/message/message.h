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
  //   is written at the start of every stream
  uint16_t magic;
  //   protocol character
  uint16_t protocol;
  //   message body size
  int size;
  //   message body
  char *body;
} Message;

// UnmarshallMessage - returns a message struct based on a given
// bytestream
Message UnmarshallMessage(int message_sender, const char *marshalled_message);
// MarshallMessage - takes input and returns an encoded sequence
int MarshallMessage(unsigned char *dest, const uint16_t magic,
                    const uint16_t protocol, const char *content);

// ExtractMessageBodySize - returns size of data that is packed inside
// messagExtractMessageProtocol
int ExtractMessageBodySize(const unsigned char *buf);
// CalculatePayloadSize - calculates total payload size
// after appending headers
int CalculatePayloadSize(const unsigned char *src);
// ExtractMessageMagic - return message's magic value
uint16_t ExtractMessageMagic(const unsigned char *buf);
// returns a hex value representing message time
uint16_t ExtractMessageProtocol(const unsigned char *buf);
// int ExtractMessageBody(unsigned char *dest, const unsigned char *src);
const char *ExtractMessageBody(const unsigned char *src);
#endif