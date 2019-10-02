#ifndef PAYLOAD
#define PAYLOAD
#include <stddef.h>
#include "../shared/consts.h"

typedef struct {
    int version;
	const char *name;
	int length;
    char *data;
} Payload;

// returns hash code or error (which is < 0)
Payload* New(int version,char *name,char *data);

// returns hash code or error (which is < 0)
char* Data();

// returns length of payload or error (which is < 0)
int Length();


#endif
