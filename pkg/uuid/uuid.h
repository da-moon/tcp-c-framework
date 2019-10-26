#ifndef UUID
#define UUID
#include "../shared/consts.h"
#include "../shared/utils.h"
// InitializeUUID - This method needs to be called 
// before calling Generate UUID . It would set a random
// seed to generate future UUIDs from.
int  InitializeUUID(void);
// GenerateUUID - Generates a UUID V4 string 
void GenerateUUID(char *dst);
#endif
