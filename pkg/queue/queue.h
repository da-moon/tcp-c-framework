#ifndef QUEUE
#define QUEUE
    #include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include "../shared/consts.h"
    #include "../payload/payload.h"
    // Queue - Queue implementation using a char array.
    typedef struct {
        // char *buffer[CONSTS MAX_BUFFER];
        Payload *entities[CONSTS MAX_BUFFER];
        int head, tail,full, empty;
        // mutex is used for functions to lock on before modifying
        // the array and condition variables for when it's not empty or full.
        pthread_mutex_t *mutex;
        pthread_cond_t *notFull, *notEmpty;
    } Queue;

    // Prototype decl
    Queue* NewQueue(void);
    void Destroy(Queue *q);
    void Push(Queue *q,int origin, char* msg);
    Payload* Pop(Queue *q);

#endif

