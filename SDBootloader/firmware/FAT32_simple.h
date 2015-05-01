/*
 *  FAT32_simple.h
 *  SDbootloader
 *
 *  Created by Michael Hill on 9/27/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

unsigned char readBootSector(void);
unsigned long findFirmware();
unsigned long getFirstSector(unsigned long clusterNumber);

// external variables
volatile unsigned long firstDataSector, rootCluster;
volatile unsigned int sectorPerCluster, reservedSectorCount, bytesPerSector;
unsigned long unusedSectors;