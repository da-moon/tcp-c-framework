#ifndef HANDLER
#define HANDLER
    #include "../shared/consts.h"
    #include "../shared/utils.h"
    typedef struct {
        int socketFd;
        int clientSockets[CONSTS MAX_BUFFER];
        int numClients;
    } Connection;
    void ProtocolHandler(char* name,Connection *mux,char* msg);
#endif
