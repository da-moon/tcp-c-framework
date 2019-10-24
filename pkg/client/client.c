#include "client.h"
// Main loop to take in input and display output result from server
void Loop(int socket) {
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
            handle_reply(socket);
          }
          // read from keyboard (stdin) and send to server
          else if (connection_file_descriptor_socket == 0) {
            read_stdin_and_send(socket);
          }
        }
      }
    }
  }
}
void handle_reply(int socket) {
  char msgBuffer[CONSTS MAX_BUFFER];
  int numBytesRead = read(socket, msgBuffer, CONSTS MAX_BUFFER - 1);
  if (numBytesRead > 1) {
    msgBuffer[numBytesRead] = '\0';
    fprintf(stderr, "[DEBUG] reading server reply on socket [%d] \n", socket);
    fprintf(stderr, "[DEBUG] receive data from server - %s\n", msgBuffer);
  }
  memset(&msgBuffer, 0, sizeof(msgBuffer));
}
void read_stdin_and_send(int socket) {
  char payloadBuffer[CONSTS MAX_BUFFER];
  fgets(payloadBuffer, CONSTS MAX_BUFFER - 1, stdin);
  if (SendMessageOverTheWire(socket, REQ_SEND_MSG, payloadBuffer) == -1)
    perror("write failed: ");
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
  if (write(socket, "/exit\n", CONSTS MAX_BUFFER - 1) == -1)
    perror("write failed: ");

  close(socket);
  exit(1);
}