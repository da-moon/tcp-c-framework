#include "multiplexer.h"

// Thread to handle new connections. Adds client's fd to list of client fds and
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
  unsigned char buffer[MAX_BUFFER];
  int read_size;
  while ((read_size = recv(clientSocketFd, buffer, MAX_BUFFER, 0)) > 0) {

    // If the client sent /exit\n,
    // remove them from the client list and close their socket
    if (strcmp(buffer, "/exit\n") == 0) {
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
      // extracting message length
      const int message_size = ExtractMessageBodySize(buffer);

      if (message_size != 0) {

        const Message *message = UnmarshallMessage(clientSocketFd, buffer);
        if (message != NULL) {
          Push(q, clientSocketFd, message);
        }
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
