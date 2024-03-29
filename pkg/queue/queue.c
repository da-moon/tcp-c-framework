#include "queue.h"

// NewQueue - Initializes a new Queue
Queue *NewQueue(void) {
  Queue *q = (Queue *)malloc(sizeof(Queue));
  if (q == NULL) {
    perror("Couldn't allocate anymore memory!");
    exit(EXIT_FAILURE);
  }

  q->empty = 1;
  q->full = q->head = q->tail = 0;
  q->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  if (q->mutex == NULL) {
    perror("Couldn't allocate anymore memory!");
    exit(EXIT_FAILURE);
  }
  pthread_mutex_init(q->mutex, NULL);

  q->notFull = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  if (q->notFull == NULL) {
    perror("Couldn't allocate anymore memory!");
    exit(EXIT_FAILURE);
  }
  pthread_cond_init(q->notFull, NULL);

  q->notEmpty = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  if (q->notEmpty == NULL) {
    perror("Couldn't allocate anymore memory!");
    exit(EXIT_FAILURE);
  }
  pthread_cond_init(q->notEmpty, NULL);

  return q;
}
// QueueDestroy - destroys a queue
void DestroyQueue(Queue *q) {
  pthread_mutex_destroy(q->mutex);
  pthread_cond_destroy(q->notFull);
  pthread_cond_destroy(q->notEmpty);
  free(q->mutex);
  free(q->notFull);
  free(q->notEmpty);
  free(q);
}

// Push to end of Queue
void Push(Queue *q, int origin, const Message msg) {

  q->messages[q->tail] = msg;
  q->tail++;
  if (q->tail == MAX_BUFFER)
    q->tail = 0;
  if (q->tail == q->head)
    q->full = 1;
  q->empty = 0;
}

// Pop front of Queue
Message Pop(Queue *q) {
  Message entity = q->messages[q->head];
  q->head++;
  if (q->head == MAX_BUFFER)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;

  return entity;
}
