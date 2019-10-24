#ifndef HANDLER
#define HANDLER
    #include "../shared/consts.h"
    #include "../shared/utils.h"
    #include "../payload/payload.h"
    #include "../protocol/protocol.h"

    typedef struct {
        int socketFd;
        int clientSockets[CONSTS MAX_BUFFER];
        int numClients;
    } Connection;
    void ProtocolHandler(char* name,Connection *mux,Payload *message);
#endif
