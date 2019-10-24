#ifndef MULTIPLEXER
#define MULTIPLEXER
    #include "../shared/consts.h"
    #include "../shared/utils.h"
    #include "../queue/queue.h"
    #include "../handler/handler.h"
    #include "../payload/payload.h"
    #include "../protocol/protocol.h"
    #include <ctype.h>
    #include <stdio.h>
    // accept
    #include <netdb.h>
    // strcmp
    #include <string.h>
    // close - read - write
    #include <unistd.h>
    // fd_set
    #include <sys/select.h>
    // Struct containing important data for the server to work.
    // Namely the list of client sockets, that list's mutex,
    // the server's socket for new connections, and the message Queue
    typedef struct {
        fd_set readFds;
        HANDLER Connection *conn;
        pthread_mutex_t *clientListMutex;
        QUEUE Queue *Queue;
        // clientSocketFd is used to have a local copy of
        // the socket per connection
        // using the value in conn causes the server to shit the bed
        // and behave erraticaly
        int clientSocketFd;
    } Multiplexer;

    void Disconnect(Multiplexer *data, int clientSocketFd);
    void *Multiplex(void *data);
    void *ClientHandler(void *chv);
    void *RequestHandler(void *data);
#endif
