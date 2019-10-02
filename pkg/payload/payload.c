#include "payload.h"
PAYLOAD Payload* New(int version,char *name,char *data){
    unsigned char buf[CONSTS MAX_BUFFER]; // much bigger than needed
	size_t payload_len = -1;
	const unsigned char *digest = NULL;
}