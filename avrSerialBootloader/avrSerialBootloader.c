#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/delay.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/boot.h>
#include "UART_routines.h"
#include "xmodem.h"

#define F_OSC 8000000		           /* oscillator-frequency in Hz */
#define UART_BAUD_RATE 38400
//#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_OSC)/((UART_BAUD_RATE)*16l)-1)
#define MYUBRR F_CPU/16/UART_BAUD_RATE-1

#define SOH 0x01
#define ACK 0x06
#define EOT 0x04
#define NAK 21

#define CHAR 0
#define INT  1
#define LONG 2

//#define PJMP 0x01
#define PJMP 0x20

#define PRGPIN  PINC
#define PRGMASK 0x20

void (*app_start)(void) = 0x0000;

void boot_program_page (uint32_t page, uint8_t *buf)
{
    uint16_t i;
    uint8_t sreg;

    // Disable interrupts.

    sreg = SREG;
    cli();

    eeprom_busy_wait ();

    boot_page_erase (page);
    boot_spm_busy_wait ();      // Wait until the memory is erased.


    for (i=0; i<SPM_PAGESIZE; i+=2)
    {
        // Set up little-endian word.

        uint16_t w = *buf++;
        w += (*buf++) << 8;
    
        boot_page_fill (page + i, w);
    }

    boot_page_write (page);     // Store buffer in flash page.
    boot_spm_busy_wait();       // Wait until the memory is written.

    // Reenable RWW-section again. We need this if we want to jump back
    // to the application after bootloading.

    boot_rww_enable ();

    // Re-enable interrupts (if they were ever enabled).

    SREG = sreg;
}


int main(void) {

    unsigned char buff[256];
    unsigned char data, cksum[2];
    unsigned char jumper;
    int i;
    int packetnumber;
    int complete;
    int curr_buff_pos;
    int curr_page_pos;
    
    int crc_recv;
    int crc_calc;
    
    // test
    unsigned char blah;
        
    // check programming jumper
    DDRC = 0x00;
    PORTC = PRGMASK;
    jumper = PRGPIN;
    
    uart0_init(MYUBRR); // init USART
    
    // if jumper is ON, just go to the program
    if ((jumper & PJMP) != 0x00)
    {
        // jumper is on
        transmitString("Jumping\r\n");
        app_start();
    }
    
    // ask user to start XMODEM transfer
    transmitString("Programming Mode\r\n");
    transmitString("Start XMODEM transfer, then replace jumper.\r\n");
    
    while ((jumper & PJMP) == 0x00)
    {
        // jumper is still off
        // check again
        jumper = PRGPIN;
    }
    
    // add a delay to let things settle down
    for (i = 0; i < 40; i++)
    {
        _delay_loop_2(65535);
    }
    
    complete = 0;
    
    transmitByte('C');
    packetnumber = 0;
    curr_buff_pos = 0;
    curr_page_pos = 0;
    //while(complete == 0 && packetnumber < 10)
    while(complete == 0)
    {
        // get header and packet number
        data = receiveByte();
        
        if (data == EOT)
        {
            transmitByte(ACK);
            complete = 1;
        } 
        else
        {
            data = receiveByte();
            data = receiveByte();
        
            // receive data
            for (i = 0; i < 128; i++)
            {
                data = receiveByte();
                buff[curr_buff_pos] = data;
                curr_buff_pos++;
            }
            
            // get 2 byte checksum
            cksum[0] = receiveByte();
            cksum[1] = receiveByte();
            // calculate checksum
            
            crc_recv = cksum[0];
            crc_recv = crc_recv << 8;
            crc_recv = crc_recv | cksum[1];
            
            // get crc
            crc_calc = calcrc(&buff[curr_buff_pos-128], 128);
            
            if (crc_recv == crc_calc)
            {
        
                if (curr_buff_pos >= SPM_PAGESIZE)
                {
                    // write a page of memory
                    boot_program_page(curr_page_pos, &buff[0]);
                    // increment page
                    curr_page_pos += SPM_PAGESIZE;
                    
                    if (SPM_PAGESIZE < 128)
                    {
                        boot_program_page(curr_page_pos, &buff[SPM_PAGESIZE]);
                        curr_page_pos += SPM_PAGESIZE;
                    }
                    
                    curr_buff_pos = 0;
                }
            
                // send ACK
                transmitByte(ACK);
                packetnumber++;
            }
            else
            {
                // error in transmission, send NAK
                // decrement buffer position
                curr_buff_pos -= 128;
                transmitByte(NAK);
            }

        }
    }
    
    for (i = 0; i < 20; i++)
    {
        _delay_loop_2(65535);
    }
    
    transmitString("Programming complete.\r\n");
    
    //while(1) {}
    app_start();
}
