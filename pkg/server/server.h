#ifndef SERVER
#define SERVER
#include "../handlers/handlers.h"
#include "../multiplexer/multiplexer.h"
#include "../queue/queue.h"
#include "../shared/consts.h"
// AddHandler - Spawns the new client handler thread
// and message consumer thread based on passed value
// it returns a multiplexer objext in which the trheads are wrapped
void InitializeRPCHandlers(int socketFd);
// Bind - Sets up and binds the socket
void Bind(struct sockaddr_in *serverAddr, int socketFd, long port);
#endif
