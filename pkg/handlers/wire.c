#include "wire.h"

int IsValidProtocol(const unsigned char *buf) {
  if (*(uint16_t *)(buf) == htons(0xC0DE))
    return 1;
  return 0;
}