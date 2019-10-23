#ifndef PAYLOAD
#define PAYLOAD
    #include <stddef.h>
    #include <stdio.h>
    #include <stdlib.h>
    // For string helper methods
    #include <string.h>
    #include "../shared/consts.h"
   typedef struct {
        char *data[CONSTS MAX_BUFFER];
    } Payload;
    Payload* NewPayload(char* msg);
#endif