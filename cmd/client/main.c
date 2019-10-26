
#include "../../pkg/client/client.h"

// https://stackoverflow.com/questions/19127398/socket-programming-read-is-reading-all-of-my-writes

static int connection_socket;
void interrupt_handler(int signal);

int main(int argc, char *argv[]) {
  struct sockaddr_in serverAddr;
  struct hostent *host;
  long port;
  //   if (argc < 4) {
  // fprintf(stderr, "./client [host] [port] [protocol]\n");
  // exit(1);
  //   }
  if ((host = gethostbyname(argv[1])) == NULL) {
    fprintf(stderr, "Couldn't get host name\n");
    exit(1);
  }
  port = strtol(argv[2], NULL, 0);
  if ((connection_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "Couldn't create socket\n");
    exit(1);
  }
  establish_connection_with_server(&serverAddr, host, connection_socket, port);
  set_non_blocking(connection_socket);
  set_non_blocking(0);
  // Set a handler for the interrupt signal
  signal(SIGINT, interrupt_handler);
  Loop(connection_socket);
}
void interrupt_handler(int signal) { leave_request(connection_socket); }
