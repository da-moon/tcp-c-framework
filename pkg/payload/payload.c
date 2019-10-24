#include "payload.h"
Payload* NewPayload(char* msg,int origin){
    Payload *p  = (Payload *)malloc(sizeof(Payload));
    if(p == NULL)
    {
        perror("Couldn't allocate anymore memory!");
        exit(EXIT_FAILURE);
    }
    p->data = malloc(sizeof(msg));
    p->origin = origin;
    p->type = 'D';
    strcpy(p->data, msg);
    return p;
}
