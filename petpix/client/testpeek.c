#include <peekpoke.h>
#include <pet.h>

#define KEYMATRIX 0x0203
#define VMEM_START      0x8000

int main(void)
{
    unsigned char key = 0;
    unsigned char *p = (unsigned char *)VMEM_START;
    while (1)
    {
        key = PEEK(KEYMATRIX);
        *p = key;
    }
    
    return 0;
}