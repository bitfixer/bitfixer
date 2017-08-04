#define PA2 0x04
#define CIA2_DDRA *((unsigned char *)0xDD02)

void test(void)
{
    // set port register a to output for bit 2
    //unsigned char reg = CIA2.ddra;
    unsigned char reg = CIA2_DDRA;
    // raise bit 2
    reg = reg | PA2;
    CIA2_DDRA = reg;
    
    *((unsigned char *)0x04A0) = 0;
    *((unsigned char *)0xD8A0) = 0;
}