#include "handlers.h"
void ListDirectoryProtocolSendRequestToServer(int socket) {
  printf("Enter Directory name For server list\n");
  char input[MAX_BUFFER];
  fgets(input, MAX_BUFFER - 1, stdin);
  char *arr_ptr = &input[0];
  char *request = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
  int mesg_length = MarshallMessage(request, 0xC0DE, LIST_DIR_REQUEST, input);
  Message message;
  message.body = (char *)(request + PROTOCOL_HEADER_LEN);

  if (send(socket, request, strlen(arr_ptr) + PROTOCOL_HEADER_LEN, 0) == -1)
    perror("write failed: ");
  fprintf(stderr,
          "[DEBUG] client : sending download reques for file %s to server\n",
          message.body);
}
void ListDirectoryProtocolServerHandler(int socket, Message message) {
  char *arr_ptr = &message.body[0];
  int payload_length = strlen(arr_ptr);

  char *reply = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
  int mesg_length = MarshallMessage(reply, 0xC0DE, message.protocol, arr_ptr);
  if (send(socket, reply, strlen(arr_ptr) + PROTOCOL_HEADER_LEN, 0) == -1)
    perror("write failed: ");
  fprintf(stderr, "[DEBUG] Echo Handler Server : Replying bac .... \n");
}
