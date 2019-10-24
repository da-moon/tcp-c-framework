#ifndef SERVER
#define SERVER
#include "../multiplexer/multiplexer.h"
#include "../queue/queue.h"
#include "../shared/consts.h"
void Run(int socketFd, void *(*handler)(void *));
void Bind(struct sockaddr_in *serverAddr, int socketFd, long port);
void AddHandler(char *name, void *(*handler)(void *));
#endif
