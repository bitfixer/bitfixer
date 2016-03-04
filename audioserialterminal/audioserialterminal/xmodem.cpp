//
//  xmodem.cpp
//  audioserialterminal
//
//  Created by Michael Hill on 2/28/16.
//  Copyright © 2016 jaunt. All rights reserved.
//

#include "xmodem.hpp"
#include <unistd.h>
#define SOH 0x01
#define ACK 0x06
#define EOT 0x04
#define NAK 0x15
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
    /*
    while (port->recv(&start, 1) == 0)
    {
        usleep(10000);
    }
    */
    printf("waiting to start\n");
    port->recv_sync(&start, 1);
    
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
        
        unsigned char blknum_comp = 255 - blknum;
        port->send(&blknum_comp, 1);
        
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
        
        printf("looking for ACK, got %02X\n", temp);
        
        if (temp == ACK)
        {
            currptr += bytesInBlock;
            remainingBytes -= bytesInBlock;
            blknum++;
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
    printf("Receiving!\n");
    
    unsigned char recv_buffer[1000000];
    long recv_pos = 0;
    
    unsigned char ch = 'C';
    
    port->send(&ch, 1);
    printf("sent C\n");
    
    unsigned char soh = 0;
    unsigned char blknum = 0;
    unsigned char blknum_comp = 255;
    unsigned char block[128];
    int recv_checksum = 0;
    int calc_checksum = 0;
    unsigned char cksum[2];
    
    bool done = false;
    unsigned char prev_block = 1;
    bool last_block_good = false;
    bool first_block_received = false;
    
    while (!done)
    {
        port->recv_sync(&soh, 1);
        //printf("got %02X\n", soh);
        if (soh == SOH)
        {
            port->recv_sync(&blknum, 1);
            //printf("blknum was %02X\n", blknum);
            port->recv_sync(&blknum_comp, 1);
            //printf("blknum_comp was %02X\n", blknum_comp);
            
            int res = port->recv_sync(block, 128);
            printf("block recv %d\n", res);
            
            /*
            for (int i = 0; i < 128; i++)
            {
                printf("%d: %02X\n", i, block[i]);
            }
            */
            
            
            port->recv_sync(cksum, 2);
            
            recv_checksum = cksum[0];
            recv_checksum = recv_checksum << 8;
            recv_checksum = recv_checksum | cksum[1];
            printf("recv checksum: %d\n", recv_checksum);
            
            calc_checksum = calcrc(block, 128);
            printf("calc checksum: %d\n", calc_checksum);
            printf("received block %d\n", blknum);
            first_block_received = true;
            
            if (recv_checksum == calc_checksum)
            {
                // block was good
                // write received block
                bool block_ok = true;
                if (blknum != prev_block)
                {
                    // block should only advance by 1
                    unsigned char next_block = prev_block + 1; // wrap around
                    if (blknum == next_block && blknum_comp == 255 - blknum)
                    {
                        // advance position
                        recv_pos += 128;
                    }
                    else
                    {
                        block_ok = false;
                    }
                }
                if (block_ok)
                {
                    prev_block = blknum;
                    memcpy(&recv_buffer[recv_pos], block, 128);
                    printf("sending ACK\n");
                    last_block_good = true;
                    // send ACK
                    unsigned char ack = ACK;
                    port->send(&ack, 1);
                }
                else // block number mismatch, reject block
                {
                    last_block_good = false;
                    printf("sending NAK\n");
                    // error, need to resend
                    unsigned char nak = NAK;
                    port->send(&nak, 1);
                }
            }
            else
            {
                last_block_good = false;
                printf("sending NAK\n");
                // error, need to resend
                unsigned char nak = NAK;
                port->send(&nak, 1);
                
                // TEST TEST
                //done = true;
            }
        }
        else if (soh == EOT)
        {
            if (last_block_good)
            {
                // done
                printf("file received!\n");
                unsigned char ack = ACK;
                port->send(&ack, 1);
                done = true;
            }
            else
            {
                printf("sending NAK\n");
                // error, need to resend
                unsigned char nak = NAK;
                port->send(&nak, 1);
            }
        }
        else
        {
            // something's off
            usleep(100000);
            // flush buffer
            unsigned char temp[1024];
            while (port->recv(temp, 1024) > 0) {}
            
            //if (first_block_received)
            {
                printf("sending NAK?\n");
                // error, need to resend
                unsigned char nak = NAK;
                port->send(&nak, 1);
            }
            
            /*
            else
            {
                // try to start again
                unsigned char start = 'C';
                port->send(&start, 1);
            }
            */
        }
    }
    
    /*
    long len = 0;
    for (long i = 0; i < recv_pos; i++)
    {
        if (recv_buffer[i] == XEOF)
        {
            recv_buffer[i] = 0x00;
            len = i;
        }
    }
    */
    
    printf("received:\n%s\n", recv_buffer);
     
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

