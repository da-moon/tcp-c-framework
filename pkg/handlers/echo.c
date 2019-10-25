#include "handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void EchoProtocolHandleServerReply(int socket) {
  char msgBuffer[MAX_BUFFER];
  int numBytesRead = read(socket, msgBuffer, MAX_BUFFER - 1);
  if (numBytesRead > 1) {
    Message *reply = UnmarshallMessage(socket, msgBuffer);
    // reply->body[reply->size + 1] = '\0';
    fprintf(stderr, "%s\n", reply->body);
  }

  memset(&msgBuffer, 0, sizeof(msgBuffer));
}
void EchoProtocolSendRequestToServer(int socket) {
  char payloadBuffer[MAX_BUFFER];
  fgets(payloadBuffer, MAX_BUFFER - 1, stdin);
  unsigned char request[MAX_BUFFER];
  int mesg_length =
      MarshallMessage(request, 0xC0DE, ECHO_REQUEST, payloadBuffer);
  if (send(socket, request, MAX_BUFFER, 0) == -1)
    perror("write failed: ");
}

void EchoProtocolServerHandler(char *result, Message *message) {
  unsigned char *src[MAX_BUFFER];
  fprintf(stderr, "[DEBUG] server : recieved message data: %s\n",
          message->body);
  fprintf(stderr, "[DEBUG] server : Echoing .... \n");
  memset(src, 0, MAX_BUFFER);
  strcpy(src, "[ECHO-PROTOCOL] ");
  strcat(src, message->body);
  MarshallMessage(result, 0xC0DE, ECHO_REPLY, src);
}