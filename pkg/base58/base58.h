
#ifndef BASE58
#define BASE58
    #include <stdio.h>
    #include <string.h>
    #include <openssl/bn.h>
    static char const *dictionary="123456789"
                            "abcdefghijkmnopqrstuvwxyz"
                            "ABCDEFGHJKLMNPQRSTUVWXYZ" ;

    unsigned char *  Encode(unsigned char *in, int inLen, int *outLen);
    unsigned char * Decode(unsigned char *in, int inLen);
    unsigned char * getIndexes();
    unsigned char divmod58(unsigned char *in, int inLen, int i);
    unsigned char divmod256(unsigned char *in, int inLen, int i);
#endif
