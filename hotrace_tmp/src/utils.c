#include "../include/utils.h"

void *ft_memcpy(void *dst, const void *src, size_t len)
{
    size_t i;
    unsigned long int *longword_ptr;
    unsigned long int longword, himagic, lomagic;

    /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
     the "holes."  Note that there is a hole just to the left of
     each byte, with an extra at the end: ;

     bits: ;  01111110 11111110 11111110 11111111
     bytes: ; AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD

     The 1-bits make sure that carries propagate to the next 0-bit.
     The 0-bits provide holes for carries to fall into.  */
    himagic = 0x80808080L; // 10000000100000001000000010000000
    lomagic = 0x01010101L; // 1000000010000000100000001
    /* 64-bit version of the magic.  */
    /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
    if (sizeof (longword) > 4) {
        himagic = ((himagic << 16) << 16) | himagic;
        lomagic = ((lomagic << 16) << 16) | lomagic;
    }
    /* if aligned copy 1 word at a time */
    if ((uintptr_t)dst % sizeof(long) == 0 && (uintptr_t)src % sizeof(long) == 0 && len % sizeof(long) == 0) {
        longword_ptr = (unsigned long int *)dst;
        const unsigned long int *s = (unsigned long int *)src;
        for (i = 0; i < len / sizeof(unsigned long int); i++) {
            longword_ptr[i] = s[i];
            longword = longword_ptr[i];
            if (((longword - lomagic) & ~longword & himagic) != 0) {
                return dst;
            }
        }
    } else {
        char *d = dst;
        const char *s = src;
        for (i = 0; i < len; i++) {
            d[i] = s[i];
            if (d[i] == 0) {
                return dst;
            }
        }
    }
    return dst;
}