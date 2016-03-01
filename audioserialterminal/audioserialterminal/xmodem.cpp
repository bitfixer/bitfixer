//
//  xmodem.cpp
//  audioserialterminal
//
//  Created by Michael Hill on 2/28/16.
//  Copyright © 2016 jaunt. All rights reserved.
//

#include "xmodem.hpp"
#define SOH 0x01
#define ACK 0x06
#define EOT 0x04
#define NAK 21
#define XEOF 0x1A

Xmodem::Xmodem(AudioSerialPort *p) : port(p)
{
    
}

uint16_t Xmodem::calcrc(unsigned char *ptr, int count)
{
    int crc;
    char i;
    
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}

void Xmodem::send(const unsigned char *data, int len)
{
    unsigned char start = 0;
    unsigned char blknum = 1;
    while (port->recv(&start, 1) == 0)
    {
        usleep(10000);
    }
    
    printf("send: got char %02X %c\n", start, start);
    
    // send SOH character
    unsigned char temp;
    uint16_t checksum = 0;
    unsigned char cksum[2];
    
    int remainingBytes = len;
    unsigned char *currptr = (unsigned char *)data;
    unsigned char block[128];
    // send blocks until finished
    while (remainingBytes > 0)
    {
        temp = SOH;
        port->send(&temp, 1);
        port->send(&blknum, 1);
        
        memset(block, XEOF, 128);
        int bytesInBlock = 128;
        if (remainingBytes < 128)
        {
            bytesInBlock = remainingBytes;
        }
        
        memcpy(block, currptr, bytesInBlock);
        //currptr += bytesInBlock;
        //remainingBytes -= bytesInBlock;
        
        checksum = calcrc(block, 128);
        cksum[0] = (checksum & 0xff00) >> 8;
        cksum[1] = checksum & 0x00ff;
        
        port->send(block, 128);
        
        printf("send checksum: %d bytes %02X %02X\n", checksum, cksum[0], cksum[1]);
        port->send(cksum, 2);
        
        // wait for ACK or NAK
        port->recv_sync(&temp, 1);
        
        if (temp == ACK)
        {
            currptr += bytesInBlock;
            remainingBytes -= bytesInBlock;
        }
    }
    
    // send EOT character
    temp = EOT;
    port->send(&temp, 1);
    
    // wait for ACK
    
}

void Xmodem::recv(char *buffer)
{
    /*
    int characters_received = 0;
    unsigned char temp[100];
    // wait for start character
    while (characters_received == 0)
    {
        int res = port->recv(temp, 1);
        if (res == 1)
        {
            printf("received character %02X : %c\n", temp[0], temp[0]);
            characters_received++;
        }
    }
    */
    
    unsigned char ch = 'C';
    port->send(&ch, 1);
    
    unsigned char soh = 0;
    unsigned char blknum = 0;
    unsigned char block[128];
    int recv_checksum = 0;
    int calc_checksum = 0;
    unsigned char cksum[2];
    
    port->recv_sync(&soh, 1);
    if (soh == SOH)
    {
        port->recv_sync(&blknum, 1);
        printf("blknum was %02X\n", blknum);
        port->recv_sync(block, 128);
        
        for (int i = 0; i < 128; i++)
        {
            printf("%d: %02X %c\n", i, block[i], block[i]);
        }
        
        port->recv_sync(cksum, 2);
        printf("checksum bytes %02X %02X\n", cksum[0], cksum[1]);
        
        recv_checksum = cksum[0];
        recv_checksum = recv_checksum << 8;
        recv_checksum = recv_checksum | cksum[1];
        printf("recv checksum: %d\n", recv_checksum);
        
        calc_checksum = calcrc(block, 128);
        printf("calc checksum: %d\n", calc_checksum);
        printf("received block\n");
        
        if (recv_checksum == calc_checksum)
        {
            // block was good
            // send ACK
            unsigned char ack = ACK;
            port->send(&ack, 1);
        }
        else
        {
            // error, need to resend
            unsigned char nak = NAK;
            port->send(&nak, 1);
        }
    }
    else if (soh == EOT)
    {
        // done
    }
    
    /*
    while (port->recv(&soh, 1) == 0)
    {
        usleep(10000);
    }
    
    printf("SOH was %02X\n", soh);
    
    while (port->recv(&blknum, 1) == 0)
    {
        usleep(10000);
    }
    
    printf("blknum was %02X\n", blknum);
    */
     
    /*
    int bytes_remaining = 128;
    unsigned char *blkptr = block;
    while (bytes_remaining > 0)
    {
        int this_bytes_recv = port->recv(blkptr, bytes_remaining);
        printf("recv: got %d bytes\n", this_bytes_recv);
        blkptr += this_bytes_recv;
        bytes_remaining -= this_bytes_recv;
        
        if (bytes_remaining > 0 && this_bytes_recv == 0)
        {
            //sleep(2);
            usleep(10000);
        }
    }
    */
    
    
    
    
    
    /*
    // get checksum
    while (port->recv(&cksum[0], 1) == 0)
    {
        usleep(10000);
    }
    while (port->recv(&cksum[1], 1) == 0)
    {
        usleep(10000);
    }
    */
    
    
    
}

