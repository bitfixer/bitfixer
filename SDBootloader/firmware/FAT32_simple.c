/*
 *  FAT32_simple.c
 *  SDbootloader
 *
 *  Created by Michael Hill on 9/27/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "FAT32_simple.h"
#include "SD_routines.h"
#include "UART_routines.h"

#define JUMPBOOT 0
#define SIGNATURE 510
#define FIRSTSECTOR 454
#define FATSIZEF32 36
#define ROOTCLUSTER 44
#define SECTORPERCLUSTER 13
#define BYTESPERSECTOR   11
#define ATTRIB           12
#define RESERVEDSECTORCOUNT 14

#define FIRSTCLUSTERHI  20
#define FIRSTCLUSTERLO  26

#define EMPTY           0x00
#define ATTR_LONG_NAME  0x0f
#define DELETED         0xe5

unsigned char readBootSector(void)
{
    unsigned int signature;
    unsigned long firstSector;
    unsigned long bootfilestart;
    unsigned long hiddenSectors;
    unsigned char numberofFATs;
    unsigned long FATsize_F32;
    unsigned char found;
    
    unusedSectors = 0;
    
    // read first block on card
    SD_readSingleBlock(0);
    
    // check if it is boot sector
    if (buffer[JUMPBOOT] != 0xE9 && buffer[JUMPBOOT] != 0xEB)
    {
        firstSector = *(unsigned long *)&buffer[FIRSTSECTOR];
        unusedSectors = firstSector;
        
        SD_readSingleBlock(firstSector);
        
        if (buffer[JUMPBOOT] != 0xE9 && buffer[JUMPBOOT] != 0xEB)
        {
            return 1;
        }
    }
    
    // read data from bpb
    bytesPerSector = *(unsigned int*)&buffer[BYTESPERSECTOR];
    hiddenSectors = *(unsigned long*)&buffer[28];
    reservedSectorCount = *(unsigned int*)&buffer[RESERVEDSECTORCOUNT];
    numberofFATs = *(unsigned char*)&buffer[16];
    FATsize_F32 = *(unsigned long*)&buffer[FATSIZEF32];
    sectorPerCluster = *(unsigned char*)&buffer[SECTORPERCLUSTER];
    rootCluster = *(unsigned char*)&buffer[ROOTCLUSTER];
    
    firstDataSector = hiddenSectors + reservedSectorCount + (numberofFATs * FATsize_F32);
    return 0;
}

/*
unsigned long getNextCluster (unsigned long clusterNumber)
{
    unsigned int FATEntryOffset;
    unsigned long *FATEntryValue;
    unsigned long FATEntrySector;
    unsigned char retry = 0;
    unsigned char bytesPerSector;

    bytesPerSector = 512;

    //get sector number of the cluster entry in the FAT
    FATEntrySector = unusedSectors + reservedSectorCount + ((clusterNumber * 4) / bytesPerSector) ;

    //get the offset address in that sector number
    FATEntryOffset = (unsigned int) ((clusterNumber * 4) % bytesPerSector);

    //read the sector into a buffer
    while(retry <10)
    { 
        if(!SD_readSingleBlock(FATEntrySector)) 
            break; 
        retry++;
    }

    //get the cluster address from the buffer
    FATEntryValue = (unsigned long *) &buffer[FATEntryOffset];

    return ((*FATEntryValue) & 0x0fffffff);
}
*/


unsigned long getFirstSector(unsigned long clusterNumber)
{
  return (((clusterNumber - 2) * sectorPerCluster) + firstDataSector);
}



unsigned long findFirmware()
{
    unsigned long cluster, firstSector;
    unsigned char attrib;
    unsigned int firstClusterHI; // high word of first cluster
    unsigned int firstClusterLO; // low word of first cluster
    unsigned long firstCluster;
    unsigned long theCluster;
    unsigned long sector;
    int i,j;
    cluster = rootCluster;
    
    firstSector = getFirstSector(cluster);
    
    SD_readSingleBlock(firstSector);
    
    for (i = 0; i < bytesPerSector; i += 32)
    {
        if (buffer[i] == EMPTY)
        {
            return 0;
        }
        
        attrib = buffer[ATTRIB+i];
        if ((buffer[i] != DELETED) && attrib != ATTR_LONG_NAME)
        {
            // check for firmware file
            if ((buffer[i] == 'F') &&
                (buffer[i+1] == 'I') &&
                (buffer[i+2] == 'R') &&
                (buffer[i+3] == 'M') &&
                (buffer[i+8] == 'B') && 
                (buffer[i+9] == 'I') &&
                (buffer[i+10] == 'N'))
            {
                firstClusterLO = *(unsigned int*)&buffer[i+FIRSTCLUSTERLO];
                
                // assume firstClusterHI is 0
                firstCluster = firstClusterLO;
                
                return firstCluster;
            }
        }
    }
    
    return 0;
    
}