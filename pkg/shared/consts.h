#ifndef CONSTS
#define CONSTS
#include <stdint.h>
#define MAX_BUFFER 4096
#define PROTOCOL_HEADER_LEN 8
// used when initialize char array size for uuid
#define UUID_LENGTH 37

typedef enum {
  // 'A' in hex
  ECHO_REQUEST = 0x0042,
  // 'a' in hex
  ECHO_REPLY = 0x0062,
  // 'D' in hex
  DOWNLOAD_REQUEST = 0x0044,
  // 'U' in hex
  UPLOAD_REQUEST = 0x0055,
  // 'R' in hex
  READY_REPLY = 0x0052,
  // 'F' in hex
  FILE_REPLY = 0x0046,
  // 'P' in hex
  CHANGE_DIR_REQUEST = 0x0050,
  // 'L' in hex
  LIST_DIR_REQUEST = 0x004C,
  // 'l' in hex
  LIST_DIR_REPLY = 0x006C,
  //   'E' in hex
  ERROR_MESSAGE = 0x0045,
  UNKNOWN_TYPE = 0xFFFF
} MessageType;
#endif
