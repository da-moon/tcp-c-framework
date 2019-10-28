#include "server.h"

void Bind(struct sockaddr_in *serverAddr, int socketFd, long port) {
  memset(serverAddr, 0, sizeof(*serverAddr));
  serverAddr->sin_family = AF_INET;
  serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr->sin_port = htons(port);
  if (bind(socketFd, (struct sockaddr *)serverAddr,
           sizeof(struct sockaddr_in)) == -1) {
    perror("Socket bind failed: ");
    exit(1);
  }
}
void InitializeRPCHandlers(int socketFd) {
  Multiplexer mux;
  mux.conn = malloc(sizeof *mux.conn);
  mux.conn->numClients = 0;
  mux.conn->socketFd = socketFd;
  mux.Queue = NewQueue();
  mux.clientListMutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_t connectionThread;
  pthread_t payloadThread;
  pthread_mutex_init(mux.clientListMutex, NULL);
  // Start thread to handle new client connections
  if ((pthread_create(&connectionThread, NULL, (void *)&Multiplex,
                      (void *)&mux)) == 0) {
    fprintf(stderr, " [DEBUG] Multiplexed Connection to client\n");
  }

  FD_ZERO(&(mux.readFds));
  FD_SET(socketFd, &(mux.readFds));

  // Start thread to handle requests received
  if ((pthread_create(&payloadThread, NULL, (void *)&ServerRequestHandler,
                      (void *)&mux)) == 0) {
    fprintf(stderr, "[DEBUG] Request handler started\n");
  }
  pthread_join(connectionThread, NULL);
  pthread_join(payloadThread, NULL);
  DestroyQueue(mux.Queue);
  pthread_mutex_destroy(mux.clientListMutex);
  free(mux.clientListMutex);
  free(mux.conn);
}