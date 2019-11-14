#ifndef MULTIPLEXER
#define MULTIPLEXER
#include "../message/message.h"
#include "../queue/queue.h"
#include "../shared/consts.h"
#include "../shared/utils.h"
#include <ctype.h>
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
// accept
#include <netdb.h>
// strcmp
#include <string.h>
// close - read - write
#include <unistd.h>
// fd_set
#include <sys/select.h>
// connection  struct
typedef struct {
  int socketFd;
  int clientSockets[MAX_BUFFER];
  int numClients;
} Connection;
// Struct containing important data for the server to work.
// Namely the list of client sockets, that list's mutex,
// the server's socket for new connections, and the message Queue
typedef struct {
  fd_set readFds;
  Connection *conn;
  pthread_mutex_t *clientListMutex;
  char dir[256];
  QUEUE Queue *Queue;
  // clientSocketFd is used to have a local copy of
  // the socket per connection
  // using the value in conn causes the server to shit the bed
  // and behave erraticaly
  int clientSocketFd;
} Multiplexer;

void Disconnect(Multiplexer *data, int clientSocketFd);
void *Multiplex(void *arg);
void *ClientHandler(void *arg);
#endif
