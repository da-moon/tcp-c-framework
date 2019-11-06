#include "handlers.h"
void ChangeDirectoryProtocolSendRequestToServer(int socket) {
  printf("Enter Directory name For server TO change into\n");
  char input[MAX_BUFFER];
  fgets(input, MAX_BUFFER - 1, stdin);
  char *arr_ptr = &input[0];
  arr_ptr[strlen(arr_ptr)-1] ='\0' ;
  char *request = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
  int mesg_length = MarshallMessage(request, 0xC0DE, CHANGE_DIR_REQUEST, arr_ptr);
  Message message;
  message.body = (char *)(request + PROTOCOL_HEADER_LEN);
  if (send(socket, request, strlen(arr_ptr) + PROTOCOL_HEADER_LEN, 0) == -1)
    perror("write failed: ");
  fprintf(stderr,
          "[DEBUG] client : sending change directory request for file %s to "
          "server\n",
          message.body);
}

// ROOT_DIR
void ChangeDirectoryProtocolServerHandler(int socket, Message message) {
  message.body = ROOT_DIR;
  char *arr_ptr = &message.body[0];
  int payload_length = strlen(arr_ptr);
  char *reply = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
  int mesg_length = MarshallMessage(reply, 0xC0DE, READY_REPLY, arr_ptr);
  if (send(socket, reply, strlen(arr_ptr) + PROTOCOL_HEADER_LEN, 0) == -1)
    perror("write failed: ");
  fprintf(stderr,
          "[DEBUG] Change Directory Handler Server : Replying back .... \n");
}
