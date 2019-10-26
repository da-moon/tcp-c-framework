#include "utils.h"
char *Trim(char *str) {
  size_t len = 0;
  char *frontp = str;
  char *endp = NULL;

  if (str == NULL) {
    return NULL;
  }
  if (str[0] == '\0') {
    return str;
  }

  len = strlen(str);
  endp = str + len;

  /* Move the front and back pointers to address the first non-whitespace
   * characters from each end.
   */
  while (isspace((unsigned char)*frontp)) {
    ++frontp;
  }
  if (endp != frontp) {
    while (isspace((unsigned char)*(--endp)) && endp != frontp) {
    }
  }

  if (frontp != str && endp == frontp)
    *str = '\0';
  else if (str + len - 1 != endp)
    *(endp + 1) = '\0';

  /* Shift the string so that it starts at str so that if it's dynamically
   * allocated, we can still free it on the returned pointer.  Note the reuse
   * of endp to mean the front of the string buffer now.
   */
  endp = str;
  if (frontp != str) {
    while (*frontp) {
      *endp++ = *frontp++;
    }
    *endp = '\0';
  }

  return str;
}

char *magic_reallocating_fgets(char **bufp, size_t *sizep, FILE *fp) {
  size_t len;
  if (fgets(*bufp, *sizep, fp) == NULL)
    return NULL;
  len = strlen(*bufp);
  while (strchr(*bufp, '\n') == NULL) {
    *sizep += 100;
    *bufp = realloc(*bufp, *sizep);
    if (fgets(*bufp + len, *sizep - len, fp) == NULL)
      return *bufp;
    len += strlen(*bufp + len);
  }
  fprintf(stderr, "[DEBUG] size reallocation was successful.\n");
  return *bufp;
}
void print_array_in_hex(unsigned char *array) {
  int len = ntohl(*(int *)(array + 2));
  for (int i = 0; i < len + PROTOCOL_HEADER_LEN && i < 64; i++) {
    printf("%2x ", array[i]);
    if (i % 16 == 15)
      puts("");
  }
  puts("");
}
