//#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
//#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include "UART_routines.h"
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "SPI_routines.h"
#include "sd_routines.h"
#include "FAT32_simple.h"

#define SPI_CTL DDRB
#define SPI_PORT PORTB

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

void flash_firmware(unsigned long startCluster)
{
    unsigned int FATEntryOffset;
    unsigned long *FATEntryValue;
    unsigned long FATEntrySector;
    unsigned long Cluster;

    unsigned long sector;
    int current_block,j,k;
    
    Cluster = startCluster;
    
    // get first sector of the FAT
    // can assume this is in the first FAT sector since this is the first file
    FATEntrySector = unusedSectors + reservedSectorCount;
    
    current_block = 0;
    
    // loop through blocks of the file
    while (current_block < 28)
    {
        // get first sector of the cluster
        sector = getFirstSector(Cluster);
        
        for (j = 0; j < sectorPerCluster; j++)
        {
            transmitByte('0'+j);
            SD_readSingleBlock(sector+j);
            
            // write this block to program memory
            for (k = 0; k < bytesPerSector; k += SPM_PAGESIZE)
            {
                // flash each page of memory
                boot_program_page((bytesPerSector*current_block)+k, &buffer[k]);
            }
            
            // step to the next block
            current_block++;
            
            // stop if this is the last block
            if (current_block >= 28)
            {
                break;
            }
        }
        
        // get the next Cluster
        // read the FAT entry sector
        SD_readSingleBlock(FATEntrySector);
        // get FAT entry for this cluster
        FATEntryOffset = Cluster * 4;
        // get FAT entry
        FATEntryValue = (unsigned long *)&buffer[FATEntryOffset];
        // mask the FAT entry to get the next cluster
        Cluster = (*FATEntryValue) & 0x0fffffff;
    }
}

int main(void)
{
    int pp = 0;
    unsigned char error;
    unsigned long bootfileFirstCluster;
    unsigned char thisversion;
    
    SPI_CTL = 0x2C;
    SPI_PORT = 0xFF;
    
    spi_init();
    // initialize uart
    uart0_init();
    
    MCUCR = 0x00;
    
    int i;
    int j;
    cardType = 0;
    
    // try to initialize SD card
    for (i = 0; i < 5; i++)
    {
        error = SD_init();
        if (!error) break;
    }
    
    if(error)
    {
        // if SD card fails to initialize, assume no card present
        // jump to main application
        app_start(); 
    }
    
    // read information from the boot sector
    error = readBootSector();
    if (error == 1)
    {
        // jump to main application if there is a problem with the boot sector
        app_start();
    }
    
    // find the firmware file
    bootfileFirstCluster = findFirmware();
        
    // if we have found a firmware file,
    if (bootfileFirstCluster > 0)
    {
        // found firmware, now reprogram the chip
        flash_firmware(bootfileFirstCluster);
    }
    
    // jump to application
    app_start();
}