#include "handlers.h"

void BroadcastProtocolHandleServerReply(int socket) {
  char msgBuffer[CONSTS MAX_BUFFER];
  int numBytesRead = read(socket, msgBuffer, CONSTS MAX_BUFFER - 1);
  if (numBytesRead > 1) {
    msgBuffer[numBytesRead] = '\0';
    fprintf(stderr,
            "[DEBUG] BROADCAST PROTOCOL - receive data from server - %s\n",
            msgBuffer);
  }
  memset(&msgBuffer, 0, sizeof(msgBuffer));
}
void BroadcastProtocolSendRequestToServer(int socket) {
  char payloadBuffer[CONSTS MAX_BUFFER];
  fgets(payloadBuffer, CONSTS MAX_BUFFER - 1, stdin);
  if (SendMessageOverTheWire(socket, BROADCAST_REQUEST, payloadBuffer) == -1)
    perror("write failed: ");
}
