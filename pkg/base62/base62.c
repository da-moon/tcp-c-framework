#include "base62.h"

BASE62 char * Encode(uint64_t num)
{
    /* 64 bits is up to 11 digits in base 62 */
    static char enc[15] ;
    int         i, len ;
    uint64_t    rem ;
    char        c ;

    /* Zero out output buffer */
    memset(enc, 0, sizeof(enc));
    if (num==0) {
        enc[0]='0';
        return enc;
    }
    i=0 ;
    while (num>0) {
        rem = num % 62 ;
        enc[i] = BASE62 dictionary[rem];
        i++ ;
        num /= 62 ;
    }
    /* Reverse string */
    len = (int)strlen(enc);
    for (i=0 ; i<len/2 ; i++) {
        c = enc[i] ;
        enc[i] = enc[len-i-1];
        enc[len-i-1] = c ;
    }
    return enc ;
}
BASE62 uint64_t Decode(char * s)
{
    uint64_t    num ;
    uint64_t    p ;
    char     *  pos ;
    int         i ;

    if (!s)
        return 0 ;

    p=1;
    num=0;
    for (i=strlen(s)-1 ; i>=0 ; i--) {
        pos = strchr(BASE62 dictionary, (int)s[i]);
        if (!pos) {
            return 0 ;
        }
        num += (uint64_t)(pos-BASE62 dictionary) * p ;
        p *= 62 ;
    }
    return num ;
}

