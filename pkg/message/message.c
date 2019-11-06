#include "message.h"
Message UnmarshallMessage(int message_sender, const char *marshalled_message) {
  const uint32_t message_size = ExtractMessageBodySize(marshalled_message);
  const uint16_t message_magic = ExtractMessageMagic(marshalled_message);
  const uint16_t message_protocol = ExtractMessageProtocol(marshalled_message);
  const char *message_body = ExtractMessageBody(marshalled_message);

  Message p;
  p.body = malloc(sizeof(message_body));
  if (p.body == NULL) {
    perror("Couldn't allocate anymore memory!");
    exit(EXIT_FAILURE);
  }
  p.message_sender = message_sender;
  p.protocol = message_protocol;
  p.magic = message_magic;
  p.size = message_size;
  strcpy(p.body, message_body);
  return p;
  //   printf("EXECUTING BROADCAST REPLY ...\n");
}

int MarshallMessage(unsigned char *dest, const uint16_t magic,
                    const uint16_t protocol, const char *content) {
  char *arr_ptr = &content[0];
  int payload_length = strlen(arr_ptr);
  // Write magic short 2 bytes
  *(uint16_t *)(dest) = htons(magic);
  // Write protocol - short 2 bytes
  *(uint16_t *)(dest + 2) = htons(protocol);
  // Write body length - long 4 bytes
  *(uint32_t *)(dest + 4) = htonl(payload_length);
  // Write message

  strncpy((char *)(dest + PROTOCOL_HEADER_LEN), content, payload_length);

  return PROTOCOL_HEADER_LEN + payload_length;
}
// Message to real content
// The return value is the from/to descriptor
const char *ExtractMessageBody(const unsigned char *src) {
  // int ExtractMessageData(unsigned char *dest, const unsigned char *src) {
  char *dest;
  uint32_t decoded_body_length = ExtractMessageBodySize(src);
  printf("Extract msg body -> %lu\n", decoded_body_length);

  dest = malloc(decoded_body_length + PROTOCOL_HEADER_LEN + 20);
  strncpy((char *)dest, (char *)src + PROTOCOL_HEADER_LEN, 14);
  dest[decoded_body_length] = 0;
  return dest;
}
int CalculatePayloadSize(const unsigned char *src) {
  return ntohl(*(uint32_t *)(src + PROTOCOL_HEADER_LEN));
}

// Get length of the body
int ExtractMessageBodySize(const unsigned char *buf) {
  return ntohl(*(uint32_t *)(buf + 4));
}

// Get message type
uint16_t ExtractMessageProtocol(const unsigned char *buf) {
  return ntohs(*(uint16_t *)(buf + 2));
}
uint16_t ExtractMessageMagic(const unsigned char *buf) {
  return ntohs(*(uint16_t *)(buf));
}
