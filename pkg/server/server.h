#ifndef SERVER
#define SERVER
    #include "../shared/consts.h"
    #include "../queue/queue.h"
    #include "../multiplexer/multiplexer.h"

    void Run(int socketFd);
    void Bind(struct sockaddr_in *serverAddr, int socketFd, long port);

#endif
