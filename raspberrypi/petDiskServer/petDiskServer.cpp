#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>

class rpiSoftSPI
{
public:
    rpiSoftSPI(int clockPin,
               int chipSelectPin,
               int misoPin,
               int mosiPin)
    : _clockPin(clockPin)
    , _chipSelectPin(chipSelectPin)
    , _misoPin(misoPin)
    , _mosiPin(mosiPin)
    {};
    
    ~rpiSoftSPI() {};
    
    void init()
    {
        // clock
        pinMode(_clockPin, OUTPUT);
        digitalWrite(_clockPin, LOW);
        
        // chip select
        pinMode(_chipSelectPin, OUTPUT);
        digitalWrite(_chipSelectPin, LOW);
        
        // data in (MISO)
        pinMode(_misoPin, INPUT);
        
        // data out (MOSI)
        pinMode(_mosiPin, OUTPUT);
        digitalWrite(_mosiPin, LOW);
    }
    
    void send(unsigned char byte)
    {
        // set clock
        setClock(LOW);

        // set chip select
        setChipSelect(LOW);
        
        for (unsigned char bit = 0x80; bit != 0; bit >>= 1)
        {
            // present bit
            setMosi(byte & bit);
            delayMicroseconds(2);
            setClock(HIGH);
            delayMicroseconds(2);
            setClock(LOW);
        }
        
        setChipSelect(HIGH);
    }
    
private:
    int _clockPin;
    int _chipSelectPin;
    int _misoPin;
    int _mosiPin;
    
    void setClock(int value)
    {
        digitalWrite(_clockPin, value);
    }
    
    void setChipSelect(int value)
    {
        digitalWrite(_chipSelectPin, value);
    }
    
    void setMosi(int value)
    {
        digitalWrite(_mosiPin, value);
    }
    
    int getMiso()
    {
        return digitalRead(_misoPin);
    }
    
};

int main(int argc, char **argv)
{
    unsigned char buffer[256];
    int value = 0;
    rpiSoftSPI spi(1, // clock
                   0, // chip select
                   3, // miso
                   2  // mosi
                   );
    
    wiringPiSetup();
    spi.init();
    
    while(1)
    {
        spi.send(0xCD);
        delayMicroseconds(5000000);
    }
    
    printf("checking for commands..\n");
    
    return 1;
}
