#include "wire.h"

const uint16_t magic = 0xC0DE;
int IsValidProtocol(const unsigned char *buf) {
  if (*(uint16_t *)(buf) == htons(0xC0DE))
    return 1;
  return 0;
}
int SendMessageOverTheWire(int target_socket, const uint16_t protocol,
                           const char *src) {
  unsigned char buffer[MAX_BUFFER];
  int mesg_length = MarshallMessage(buffer, magic, protocol, src);
  return send(target_socket, buffer, mesg_length, 0);
}
