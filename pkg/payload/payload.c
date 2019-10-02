#include "payload.h"
PAYLOAD int Payload(char *result,char* protocol,char* data){
    memset(result, 0, CONSTS MAX_BUFFER);
    strcat(result, "@");
    strcpy(result, protocol);
    strcat(result, "_");
    strcat(result, data);
    return  0;
}
PAYLOAD int ExtractProtocol(char *result,char* data){
    return  0;
}
PAYLOAD int ExtractData(char *result,char* data){
    return  0;
}
