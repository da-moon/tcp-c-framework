#ifndef HANDLER
#define HANDLER
    #include "../multiplexer/multiplexer.h"
    #include "../shared/utils.h"
    typedef struct {
        char name[CONSTS MAX_BUFFER];
        // func is the method that is invoked as protocol is triggered
        // func takes a int[socket]and string[payload] as arguments
        void *(*func) (char*);
    } Handler;
    void *RequestHandler(void *data);
    void *FileHandler(void *data);
    void ProtocolHandler(char* name,int socket,char* payload);
#endif
