#include "payload.h"
Payload* NewPayload(char* msg){
    Payload *p = (Payload *)malloc(sizeof(Payload));
    if(p == NULL)
    {
        perror("Couldn't allocate anymore memory!");
        exit(EXIT_FAILURE);
    }
    memset(p->data, 0, CONSTS MAX_BUFFER);
    strcpy(p->data, msg);
    return p;
}
