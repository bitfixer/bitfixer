//
//  c64drive.h
//  
//
//  Created by Michael Hill on 8/15/17.
//
//

#ifndef c64drive_h
#define c64drive_h

#define LISTEN   0x20
#define UNLISTEN 0x3F
#define TALK     0x40
#define UNTALK   0x5F

typedef struct
{
    unsigned char device_id;
    unsigned char data_size;
    unsigned char data_buffer[128];
    unsigned char is_last_data_buffer;
    unsigned char atn_size;
    unsigned char atn_buffer[8];
    
} dataPacket;

#endif /* c64drive_h */
