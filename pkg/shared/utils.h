#ifndef UTILS
#define UTILS
#include "consts.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char *Trim(char *str);
char *magic_reallocating_fgets(char **bufp, size_t *sizep, FILE *fp);
void print_array_in_hex(unsigned char *array);
#endif
