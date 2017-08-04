#ifndef __PET_DISK_COMMAND_H__
#define __PET_DISK_COMMAND_H__

#define PD_CMD_OPEN_FILE_FOR_READING 0x01
#define PD_CMD_OPEN_FILE_FOR_WRITING 0x02
#define PD_CMD_WRITE_BLOCK 0x03
#define PD_CMD_READ_BLOCK 0x04
#define PD_CMD_CLOSE_FILE 0x05
#define PD_CMD_DIRECTORY 0x06
#define PD_CMD_GET_NEXT_DIRECTORY_ENTRY 0x07
#define PD_CMD_CHANGE_DIRECTORY 0x08

typedef struct
{
    unsigned char command_id;
    unsigned char arg[40];
} petDiskCommand;

typedef struct
{
    unsigned char valid;
    unsigned char name_length;
    unsigned char name[17];
    unsigned char ext[3];
} DirectoryEntry;

#endif
