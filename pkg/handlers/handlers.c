#include "handlers.h"

void *ServerRequestHandler(void *arg) {
  Multiplexer *mux = (Multiplexer *)arg;
  while (1) {
    // Obtain lock and pop message from Queue when not empty
    pthread_mutex_lock((mux->Queue)->mutex);
    while ((mux->Queue)->empty) {
      pthread_cond_wait((mux->Queue)->notEmpty, (mux->Queue)->mutex);
    }
    const Message message = QUEUE Pop(mux->Queue);
    pthread_mutex_unlock((mux->Queue)->mutex);
    pthread_cond_signal((mux->Queue)->notFull);

    for (int i = 0; i < mux->conn->numClients; i++) {
      int socket = mux->conn->clientSockets[i];

      switch (message.protocol) {
        if (socket != 0) {

        case ECHO_REQUEST: {
          if (message.message_sender == socket) {
            EchoProtocolServerHandler(socket, message);
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