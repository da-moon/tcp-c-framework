#include "handlers.h"

void *ServerRequestHandler(void *arg) {
  Multiplexer *mux = (Multiplexer *)arg;
  while (1) {
    // Obtain lock and pop message from Queue when not empty
    pthread_mutex_lock((mux->Queue)->mutex);
    while ((mux->Queue)->empty) {
      pthread_cond_wait((mux->Queue)->notEmpty, (mux->Queue)->mutex);
    }
    const Message *message = QUEUE Pop(mux->Queue);
    pthread_mutex_unlock((mux->Queue)->mutex);
    pthread_cond_signal((mux->Queue)->notFull);
    for (int i = 0; i < mux->conn->numClients; i++) {
      int socket = mux->conn->clientSockets[i];

      switch (message->protocol) {
        if (socket != 0) {

        case BROADCAST_REQUEST: {
          unsigned char reply[MAX_BUFFER];

          BroadcastProtocolServerHandler(reply, message);
          int result = send(socket, reply, MAX_BUFFER, 0);
          //   int result = write(socket, message->body, MAX_BUFFER);
          if (result == -1) {
            perror("Socket write failed: ");
          }
          break;
          //
        }
        case ECHO_REQUEST: {
          if (message->message_sender == socket) {
            unsigned char reply[MAX_BUFFER];
            EchoProtocolServerHandler(reply, message);
            int result = send(socket, reply, MAX_BUFFER, 0);
            //   int result = write(socket, message->body, MAX_BUFFER);
            if (result == -1) {
              perror("Socket write failed: ");
            }
          }
          break;
        }
        default: {
          break;
        }
        }
      }
    }
  }
}