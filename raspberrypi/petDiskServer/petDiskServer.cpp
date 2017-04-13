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
               int mosiPin,
               bool masterDrivesChipSelect = true)
    : _clockPin(clockPin)
    , _chipSelectPin(chipSelectPin)
    , _misoPin(misoPin)
    , _mosiPin(mosiPin)
    , _clockDelay(3)
    , _masterDrivesChipSelect(masterDrivesChipSelect)
    {};
    
    ~rpiSoftSPI() {};
    
    void init()
    {
        // clock
        pinMode(_clockPin, OUTPUT);
        digitalWrite(_clockPin, LOW);
        
        if (_masterDrivesChipSelect)
        {
            // chip select
            pinMode(_chipSelectPin, OUTPUT);
            digitalWrite(_chipSelectPin, HIGH);
        }
        else
        {
            pinMode(_chipSelectPin, INPUT);
        }
        
        // data in (MISO)
        pinMode(_misoPin, INPUT);
        
        // data out (MOSI)
        pinMode(_mosiPin, OUTPUT);
        digitalWrite(_mosiPin, LOW);
    }
    
    bool isSelected()
    {
        if (_masterDrivesChipSelect)
        {
            return false;
        }
        
        int cs = getChipSelect();
        //printf("cs %d\n", cs);
        return (cs == LOW);
    }
    
    void send(unsigned char byte)
    {
        unsigned char recvByte = 0;
        //printf("sending %X\n", byte);
        if (_masterDrivesChipSelect)
        {
            // set clock
            setClock(LOW);
        }
            
        // set chip select
        setChipSelect(LOW);
        
        for (unsigned char bit = 0x80; bit != 0; bit >>= 1)
        {
            // present bit
            setMosi(byte & bit);
            delayMicroseconds(_clockDelay);
            setClock(HIGH);
            
            recvByte <<= 1;
            if (getMiso() == HIGH)
            {
                recvByte = recvByte | 0x01;
            }
            
            delayMicroseconds(_clockDelay);
            setClock(LOW);
        }
        
        if (_masterDrivesChipSelect)
        {
            setChipSelect(HIGH);
        }
        else
        {
            while (getChipSelect() == LOW)
            {
                delayMicroseconds(1);
            }
        }
        
        printf("spi received %X\n", recvByte);
    }
    
private:
    int _clockPin;
    int _chipSelectPin;
    int _misoPin;
    int _mosiPin;
    int _clockDelay;
    bool _masterDrivesChipSelect;
    
    void setClock(int value)
    {
        digitalWrite(_clockPin, value);
    }
    
    void setChipSelect(int value)
    {
        digitalWrite(_chipSelectPin, value);
    }
    
    int getChipSelect()
    {
        return digitalRead(_chipSelectPin);
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
                   2, // mosi
                   false
                   );
    
    wiringPiSetup();
    spi.init();
    unsigned char tmp = 0xA0;
    
    while(1)
    {
        if (spi.isSelected())
        {
            spi.send(tmp);
            tmp++;
        }
        else
        {
            delayMicroseconds(1000);
        }
    }
    
    printf("checking for commands..\n");
    
    return 1;
}
