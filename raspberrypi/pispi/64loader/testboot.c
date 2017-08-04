#define SCREENRAM 0x0400
#define COLORRAM 0xD800

void main()
{
    int i;
    
    for (i = 0; i < 1000; i++)
    {
        *((unsigned char *)(SCREENRAM+i)) = 0;
        *((unsigned char *)(COLORRAM+i)) = 0;
    }
}