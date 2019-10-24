#include "protocol.h"

// Protocol format
// HEX  || BA AD |             |       |
// desc || Magic | Body Length | Type  | Body
// Magic number of the protocol
const uint16_t MAGIC = 0xBADD;

// Message to real content
// The return value is the from/to descriptor
int ExtractMessageData(unsigned char *dest, const unsigned char *src)
{
    int content_length = ntohl(*(uint32_t*)(src + 2)) - 4;
    int to = ntohl(*(uint32_t*)(src + PROTOCOL_HEADER_LEN));
    strncpy((char*)dest, (char*)src + PROTOCOL_HEADER_LEN + 4, content_length);
    dest[content_length] = 0;
    return to;
}
int ExtractMessageLength(const unsigned char *src)
{
    return ntohl(*(uint32_t*)(src+PROTOCOL_HEADER_LEN));
}

int IsValidProtocol(const unsigned char *buf)
{
    if(*(uint16_t*)(buf) == htons(MAGIC)) return 1;
    return 0;
}
// Get length of the body
int get_body_length(const unsigned char *buf)
{
    return ntohl(*(uint32_t*)(buf + 2));
}
// Get message type
MessageType get_msg_type(const unsigned char *buf)
{
    return ntohs(*(uint16_t*)(buf + 6));
}

// Write disconnect request message to dest
int request_disconnect_msg(unsigned char *dest)
{
    // Write magic
    *(uint16_t*)(dest)      = htons(MAGIC);
    // Write body length
    *(uint32_t*)(dest + 2)  = 0;
    // Write type
    *(uint16_t*)(dest + 6)  = htons(REQ_DISCONNECT);
    return PROTOCOL_HEADER_LEN;
}

// Write the client communication request message
int request_comm_msg(unsigned char *dest, int to_desc, const char *content)
{
    // Write magic
    *(uint16_t*)(dest)      = htons(MAGIC);
    // Write body length
    *(uint32_t*)(dest + 2)  = htonl(4 + strlen(content));
    // Write type
    *(uint16_t*)(dest + 6)  = htons(REQ_SEND_MSG);
    // Write to_desc
    *(uint32_t*)(dest + PROTOCOL_HEADER_LEN) = htonl(to_desc);
    // Write message
    strncpy((char*)(dest + PROTOCOL_HEADER_LEN + 4), content, strlen(content));
    return PROTOCOL_HEADER_LEN + 4 + strlen(content);
}
#ifdef PROTOCOL_TEST

// Test use
void print_array_in_hex(unsigned char *array)
{
    int len = ntohl(*(int*)(array + 2));
    for(int i = 0; i < len + PROTOCOL_HEADER_LEN && i < 64; i++)
    {
        printf("%2x ", array[i]);
        if(i%16 == 15) puts("");
    }
    puts("");
}
#endif