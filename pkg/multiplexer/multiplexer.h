#ifndef MULTIPLEXER
#define MULTIPLEXER
    #include "../shared/consts.h"
    #include "../queue/queue.h"
    #include "../handler/handler.h"

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
        int clientSocketFd;

    } Multiplexer;

    // Simple struct to hold the Multiplexer and the new client's socket fd.
    // Used only in the client handler thread.
    typedef struct {
        Multiplexer *data;
    } Payload;
    void *Multiplex(void *data);
    void Disconnect(Multiplexer *data, int clientSocketFd);
    void *ClientHandler(void *chv);
    void *RequestHandler(void *data);
    // void *FileHandler(void *data);
    // char *trimmsg(char *str);
#endif
