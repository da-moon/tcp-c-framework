#include "handler.h"


HANDLER void ProtocolHandler(char* name,Connection *conn,Payload *message){
    fprintf(stderr, "PROTOCOL: [%s] | MESSAGE SENDER [%d] | Message data : [%s]\n" ,name,message->origin,message->data);
        for(int i = 0; i < conn->numClients; i++)
        {
            int socket = conn->clientSockets[i];
            if(socket != 0){

                int result=   write(socket, message->data, MAX_BUFFER) ;
              if (result ==-1) {
                perror("Socket write failed: ");
              }
            }
        }
    }