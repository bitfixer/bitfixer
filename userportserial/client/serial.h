#ifndef __SERIAL_H__
#define __SERIAL_H__

// memory addresses for I/O
#define PORTB           0xDD01
#define DDRB            0xDD03
#define PORTA           0xDD00
#define DDRA            0xDD02
#define ICR             0xDD0D

// flag bit in ICR register
#define FLAG            0x10

// PA2 handshaking line in PORTA, output
#define PA2             0x04

unsigned char test();

void serial_init();
unsigned char serial_byte_ready();
unsigned char serial_read_byte();
void serial_done_reading();

#endif