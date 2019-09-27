#ifndef SERVER
#define SERVER
    #include "../shared/consts.h"
    #include "../queue/queue.h"
    #include "../multiplexer/multiplexer.h"
    // #include <pthread.h>
    // #include <stdlib.h>
    // #include <stdio.h>
    // #include <unistd.h>
    // #include <errno.h>
    // #include <string.h>
    // #include <netdb.h>
    // #include <sys/select.h>
    // #include <sys/types.h>
    // #include <sys/socket.h>
    // #include <netinet/in.h>
    // #include <arpa/inet.h>
    // #include <fcntl.h>
    void Run(int socketFd);
    void Bind(struct sockaddr_in *serverAddr, int socketFd, long port);

#endif
