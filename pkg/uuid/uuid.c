#include "uuid.h"

#include <stdio.h>
#include <stdint.h>
static uint64_t seed[2];

int InitializeUUID(void) {
  int res;
  FILE *fp = fopen("/dev/urandom", "rb");
  if (!fp) {
    return -1;
  }
  res = fread(seed, 1, sizeof(seed), fp);
  fclose(fp);
  if ( res != sizeof(seed) ) {
    return -1;
  }
  return 0;
}

void GenerateUUID(char *dst) {
  static const char *template = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
  static const char *chars = "0123456789abcdef";
  union { unsigned char b[16]; uint64_t word[2]; } s;
  const char *p;
  int i, n;
  // get random
  s.word[0] = xor_shift(seed);
  s.word[1] = xor_shift(seed);
  // build string
  p = template;
  i = 0;
  while (*p) {
    n = s.b[i >> 1];
    n = (i & 1) ? (n >> 4) : (n & 0xf);
    switch (*p) {
      case 'x'  : *dst = chars[n];              i++;  break;
      case 'y'  : *dst = chars[(n & 0x3) + 8];  i++;  break;
      default   : *dst = *p;
    }
    dst++, p++;
  }
  *dst = '\0';
}