#include "multiplexer.h"

// Adds client's fd to list of client fds and
// spawns a new ClientHandler thread for it
void *Multiplex(void *arg) {
  Multiplexer *mux = (Multiplexer *)arg;
  while (1) {
    int clientSocketFd = accept((mux->conn)->socketFd, NULL, NULL);
    if (clientSocketFd > 0) {
      fprintf(stderr, " accepted new client. Socket: %d\n", clientSocketFd);
      // Obtain lock on clients list and add new client in
      pthread_mutex_lock(mux->clientListMutex);
      if ((mux->conn)->numClients < MAX_BUFFER) {
        // Add new client to list
        for (int i = 0; i < MAX_BUFFER; i++) {
          if (!FD_ISSET((mux->conn)->clientSockets[i], &(mux->readFds))) {
            (mux->conn)->clientSockets[i] = clientSocketFd;
            i = MAX_BUFFER;
          }
        }

        FD_SET(clientSocketFd, &(mux->readFds));
        mux->clientSocketFd = clientSocketFd;

        pthread_t clientThread;
        if ((pthread_create(&clientThread, NULL, (void *)&ClientHandler,
                            (void *)mux)) == 0) {
          (mux->conn)->numClients++;
          fprintf(stderr,
                  "Client connection to server has been successfully "
                  "multiplexed on socket: %d\n",
                  clientSocketFd);
        } else
          close(clientSocketFd);
      }
      pthread_mutex_unlock(mux->clientListMutex);
    }
  }
}

// ClientHandler - Listens for payloads from client to add to queue
void *ClientHandler(void *arg) {
  Multiplexer *mux = (Multiplexer *)arg;

  Queue *q = mux->Queue;
  int clientSocketFd = mux->clientSocketFd;
  char *header = malloc(PROTOCOL_HEADER_LEN + 1);
  int n;
  while ((n = read(clientSocketFd, header, PROTOCOL_HEADER_LEN)) > 1) {
    uint16_t magic = ExtractMessageMagic(header);
    if (magic == 0xC0DE) {
      uint16_t protocol = ExtractMessageProtocol(header);
      uint32_t payload_size = ExtractMessageBodySize(header);
      char *recv_buffer = malloc(payload_size);
      read(clientSocketFd, recv_buffer, payload_size);
      if (strcmp(recv_buffer, "/exit\n") == 0) {
        fprintf(stderr, "Client on socket %d has disconnected.\n",
                clientSocketFd);
        Disconnect(mux, clientSocketFd);
        return NULL;
      } else {
        // Wait for Queue to not be full before pushing message
        while (q->full) {
          pthread_cond_wait(q->notFull, q->mutex);
        }
        pthread_mutex_lock(q->mutex);

        Message message;
        message.message_sender = clientSocketFd;
        message.magic = magic;
        message.protocol = protocol;
        message.size = payload_size;
        message.body = recv_buffer;
        Push(q, clientSocketFd, message);
      }
      pthread_mutex_unlock(q->mutex);
      pthread_cond_signal(q->notEmpty);
    }
  }
}

// Removes the socket from the list of active client sockets and closes it
void Disconnect(Multiplexer *data, int clientSocketFd) {
  pthread_mutex_lock(data->clientListMutex);
  for (int i = 0; i < MAX_BUFFER; i++) {
    if ((data->conn)->clientSockets[i] == clientSocketFd) {
      (data->conn)->clientSockets[i] = 0;
      close(clientSocketFd);
      (data->conn)->numClients--;
      i = MAX_BUFFER;
    }
  }
  pthread_mutex_unlock(data->clientListMutex);
}
