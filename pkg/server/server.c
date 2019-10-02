#include "server.h"
#include "../multiplexer/multiplexer.h"

//Spawns the new client handler thread and message consumer thread
SERVER void Run(int socketFd)
{
    MULTIPLEXER Multiplexer data;
    data.numClients = 0;
    data.socketFd = socketFd;
    data.Queue = QUEUE  New();
    data.clientListMutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(data.clientListMutex, NULL);

    //Start thread to handle new client connections
    pthread_t connectionThread;
    if((pthread_create(&connectionThread, NULL, (void *)&MULTIPLEXER Multiplex, (void *)&data)) == 0)
    {
        fprintf(stderr, "Multiplexed Connection to client\n");
    }

    FD_ZERO(&(data.readFds));
    FD_SET(socketFd, &(data.readFds));

    //Start thread to handle requests received
    pthread_t messagesThread;
    if((pthread_create(&messagesThread, NULL, (void *)&MULTIPLEXER FileHandler, (void *)&data)) == 0)
    // if((pthread_create(&messagesThread, NULL, (void *)&MULTIPLEXER RequestHandler, (void *)&data)) == 0)
    {
        fprintf(stderr, "Request handler started\n");
    }

    pthread_join(connectionThread, NULL);
    pthread_join(messagesThread, NULL);
    QUEUE Destroy(data.Queue);
    pthread_mutex_destroy(data.clientListMutex);
    free(data.clientListMutex);
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
