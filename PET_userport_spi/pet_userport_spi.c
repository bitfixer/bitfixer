/*
 * upserial.c
 * PET user port spi adapter
 *
 * Created: 3/15/2016
 *  Author: Michael Hill
 */ 

#include "UART_routines.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define PET 1

#define PET_CA1_INPORT  PINA
#define PET_CA1_OUTPORT PORTA
#define PET_CA1_REG     DDRA
#define PET_CA1_PIN     PA2

#define PET_CB2_INPORT  PINA
#define PET_CB2_OUTPORT PORTA
#define PET_CB2_REG     DDRA
#define PET_CB2_PIN     PA0

#define DATA_REG        DDRC
#define DATA_INPORT     PINC
#define DATA_OUTPORT    PORTC

#define SPI_READY_REG       DDRB
#define SPI_READY_OUTPORT   PORTB
#define SPI_READY_PIN       PB0

#define BUFFER_SIZE         2048

#if PET80COL == 1
    #define SCREEN_BYTES_SIZE   2000
#else
    #define SCREEN_BYTES_SIZE   1000
#endif

#if PET80COL == 1
    #define GET_SCREEN_CMD      0x10
#else
    #define GET_SCREEN_CMD      0x11
#endif

#ifdef PET
#define HANDSHAKE_OUTPUT_INPORT     PET_CA1_INPORT
#define HANDSHAKE_OUTPUT_OUTPORT    PET_CA1_OUTPORT
#define HANDSHAKE_OUTPUT_REG        PET_CA1_REG
#define HANDSHAKE_OUTPUT_PIN        PET_CA1_PIN

#define HANDSHAKE_INPUT_INPORT      PET_CB2_INPORT
#define HANDSHAKE_INPUT_OUTPORT     PET_CB2_OUTPORT
#define HANDSHAKE_INPUT_REG         PET_CB2_REG
#define HANDSHAKE_INPUT_PIN         PET_CB2_PIN
#endif

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
    DATA_OUTPORT = byte;
}

unsigned char input_byte()
{
    return DATA_INPORT;
}

void set_data_output()
{
    DATA_REG = 0xFF;
}

void set_data_input()
{
    DATA_REG = 0x00;
}

unsigned char read_byte()
{
    unsigned char byte = DATA_INPORT;
    return byte;
}

void init()
{
    HANDSHAKE_OUTPUT_REG = (1<<HANDSHAKE_OUTPUT_PIN);
    HANDSHAKE_OUTPUT_OUTPORT = 0x00;
    
    DATA_REG = 0x00; // set data register to input
    
    SPI_READY_REG = 0x41;
    
    DDRD = 0x80;
    PORTD = 0x80;
    // PA0 is PA2 from user port
    // PA1 is /PC2 handshake line
    // MISO out
    
    set_data_input();
    uart0_init(25); // 38.4kbaud
    spi_init();
}

void lower_flag()
{
    HANDSHAKE_OUTPUT_OUTPORT = 0x00;
}

void raise_flag()
{
    HANDSHAKE_OUTPUT_OUTPORT = (1<<HANDSHAKE_OUTPUT_PIN);
}

unsigned char get_command()
{
    // wait for PA0 low
    unsigned char val;
    unsigned char byte;
    do {
        //val = PINA & (1<<PA0);
        val = HANDSHAKE_INPUT_INPORT & (1<<HANDSHAKE_INPUT_PIN);
    } while (val != 0);
    
    // read byte and lower flag
    byte = input_byte();
    lower_flag();
    
    // wait for PA0 high
    do {
        //val = PINA & (1<<PA0);
        val = HANDSHAKE_INPUT_INPORT & (1<<HANDSHAKE_INPUT_PIN);
    } while (val == 0);
    
    raise_flag();
    return byte;
}

void relay_command(unsigned char cmd)
{
    // set byte to send
    SPDR = cmd;
    
    // signal to read
    SPI_READY_OUTPORT = 0x00;
    while(!(SPSR & (1<<SPIF)));
    
}

int main(void)
{
    unsigned char val;
    unsigned char cmd;
    unsigned char buffer[BUFFER_SIZE];
    // wait for PET to request a frame
    SPI_READY_OUTPORT = (1<<SPI_READY_PIN);
    init();
    
    while (1)
    {
        // send simulated command
        cmd = GET_SCREEN_CMD;
        relay_command(cmd);
        
        for (int i = 0; i < SCREEN_BYTES_SIZE; i++)
        {
            buffer[i] = spi_receive();
        }
        
        SPI_READY_OUTPORT = (1<<SPI_READY_PIN);
        _delay_ms(1);
        
        set_data_output();
        // raise handshake line
        HANDSHAKE_OUTPUT_OUTPORT = (1<<HANDSHAKE_OUTPUT_PIN);
        
        // wait for handshake input to go low
        val = HANDSHAKE_INPUT_INPORT & (1<<HANDSHAKE_INPUT_PIN);
        while (val != 0x00)
        {
            val = HANDSHAKE_INPUT_INPORT & (1<<HANDSHAKE_INPUT_PIN);
        }
        
        HANDSHAKE_OUTPUT_OUTPORT = 0x00;
        
        for (int i = 0; i < SCREEN_BYTES_SIZE; i++)
        {
            DATA_OUTPORT = buffer[i];
            val = HANDSHAKE_INPUT_INPORT & (1<<HANDSHAKE_INPUT_PIN);
            while (val == 0x00)
            {
                val = HANDSHAKE_INPUT_INPORT & (1<<HANDSHAKE_INPUT_PIN);
            }
            
            i++;
            DATA_OUTPORT = buffer[i];
            
            if (i < SCREEN_BYTES_SIZE-1)
            {
                val = HANDSHAKE_INPUT_INPORT & (1<<HANDSHAKE_INPUT_PIN);
                while (val != 0x00)
                {
                    val = HANDSHAKE_INPUT_INPORT & (1<<HANDSHAKE_INPUT_PIN);
                }
            }
        }
        
        _delay_ms(40);
    }
}

/*
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
        
        PORTB = 0x01;
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
        output_byte(0x00);
        set_data_input();
        
        // raise flag, done
        raise_flag();
        
        // wait for PA0 high, master is done
        do {
            val = PINA & (1<<PA0);
        } while (val == 0);
    }
}
*/
