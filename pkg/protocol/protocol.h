// This file includes the custom protocol used in the experiment
#ifndef PROTOCOL
#define PROTOCOL
// Note that the protocol transmits in big endian
// Below is the definition of our protocol:
// Protocol format
// HEX  || BE EF |             |       |
// byte || -- -- | -- -- -- -- | -- -- | --...
// desc || Magic | Body Length | Type  | Body

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../shared/consts.h"


typedef enum {
    REQ_SEND_MSG    = 0x0001,
    REQ_DISCONNECT  = 0x0002,
    RPL_SEND_MSG    = 0x0100,
    RPL_SEND_SENDER = 0x0200,
    UNKNOWN_TYPE    = 0xFFFF
} MessageType;

#define PROTOCOL_HEADER_LEN 8

int IsValidProtocol(const unsigned char *buf);
int extract_message_body_size(const unsigned char *buf);
MessageType get_msg_type(const unsigned char *buf);
int request_disconnect_msg(unsigned char* dest);
// ---- Request format ----
// [Header] [uint32_t String length] [char string message]
// Write the client communication request message
int request_comm_msg(unsigned char *dest, int to_desc, const char *content);
int ExtractMessageData(unsigned char *dest, const unsigned char *src);
int ExtractMessageLength(const unsigned char *src);
#ifdef PROTOCOL_TEST
    void print_array_in_hex(unsigned char *array);
#endif

#endif