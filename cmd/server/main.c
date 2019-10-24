#include "../../pkg/multiplexer/multiplexer.h"
#include "../../pkg/queue/queue.h"
#include "../../pkg/server/server.h"
#include "../../pkg/shared/consts.h"

int main(int argc, char *argv[]) {
  struct sockaddr_in serverAddr;
  long port = 8080;
  int socketFd;

  if (argc == 2)
    port = strtol(argv[1], NULL, 0);

  if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Socket creation failed");
    exit(1);
  }

  Bind(&serverAddr, socketFd, port);
  if (listen(socketFd, 1) == -1) {
    perror("listen failed: ");
    exit(1);
  }
  Run(socketFd, (void *)&RequestHandler);
  close(socketFd);
}
