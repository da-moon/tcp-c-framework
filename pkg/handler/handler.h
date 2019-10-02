#ifndef HANDLER
#define HANDLER
    #include "../multiplexer/multiplexer.h"
    void *RequestHandler(void *data);
    void *FileHandler(void *data);
    char *trimmsg(char *str);
#endif
