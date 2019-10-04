/// @source       testQueue.c
/// @description  Tests of the Queue data structure.

// -----------------------------------------------------------------------------

// common configuration options & declarations (always include first)
#include "config.h"

// C language includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if PLATFORM(Windows)
#define strdup _strdup
#endif

// application includes
#include "util/Queue.h" /* queue_xxx functions */

// prototypes
static bool cmpString(void*, void*);
static void testAddTail(ushort, Queue*, void*);
static Queue* testCreate(ushort, ushort);
static void testDestroy(ushort, Queue*);
static void testEmpty(ushort, Queue*);
// static void testNotDestroy(ushort, Queue*);
static void testNotEmpty(ushort, Queue*);
static void testNotRemoveHead(ushort, Queue*);
static void testRemoveIf(ushort, Queue*, char*);
static void testRemoveHead(ushort, Queue*);
static void testSize(ushort, Queue*, ushort);

static void testSet(Queue*);

// -----------------------------------------------------------------------------

// the main program

int main(void)
{
   Queue* q;

   q = testCreate    (/*test*/0, 0);
   testSet(q);

   q = testCreate    (/*test*/0, 1);
   testSet(q);

   q = testCreate    (/*test*/0, 2);
   testSet(q);

   q = testCreate    (/*test*/0, 3);
   testSet(q);

   q = testCreate    (/*test*/0, 4);
   testSet(q);

   q = testCreate    (/*test*/0, 5);
   testAddTail       (/*test*/1, q, strdup("11111")); // perm
   testAddTail       (/*test*/2, q, strdup("22222")); // perm
   testAddTail       (/*test*/3, q, strdup("33333")); // perm
   testAddTail       (/*test*/4, q, strdup("44444")); // perm
   testAddTail       (/*test*/5, q, strdup("55555")); // perm
   testAddTail       (/*test*/6, q, strdup("66666"));
   testAddTail       (/*test*/7, q, strdup("77777"));
   testAddTail       (/*test*/8, q, strdup("88888"));
   testAddTail       (/*test*/9, q, strdup("99999"));
   testAddTail       (/*test*/10, q, strdup("00000"));

   testSize          (/*test*/11, q, 10);

   testRemoveIf      (/*test*/12, q, "11111"); // first perm
   testRemoveIf      (/*test*/13, q, "55555"); // last perm

   testRemoveIf      (/*test*/14, q, "66666"); // first not perm
   testRemoveIf      (/*test*/15, q, "00000"); // last not perm

   testRemoveIf      (/*test*/16, q, "33333"); // midle perm
   testRemoveIf      (/*test*/17, q, "88888"); // middle not perm

   testRemoveIf      (/*test*/18, q, "22222"); // next to last perm
   testRemoveIf      (/*test*/19, q, "44444"); // last perm

   testRemoveIf      (/*test*/20, q, "77777"); // next to last not perm
   testRemoveIf      (/*test*/21, q, "99999"); // last not perm

   testSize          (/*test*/22, q, 0);

   return 0;
}

// -----------------------------------------------------------------------------

static bool cmpString(void* arg1, void* arg2)
{
   return ((strcmp((char*)arg1, (char*)arg2)) == 0);
}

// -----------------------------------------------------------------------------

static void testAddTail(ushort nTest, Queue* q, void* entry)
{
   printf("* ---------------------------------------------------------\n\n");
   printf("* test %d\n\n", nTest);
   printf("* queue_addTail\n");
   queue_addTail(q, entry);
   printf("* probably successful, entry [%s] added\n", (char*)entry);
   printf("*\n");
}

// -----------------------------------------------------------------------------

static Queue* testCreate(ushort nTest, ushort nEntries)
{
   Queue* q;
   printf("* ---------------------------------------------------------\n\n");

   printf("* test %d\n\n", nTest);

   printf("* queue_create(%d)\n", nEntries);
   q = queue_create(nEntries);

   if (q)
      printf("* successful, queue was created\n");
   else
   {
      printf("* failure, queue was not created\n");
      exit(1);
   }

   printf("*\n");

   return q;
}

// -----------------------------------------------------------------------------

static void testDestroy(ushort nTest, Queue* q)
{
   printf("* ---------------------------------------------------------\n\n");

   printf("* test %d\n\n", nTest);

   printf("* queue_destroy\n");

   queue_destroy(q);

   printf("* probably successful, queue destroyed\n");

   printf("*\n");
}

// -----------------------------------------------------------------------------

static void testEmpty(ushort nTest, Queue* q)
{
   printf("* ---------------------------------------------------------\n\n");

   printf("* test %d\n\n", nTest);

   printf("* queue_isEmpty\n");

   if (queue_isEmpty(q))
      printf("* successful, queue is empty\n");
   else
   {
      printf("* failure, queue is not empty\n");
      exit(1);
   }

   printf("*\n");
}

// -----------------------------------------------------------------------------

#if 0
static void testNotDestroy(ushort nTest, Queue* q)
{
   printf("* ---------------------------------------------------------\n\n");

   printf("* test %d\n\n", nTest);

   printf("* not queue_destroy\n");

   queue_destroy(q);

   printf("* failure, destroyed\n");

   printf("*\n");
}
#endif

// -----------------------------------------------------------------------------

static void testNotEmpty(ushort nTest, Queue* q)
{
   printf("* ---------------------------------------------------------\n\n");

   printf("* test %d\n\n", nTest);

   printf("* not queue_isEmpty\n");

   if (!queue_isEmpty(q))
      printf("* successful, queue is not empty\n");
   else
   {
      printf("* failure, queue is empty\n");
      exit(1);
   }

   printf("*\n");
}

// -----------------------------------------------------------------------------

static void testNotRemoveHead(ushort nTest, Queue* q)
{
   void* entry;

   printf("* ---------------------------------------------------------\n\n");
   printf("* test %d\n\n", nTest);

   printf("* not queue_removeHead\n");

   if (!(entry = queue_removeHead(q)))
      printf("* successful, entry was not removed\n");
   else
   {
      printf("* failure, entry was %s removed\n", (char*)entry);
      exit(1);
   }

   printf("*\n");
}

// -----------------------------------------------------------------------------

static void testRemoveIf(ushort nTest, Queue* q, char* arg)
{
   char* entry;

   printf("* ---------------------------------------------------------\n\n");
   printf("* test %d\n\n", nTest);

   printf("* queue_removeIf\n");

   if ((entry = queue_remove(q, cmpString, arg)))
   {
      if (!strcmp(entry, arg))
         printf("* successful, entry %s was removed\n", entry);
      else
         printf("* failure, entry %s != arg %s\n", entry, arg);
      free(entry);
   }
   else
   {
      printf("* failure, entry was not removed\n");
      exit(1);
   }

   printf("*\n");
}

// -----------------------------------------------------------------------------

static void testRemoveHead(ushort nTest, Queue* q)
{
   char* entry;

   printf("* ---------------------------------------------------------\n\n");
   printf("* test %d\n\n", nTest);

   printf("* queue_removeHead\n");

   if ((entry = queue_removeHead(q)))
   {
      printf("* successful, entry %s was removed\n", entry);
      free(entry);
   }
   else
   {
      printf("* failure, entry was not removed\n");
      exit(1);
   }

   printf("*\n");
}

// -----------------------------------------------------------------------------

static void testSize(ushort nTest, Queue* q, ushort size)
{
   ushort n;

   printf("* ---------------------------------------------------------\n\n");
   printf("* test %d\n\n", nTest);

   printf("* queue size\n");

   if ((n=queue_size(q)) == size)
      printf("* successful, size is %d\n", size);
   else
   {
      printf("* failure, size is %d\n", n);
      exit(1);
   }

   printf("\n");
}

// -----------------------------------------------------------------------------

static void testSet(Queue* q)
{
   testEmpty         (/*test*/1, q);
   testSize          (/*test*/2, q, 0);
   testNotRemoveHead (/*test*/3, q);

   testAddTail       (/*test*/4, q, strdup("11111"));
   testNotEmpty      (/*test*/5, q);
   testSize          (/*test*/6, q, 1);

   testRemoveHead    (/*test*/7, q);
   testEmpty         (/*test*/8, q);
   testSize          (/*test*/9, q, 0);
   testNotRemoveHead (/*test*/10, q);

   testAddTail       (/*test*/11, q, strdup("22222"));
   testAddTail       (/*test*/12, q, strdup("33333"));
   testNotEmpty      (/*test*/13, q);
   testSize          (/*test*/14, q, 2);
   testRemoveHead    (/*test*/15, q);
   testNotEmpty      (/*test*/16, q);
   testSize          (/*test*/17, q, 1);

   testAddTail       (/*test*/18, q, strdup("44444"));
   testAddTail       (/*test*/19, q, strdup("55555"));
   testNotEmpty      (/*test*/20, q);
   testSize          (/*test*/21, q, 3);

   testRemoveHead    (/*test*/22, q);
   testSize          (/*test*/23, q, 2);
   testNotEmpty      (/*test*/24, q);

   testRemoveHead    (/*test*/25, q);
   testSize          (/*test*/26, q, 1);
   testNotEmpty      (/*test*/27, q);

   testRemoveHead    (/*test*/28, q);
   testSize          (/*test*/29, q, 0);
   testEmpty         (/*test*/30, q);

   testDestroy       (/*test*/31, q);

   // testNotDestroy    (/*test*/28, q);
}

// -----------------------------------------------------------------------------
// the end
