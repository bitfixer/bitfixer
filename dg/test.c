main()
{
    z80_outp(0x01, 'A');
    z80_outp(0x01, 'B');
    
    unsigned char a = 0;
    while (1)
    {
        a++;
    }
}