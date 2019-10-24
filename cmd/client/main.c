#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include  "../../pkg/shared/consts.h"
#include  "../../pkg/payload/payload.h"
#include  "../../pkg/protocol/protocol.h"

// https://stackoverflow.com/questions/19127398/socket-programming-read-is-reading-all-of-my-writes
void Loop(int socketFd);
int send_payload(int socket_desc, const char *src);
void setupAndConnect(struct sockaddr_in *serverAddr, struct hostent *host, int socketFd, long port);
void setNonBlock(int fd);
void interruptHandler(int sig);

static int socketFd;

int main(int argc, char *argv[])
{
    char *filename;
    struct sockaddr_in serverAddr;
    struct hostent *host;
    long port;

    if(argc != 3)
    {
        fprintf(stderr, "./client [host] [port] \n");
        exit(1);
    }
    if((host = gethostbyname(argv[1])) == NULL)
    {
        fprintf(stderr, "Couldn't get host name\n");
        exit(1);
    }
    port = strtol(argv[2], NULL, 0);
    if((socketFd = socket(AF_INET, SOCK_STREAM, 0))== -1)
    {
        fprintf(stderr, "Couldn't create socket\n");
        exit(1);
    }
    setupAndConnect(&serverAddr, host, socketFd, port);
    setNonBlock(socketFd);
    setNonBlock(0);

    //Set a handler for the interrupt signal
    signal(SIGINT, interruptHandler);
    Loop(socketFd);
}

//Main loop to take in input and display output result from server
void Loop(int socketFd)
{
    fd_set clientFds;
    while(1)
    {
        //Reset the fd set each time since select() modifies it
        FD_ZERO(&clientFds);
        FD_SET(socketFd, &clientFds);
        FD_SET(0, &clientFds);
        //wait for an available fd
        if(select(FD_SETSIZE, &clientFds, NULL, NULL, NULL) != -1)
        {
            for(int fd = 0; fd < FD_SETSIZE; fd++)
            {
                if(FD_ISSET(fd, &clientFds))
                {
                    //receive data from server
                    if(fd == socketFd)
                    {
                        char msgBuffer[CONSTS MAX_BUFFER];
                        int numBytesRead = read(socketFd, msgBuffer, CONSTS MAX_BUFFER - 1);
                        msgBuffer[numBytesRead] = '\0';
                        if (numBytesRead>1){
                            printf("[receive data from server] %s\n", msgBuffer);
                        }
                        memset(&msgBuffer, 0, sizeof(msgBuffer));
                    }
                    //read from keyboard (stdin) and send to server
                    else if(fd == 0)
                    {
                        char payloadBuffer[CONSTS MAX_BUFFER];
                        fgets(payloadBuffer, CONSTS MAX_BUFFER - 1, stdin);
                        if(strcmp(payloadBuffer, "/exit\n") == 0)
                            //Reuse the interruptHandler function to disconnect the client
                            interruptHandler(-1);
                        else
                        {
                            if(send_payload(socketFd, payloadBuffer) == -1) perror("write failed: ");
                        }
                    }
                }
            }
        }
    }
}


int send_payload(int socket_desc, const char *src)
{
    unsigned char buffer[MAX_BUFFER];
    int mesg_length = request_comm_msg(buffer,socket_desc, src);
    return send(socket_desc, buffer, mesg_length, 0);
}

//Sets up the socket and connects
void setupAndConnect(struct sockaddr_in *serverAddr, struct hostent *host, int socketFd, long port)
{
    memset(serverAddr, 0, sizeof(serverAddr));
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_addr = *((struct in_addr *)host->h_addr_list[0]);
    serverAddr->sin_port = htons(port);
    if(connect(socketFd, (struct sockaddr *) serverAddr, sizeof(struct sockaddr)) < 0)
    {
        perror("Couldn't connect to server");
        exit(1);
    }
}

//Sets the fd to nonblocking
void setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if(flags < 0)
        perror("fcntl failed");

    fcntl(fd, F_SETFL, flags);
}

//Notify the server when the client exits by sending "/exit"
void interruptHandler(int sig_unused)
{
    if(write(socketFd, "/exit\n", CONSTS MAX_BUFFER - 1) == -1)
        perror("write failed: ");

    close(socketFd);
    exit(1);
}