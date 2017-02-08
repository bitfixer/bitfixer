//__sfr __at 0x0400 TEMP1;


void bbcc()
{
    unsigned char* c = (unsigned char*)(0x0400);
  unsigned char _x = 0xC5;
  //TEMP1 = _x;
  *c = _x;
  //__asm
  //ld a, 0x60
  //call 0x00FA
  //__endasm;
  return;
}
