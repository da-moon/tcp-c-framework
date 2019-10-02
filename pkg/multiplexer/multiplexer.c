#include "multiplexer.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//Thread to handle new connections. Adds client's fd to list of client fds and spawns a new ClientHandler thread for it
MULTIPLEXER void *Multiplex(void *data)
{
    Multiplexer *chatData = (Multiplexer *) data;
    while(1)
    {
        int clientSocketFd = accept(chatData->socketFd, NULL, NULL);
        if(clientSocketFd > 0)
        {
            fprintf(stderr, "MULTIPLEXER accepted new client. Socket: %d\n", clientSocketFd);

            //Obtain lock on clients list and add new client in
            pthread_mutex_lock(chatData->clientListMutex);
            if(chatData->numClients < CONSTS MAX_BUFFER)
            {
                //Add new client to list
                for(int i = 0; i < CONSTS MAX_BUFFER; i++)
                {
                    if(!FD_ISSET(chatData->clientSockets[i], &(chatData->readFds)))
                    {
                        chatData->clientSockets[i] = clientSocketFd;
                        i = CONSTS MAX_BUFFER;
                    }
                }

                FD_SET(clientSocketFd, &(chatData->readFds));

                //Spawn new thread to handle client's request
                Payload chv;
                chv.clientSocketFd = clientSocketFd;
                chv.data = chatData;

                pthread_t clientThread;
                if((pthread_create(&clientThread, NULL, (void *)&ClientHandler, (void *)&chv)) == 0)
                {
                    chatData->numClients++;
                    fprintf(stderr, "Client has joined chat. Socket: %d\n", clientSocketFd);
                }
                else
                    close(clientSocketFd);
            }
            pthread_mutex_unlock(chatData->clientListMutex);
        }
    }
}

//Removes the socket from the list of active client sockets and closes it
MULTIPLEXER void Disconnect(Multiplexer *data, int clientSocketFd)
{
    pthread_mutex_lock(data->clientListMutex);
    for(int i = 0; i < CONSTS MAX_BUFFER; i++)
    {
        if(data->clientSockets[i] == clientSocketFd)
        {
            data->clientSockets[i] = 0;
            close(clientSocketFd);
            data->numClients--;
            i = CONSTS MAX_BUFFER;
        }
    }
    pthread_mutex_unlock(data->clientListMutex);
}

//ClientHandler - Listens for payloads from client to add to queue
MULTIPLEXER void *ClientHandler(void *chv)
{
    Payload *vars = (Payload *)chv;
    Multiplexer *data = (Multiplexer *)vars->data;

    QUEUE Queue *q = data->Queue;
    int clientSocketFd = vars->clientSocketFd;

    char msgBuffer[CONSTS MAX_BUFFER];
    while(1)
    {
        int numBytesRead = read(clientSocketFd, msgBuffer, CONSTS MAX_BUFFER - 1);
        msgBuffer[numBytesRead] = '\0';

        //If the client sent /exit\n, remove them from the client list and close their socket
        if(strcmp(msgBuffer, "/exit\n") == 0)
        {
            fprintf(stderr, "Client on socket %d has disconnected.\n", clientSocketFd);
            Disconnect(data, clientSocketFd);
            return NULL;
        }
        else
        {
            //Wait for Queue to not be full before pushing message
            while(q->full)
            {
                pthread_cond_wait(q->notFull, q->mutex);
            }

            //Obtain lock, push payload to Queue, unlock, set condition variable
            pthread_mutex_lock(q->mutex);
            fprintf(stderr, "Pushing payload to Queue: %s\n", msgBuffer);
            QUEUE Push(q, msgBuffer);
            pthread_mutex_unlock(q->mutex);
            pthread_cond_signal(q->notEmpty);
        }
    }
}

//The "consumer" -- waits for the Queue to have messages then takes them out and broadcasts to clients
MULTIPLEXER void *RequestHandler(void *data)
{
    Multiplexer *chatData = (Multiplexer *)data;
    QUEUE Queue *q = chatData->Queue;
    int *clientSockets = chatData->clientSockets;

    while(1)
    {
        //Obtain lock and pop message from Queue when not empty
        pthread_mutex_lock(q->mutex);
        while(q->empty)
        {
            pthread_cond_wait(q->notEmpty, q->mutex);
        }
        char* msg = QUEUE Pop(q);
        pthread_mutex_unlock(q->mutex);
        pthread_cond_signal(q->notFull);
        fprintf(stderr, "recieved message: %s\n", msg);
        // fprintf(stderr, "Broadcasting .... \n");
        // for(int i = 0; i < chatData->numClients; i++)
        // {
        //     int socket = clientSockets[i];
        //     if(socket != 0 && write(socket, msg, CONSTS MAX_BUFFER - 1) == -1)
        //         perror("Socket write failed: ");
        // }
    }
}
MULTIPLEXER void *FileHandler(void *data)
{
    Multiplexer *chatData = (Multiplexer *)data;
    QUEUE Queue *q = chatData->Queue;
    int *clientSockets = chatData->clientSockets;
    char path[1024];
    strcpy( path,"/workspace/coe768/cmd/server/" );
    while(1)
    {
        //Obtain lock and pop message from Queue when not empty
        pthread_mutex_lock(q->mutex);
        while(q->empty)
        {
            pthread_cond_wait(q->notEmpty, q->mutex);
        }
        char* msg = QUEUE Pop(q);
        pthread_mutex_unlock(q->mutex);
        pthread_cond_signal(q->notFull);
        char *source = NULL;
        strcat( path, trimmsg(msg));
        fprintf(stderr, "[FILE Handler] opening file: %s\n", path);
        FILE *fp = fopen(path, "r");
        if (fp != NULL) {
        /* Go to the end of the file. */
            if (fseek(fp, 0L, SEEK_END) == 0) {
                /* Get the size of the file. */
                long bufsize = ftell(fp);
                if (bufsize == -1) { /* Error */ }

                /* Allocate our buffer to that size. */
                source = malloc(sizeof(char) * (bufsize + 1));

                /* Go back to the start of the file. */
                if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

                /* Read the entire file into memory. */
                size_t newLen = fread(source, sizeof(char), bufsize, fp);
                if ( ferror( fp ) != 0 ) {
                    fputs("Error reading file", stderr);
                } else {
                    source[newLen++] = '\0'; /* Just to be safe. */

                }
            }
        fclose(fp);
        fprintf(stderr, "file Content  %s\n",source);
        }else{
            fprintf(stderr, "file not found\n");
        }
        fprintf(stderr, "Broadcasting .... \n");
        for(int i = 0; i < chatData->numClients; i++)
        {
            int socket = clientSockets[i];
            if(socket != 0 && write(socket, msg, CONSTS MAX_BUFFER - 1) == -1)
                perror("Socket write failed: ");
        }
    }
}
char *trimmsg(char *str)
{
    size_t len = 0;
    char *frontp = str;
    char *endp = NULL;

    if( str == NULL ) { return NULL; }
    if( str[0] == '\0' ) { return str; }

    len = strlen(str);
    endp = str + len;

    /* Move the front and back pointers to address the first non-whitespace
     * characters from each end.
     */
    while( isspace((unsigned char) *frontp) ) { ++frontp; }
    if( endp != frontp )
    {
        while( isspace((unsigned char) *(--endp)) && endp != frontp ) {}
    }

    if( frontp != str && endp == frontp )
            *str = '\0';
    else if( str + len - 1 != endp )
            *(endp + 1) = '\0';

    /* Shift the string so that it starts at str so that if it's dynamically
     * allocated, we can still free it on the returned pointer.  Note the reuse
     * of endp to mean the front of the string buffer now.
     */
    endp = str;
    if( frontp != str )
    {
            while( *frontp ) { *endp++ = *frontp++; }
            *endp = '\0';
    }

    return str;
}