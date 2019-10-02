
#ifndef BASE62
#define BASE62
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdint.h>
    static char const *dictionary="0123456789"
                            "abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ;
    char * Encode(uint64_t num);
    uint64_t Decode(char * s);

#endif
