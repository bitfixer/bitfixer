#include <wiringPi.h>
#include <stdio.h>

class SPI
{
public:
    SPI() {}
    int recv(unsigned char *buffer)
    {
        int bytes_received = 0;
        int cycles = 0;
        int sck = 0;
        int val = 0;
        bool sck_rising = false;
        int this_sck = 0;
        unsigned char byte = 0;
        
        while (val == 0)
        {
            this_sck = fastDigitalRead(1);
            if (this_sck > sck)
            {
                byte <<= 1;
                byte |= fastDigitalRead(2);
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
        
        /*
        while (val == 0)
        {
            this_sck = fastDigitalRead(1);
            if (this_sck > sck)
            {
                bits[cycles] = fastDigitalRead(2);
                cycles++;
            }
            
            sck = this_sck;
            val = fastDigitalRead(0);
        }
        if (cycles == 8)
        {
            unsigned char byte = 0;
            for (int i = 0; i < 8; i++)
            {
                byte <<= 1;
                byte |= bits[i];
            }
            //printf("%02X\n", byte);
            
            buffer[bytes_received] = byte;
            bytes_received++;
        }
        */
    }
};




// test - watch for input
int main(void)
{
    wiringPiSetup();
    pinMode(0, INPUT);
    pinMode(1, INPUT);
    pinMode(2, INPUT);
    pullUpDnControl(0, PUD_OFF);
    pullUpDnControl(1, PUD_OFF);
    pullUpDnControl(2, PUD_OFF);
    
    printf("waiting..\n");
    
    unsigned char buffer[1024];
    SPI spi;
    
    /*
    while (1)
    {
        int val = 0;
        val = fastDigitalRead(0);
        //val = digitalRead(0);
        printf("val is %d\n", val);
        delay(500);
    }
    */
    
    /*
    while (1)
    {
        int val = 1;
        int cycles = 0;
        
        while (val != 0)
        {
            //val = digitalRead(0);
            val = fastDigitalRead(0);
        }
        
        while (val == 0)
        {
            cycles++;
            //val = digitalRead(0);
            val = fastDigitalRead(0);
        }
        
        printf("c %d\n", cycles);
    }
    */
    
    while (1)
    {
        // wait until CS asserted
        int val = 1;
        int sck = 0;
        while (val == 1)
        {
            val = fastDigitalRead(0);
        }
        
        /*
        int cycles = 0;
        while (val == 0)
        {
            int this_sck = fastDigitalRead(1);
            if (this_sck > sck)
            {
                bits[cycles] = fastDigitalRead(2);
                cycles++;
            }
            
            sck = this_sck;
            val = fastDigitalRead(0);
        }
        
        printf("c %d done.\n", cycles);
        
        if (cycles == 8)
        {
            unsigned char byte = 0;
            for (int i = 0; i < 8; i++)
            {
                byte <<= 1;
                byte |= bits[i];
            }
            printf("%02X\n", byte);
        }
        */
        
        int n = spi.recv(buffer);
        if (n > 0)
        {
            for (int i = 0; i < n; i++)
            {
                printf("%d: %c %02X\n", n, buffer[i], buffer[i]);
            }
        }
    }
    
}