#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>

class SPI
{
public:
    SPI() {}
    
    void init()
    {
        wiringPiSetup();
        pinMode(0, INPUT);
        pinMode(1, INPUT);
        pinMode(2, INPUT);
        pinMode(3, OUTPUT);
        pullUpDnControl(0, PUD_OFF);
        pullUpDnControl(1, PUD_OFF);
        pullUpDnControl(2, PUD_OFF);
    }
    
    int selected()
    {
        return fastDigitalRead(0);
    }
    
    int recv(unsigned char *buffer)
    {
        int bytes_received = 0;
        int cycles = 0;
        int sck = 0;
        int val = 0;
        bool sck_rising = false;
        int this_sck = 0;
        int this_bit = 0;
        unsigned char byte = 0;
        
        while (val == 0)
        {
            this_sck = fastDigitalRead(1);
            this_bit = fastDigitalRead(2);
            if (this_sck > sck)
            {
                byte <<= 1;
                byte |= this_bit;
                cycles++;
            }
            
            if (cycles == 8)
            {
                buffer[bytes_received++] = byte;
                byte = 0;
                cycles = 0;
            }
            
            sck = this_sck;
            val = fastDigitalRead(0);
        }
        
        return bytes_received;
    }
    
    int recvbyte(unsigned char *rbyte)
    {
        int cycles = 0;
        int sck = 0;
        int val = 0;
        bool sck_rising = false;
        int this_sck = 0;
        int this_bit = 0;
        unsigned char byte = 0;
        
        sck = fastDigitalRead(1);
        
        while (val == 0)
        {
            this_sck = fastDigitalRead(1);
            this_bit = fastDigitalRead(2);
            if (this_sck > sck)
            {
                byte <<= 1;
                byte |= this_bit;
                cycles++;
            }
            
            if (cycles == 8)
            {
                *rbyte = byte;
                return 1;
            }
            
            sck = this_sck;
            val = fastDigitalRead(0);
        }
        
        return 0;
    }
    
    int sendbyte(unsigned char *rbyte, unsigned char sbyte)
    {
        int cycles = 0;
        int sck = 0;
        int val = 0;
        bool sck_rising = false;
        int this_sck = 0;
        int this_bit = 0;
        unsigned char byte = 0;
        unsigned char mask = 0x80;
        //bool test = false;
        
        sck = fastDigitalRead(1);
        
        while (val == 0)
        {
            this_sck = fastDigitalRead(1);
            this_bit = fastDigitalRead(2);
            if (this_sck > sck)
            {
                byte <<= 1;
                byte |= this_bit;
                cycles++;
            }
            else if (this_sck < sck)
            {
                /*
                if (test)
                {
                    digitalWrite(3, LOW);
                    test = false;
                }
                else
                {
                    digitalWrite(3, HIGH);
                    test = true;
                }
                */
                
                if ((sbyte & mask) == 0)
                {
                    digitalWrite(3, LOW);
                }
                else
                {
                    digitalWrite(3, HIGH);
                }
                sbyte <<= 1;
            }
            
            if (cycles == 8)
            {
                *rbyte = byte;
                return 1;
            }
            
            sck = this_sck;
            val = fastDigitalRead(0);
        }
        
        return 0;
    }
    
    void send2(unsigned char byte)
    {
        int sck = 0;
        int this_sck = 0;
        int bits_sent = 0;
        unsigned char mask = 0x80;
        sck = fastDigitalRead(1);
        
        while (bits_sent < 8)
        {
            this_sck = fastDigitalRead(1);
            if (this_sck < sck)
            {
                /*
                // output bit
                if (byte & mask == 0)
                {
                    digitalWrite(3, LOW);
                }
                else
                {
                    digitalWrite(3, HIGH);
                }
                */
                digitalWrite(3, LOW);
                byte <<= 1;
                bits_sent++;
            }
            
            sck = this_sck;
        }
        
        while (sck == 0)
        {
            sck = fastDigitalRead(1);
        }
    }
    
    bool ready;
};

// test - watch for input
int main(void)
{
    unsigned char buffer[1024];
    SPI spi;
    spi.init();
    
    printf("hey\n");
    unsigned char byte;
    int readval = 0;
    int bytes_received = 0;
    int total = 0;
    unsigned char *b;
    bool readytogo = false;
    while (1)
    {
        readval = fastDigitalRead(0);
        //printf("%d", readval);
        
        if (readval == 0)
        {
            b = buffer;
            total = 0;
            bytes_received = 1;
            readytogo = false;
            while (bytes_received == 1 && !readytogo)
            {
                bytes_received = spi.recvbyte(b);
                //if (*b == 'o')
                //    readytogo = true;
                b += bytes_received;
                total += bytes_received;
            }
            
            
            // send a byte
            //byte = 'X';
            //spi.send(byte);
            
            /*
            byte = 'X';
            bytes_received = spi.sendbyte(b, byte);
            b += bytes_received;
            total += bytes_received;
            
            bytes_received = spi.recvbyte(b);
            b += bytes_received;
            total += bytes_received;
            */
             
            for (int bb = 0; bb < total; bb++)
            {
                printf("%c  %02X\n", buffer[bb], buffer[bb]);
            }
            printf("\n");
        }
        else
        {
            //printf("*");
        }
        
#ifdef POOP
        if (fastDigitalRead(0) == 0)
        {
            
            /*
            int bytes_received = 0;
            while (spi.recvbyte(&byte) == 1)
            {
                bytes_received++;
            }
            
            printf("got %d bytes", bytes_received);
            */
            
            
            /*
            int n = spi.recv(buffer);
            if (n > 0)
            {
                for (int i = 0; i < n; i++)
                {
                    printf("%d: %c %02X\n", n, buffer[i], buffer[i]);
                }
            }
            */
            //delay(500);
            
            /*
            unsigned char *b = buffer;
            int n = 1;
            int total = 0;
            n = spi.recv(b,1);
            if (n>0)
                printf("%c", b[0]);
            */
        }
        else
        {
            // not ready
            //printf("!");
            //usleep(10);
        }
#endif
    }
}