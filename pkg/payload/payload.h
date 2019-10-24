#ifndef PAYLOAD
#define PAYLOAD
    #include <stddef.h>
    #include <stdio.h>
    #include <stdlib.h>
    // #include <limits.h>
    // #include <stddef.h>
    #include <stdint.h>
    // For string helper methods
    #include <string.h>
    #include "../shared/consts.h"
   typedef struct {
        char type;
        int origin;
        char *data;
    } Payload;

    Payload* NewPayload(char* msg,int origin);

#endif