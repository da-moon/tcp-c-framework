#include  "../../pkg/shared/consts.h"
#include  "../../pkg/server/server.h"
#include  "../../pkg/queue/queue.h"

int main(int argc, char *argv[])
{
    struct sockaddr_in serverAddr;
    long port = 9999;
    int socketFd;

    if(argc == 2) port = strtol(argv[1], NULL, 0);

    if((socketFd = socket(AF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    SERVER Bind(&serverAddr, socketFd, port);
    if(listen(socketFd, 1) == -1)
    {
        perror("listen failed: ");
        exit(1);
    }

    SERVER Run(socketFd);

    close(socketFd);
}
