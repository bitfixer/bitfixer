#include "Utils.h"
#include <avr/pgmspace.h>

void pgm_memcpy(unsigned char *dest, const unsigned char *src, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        *dest++ = pgm_read_byte(&(*src++));
    }
}
