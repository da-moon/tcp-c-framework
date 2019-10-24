#ifndef CLIENT
#define CLIENT
#include "../handlers/handlers.h"
#include "../shared/consts.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
// main connection loop for client
void Loop(int connection_socket);
// Sets up the socket and establishes connection with server
void establish_connection_with_server(struct sockaddr_in *serverAddr,
                                      struct hostent *host,
                                      int connection_socket, long port);
// Sets the file descriptor to nonblocking mode
void set_non_blocking(int file_descriptor);

// Notify the server when the client exits by sending "/exit"
void leave_request(int socket);
// reads server reply and multiplexes it to an action to be taken
// based on the reply
void handle_reply(int socket);
// reads from stdin and sends a message to server
void read_stdin_and_send(int socket);
#endif
