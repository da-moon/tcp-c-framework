#include "handler.h"


HANDLER void ProtocolHandler(char* name,Connection *conn,char* msg){
    fprintf(stderr, "PROTOCOL: [%s]\n", name);
        for(int i = 0; i < conn->numClients; i++)
        {
            int socket = conn->clientSockets[i];
            if(socket != 0 && write(socket, msg, CONSTS MAX_BUFFER - 1) == -1){
            perror("Socket write failed: ");
        }
    }
}
