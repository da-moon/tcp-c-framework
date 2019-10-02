#include "server.h"
#include "../multiplexer/multiplexer.h"
// mux needs to be global considering that it needs to exist during the whole
// lifecycle of the program .
// as new client join, their information would get added to mux
MULTIPLEXER Multiplexer mux;
int num_handlers = 0 ;

//Spawns the new client handler thread and message consumer thread
SERVER void Run(int socketFd,void *(*handler) (void *))
{
    mux.conn = malloc(sizeof *mux.conn);
    mux.conn->numClients=0;
    mux.conn->socketFd=socketFd;
    mux.Queue = QUEUE  New();
    mux.clientListMutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mux.clientListMutex, NULL);

    //Start thread to handle new client connections
    pthread_t connectionThread;
    if((pthread_create(&connectionThread, NULL, (void *)&MULTIPLEXER Multiplex, (void *)&mux)) == 0)
    {
        fprintf(stderr, "Multiplexed Connection to client\n");
    }

    FD_ZERO(&(mux.readFds));
    FD_SET(socketFd, &(mux.readFds));

    //Start thread to handle requests received
    pthread_t messagesThread;
    if((pthread_create(&messagesThread, NULL, handler, (void *)&mux)) == 0)
    {
        fprintf(stderr, "Request handler started\n");
    }

    pthread_join(connectionThread, NULL);
    pthread_join(messagesThread, NULL);
    QUEUE Destroy(mux.Queue);
    pthread_mutex_destroy(mux.clientListMutex);
    free(mux.clientListMutex);
    free(mux.conn);
}
//Sets up and binds the socket
SERVER void Bind(struct sockaddr_in *serverAddr, int socketFd, long port)
{
    memset(serverAddr, 0, sizeof(*serverAddr));
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr->sin_port = htons(port);

    if(bind(socketFd, (struct sockaddr *)serverAddr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Socket bind failed: ");
        exit(1);
    }
}
