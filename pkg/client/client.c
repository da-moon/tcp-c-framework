#include "client.h"
// Main loop to take in input and display output result from server
void Loop(int socket, int argc, char *argv[]) {
  fd_set clientFds;
  while (1) {
    // Reset the connection_file_descriptor_socket set each time since select()
    // modifies it
    FD_ZERO(&clientFds);
    FD_SET(socket, &clientFds);
    FD_SET(0, &clientFds);
    // wait for an available connection_file_descriptor_socket
    if (select(FD_SETSIZE, &clientFds, NULL, NULL, NULL) != -1) {
      for (int connection_file_descriptor_socket = 0;
           connection_file_descriptor_socket < FD_SETSIZE;
           connection_file_descriptor_socket++) {
        if (FD_ISSET(connection_file_descriptor_socket, &clientFds)) {
          // receive data from server
          if (connection_file_descriptor_socket == socket) {
            //   reacting to a broadcast protocol server reply
            // BroadcastProtocolHandleServerReply(socket);
            //   reacting to a echo protocol server reply

            EchoProtocolHandleServerReply(socket);
          }
          // read from keyboard (stdin) and send to server
          else if (connection_file_descriptor_socket == 0) {
            //   sending a broadcast protocol msg to server
            // BroadcastProtocolSendRequestToServer(socket);
            //   sending a echo protocol msg to server
            EchoProtocolSendRequestToServer(socket);
          }
        }
      }
    }
  }
}

void establish_connection_with_server(struct sockaddr_in *serverAddr,
                                      struct hostent *host,
                                      int connection_socket, long port) {
  memset(serverAddr, 0, sizeof(serverAddr));
  serverAddr->sin_family = AF_INET;
  serverAddr->sin_addr = *((struct in_addr *)host->h_addr_list[0]);
  serverAddr->sin_port = htons(port);
  if (connect(connection_socket, (struct sockaddr *)serverAddr,
              sizeof(struct sockaddr)) < 0) {
    perror("Couldn't connect to server");
    exit(1);
  }
}

void set_non_blocking(int file_descriptor) {
  int flags = fcntl(file_descriptor, F_GETFL);
  if (flags < 0)
    perror("fcntl failed");

  fcntl(file_descriptor, F_SETFL, flags);
}

void leave_request(int socket) {
  if (write(socket, "/exit\n", MAX_BUFFER - 1) == -1)
    perror("write failed: ");

  close(socket);
  exit(1);
}
