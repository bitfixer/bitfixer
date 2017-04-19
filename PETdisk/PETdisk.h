#ifndef _PETDISK_H
#define _PETDISK_H

typedef enum _filedir
{
    FNONE,
    FREAD,
    FWRITE
} filedir;

typedef struct _pdStateVars
{
    char openFileAddress;
    int fileWriteByte;
    unsigned int fileReadByte;
    unsigned char useRemainderByte;
    unsigned char remainderByte;
    filedir fileDirection;
    unsigned char fileNotFound;
    unsigned long currentDirectoryCluster;
    unsigned char sd_boot_checksum;
} pdStateVars;

void pgm_memcpy(unsigned char *dest, const unsigned char *src, int len);

#endif
