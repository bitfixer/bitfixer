/*
 * upserial.c
 * userport serial adapter
 *
 * Created: 3/15/2016
 *  Author: Michael Hill
 */ 

#include "UART_routines.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//#define PA0 0x01
//#define PA1 0x02

void spi_init()
{
    SPCR = (1<<SPE); // slave, fosc/4 (2MHz)
    SPSR = 0x00;
}

unsigned char spi_receive(void)
{
    unsigned char data;
    // Wait for reception complete
    
    SPDR = 0xff;
    while(!(SPSR & (1<<SPIF)));
    data = SPDR;
    
    // Return data register
    return data;
}

unsigned char SPI_transmit(unsigned char data)
{
    // Start transmission
    SPDR = data;
    
    // Wait for transmission complete
    while(!(SPSR & (1<<SPIF)));
    data = SPDR;
    
    return(data);
}

void output_byte(unsigned char byte)
{
    PORTC = byte;
}

unsigned char input_byte()
{
    return PINC;
}

void set_data_output()
{
    DDRC = 0xFF;
}

void set_data_input()
{
    DDRC = 0x00;
}

unsigned char read_byte()
{
    unsigned char byte = PINC;
    return byte;
}

void init()
{
    DDRA = (1<<PA2);
    PORTA = (1<<PA2);
    
    DDRB = 0x41;
    
    DDRD = 0x80;
    PORTD = 0x80;
    // PA0 is PA2 from user port
    // PA1 is /PC2 handshake line
    // MISO out
    
    //set_data_output();
    set_data_input();
    uart0_init(25); // 38.4kbaud
    spi_init();
}

void lower_flag()
{
    PORTA = 0x00;
}

void raise_flag()
{
    PORTA = (1<<PA2);
}

unsigned char get_command()
{
    // wait for PA0 low
    unsigned char val;
    unsigned char byte;
    do {
        val = PINA & (1<<PA0);
    } while (val != 0);
    
    // read byte and lower flag
    byte = input_byte();
    lower_flag();
    
    // wait for PA0 high
    do {
        val = PINA & (1<<PA0);
    } while (val == 0);
    
    raise_flag();
    return byte;
}

void relay_command(unsigned char cmd)
{
    // set byte to send
    SPDR = cmd;
    
    // signal to read
    PORTB = 0x00;
    while(!(SPSR & (1<<SPIF)));
    
}

/*
ISR(SPI_STC_vect)
{
    PORTB = 0x01;
    SPDR = 0xAA;
}
*/
 
int main(void)
{
    unsigned char val;
    unsigned char cmd;
    unsigned char buffer[2048];
    PORTB = 0x01;
    
    memset(buffer, 0, 2048);
    init();
    
    // wait for command byte
    while (1)
    {
        
        cmd = get_command();
        
        // switch to output mode
        set_data_output();
        
        // wait for PA0 low
        do {
            val = PINA & (1<<PA0);
        } while (val != 0);
        
        // master has signaled for transfer
        // prepare and lower handshake line
        // relay command to pi
        relay_command(cmd);
        
        PORTD = 0x00;
        // receive buffer from pi
        for (int i = 0; i < 1024; i++)
        {
            buffer[i] = spi_receive();
        }
        
        PORTD = 0x80;
        // lower flag to indicate ready
        lower_flag();
        
        // do the transfer
        // clock on PA1 pin, handshake line from user port
        for (int i = 0; i < 1024; i++)
        {
            output_byte(buffer[i]);
            
            // wait for handshake pulse to end
            do {
                val = PINA & (1<<PA1);
            } while (val == 0);
            
            // wait for handshake pulse to start
            do {
                val = PINA & (1<<PA1);
            } while (val != 0);
        }
        
        // raise flag, done
        raise_flag();
        PORTB = 0x01;
        
        // wait for PA0 high, master is done
        do {
            val = PINA & (1<<PA0);
        } while (val == 0);
    
        set_data_input();
    }
}