#ifndef PAYLOAD
#define PAYLOAD
#include <stddef.h>
#include "../shared/consts.h"
#include <stdio.h>
// For string helper methods
#include <string.h>

int Payload(char *result,char* protocol,char* data);
int ExtractProtocol(char *result,char* data);
int ExtractData(char *result,char* data);
int ExtractLength(int *result,char* data);
#endif