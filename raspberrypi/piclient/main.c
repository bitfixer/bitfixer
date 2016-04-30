#include <wiringPi.h>
#include <stdio.h>
/*
int main (void)
{
    wiringPiSetup () ;
    pinMode (0, OUTPUT) ;
    for (;;)
    {
        digitalWrite (0, HIGH) ; delay (500) ;
        digitalWrite (0,  LOW) ; delay (500) ;
    }
    return 0 ;
}
*/

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
    
    /*
    while (1)
    {
        int val = digitalRead(0);
        printf("pin is %d\n", val);
        //delay(500);
    }
    */
    
    printf("waiting..\n");
    
    unsigned char bits[8];
    
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
    }
    
}