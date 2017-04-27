#include "rpiSoftSpi.h"
#include <stdio.h>
#include <wiringPi.h>

void rpiSoftSPI::init()
{
    // clock
    pinMode(_clockPin, OUTPUT);
    digitalWrite(_clockPin, LOW);

    if (_masterDrivesChipSelect)
    {
        printf("master\n");
        // chip select
        pinMode(_chipSelectPin, OUTPUT);
        digitalWrite(_chipSelectPin, HIGH);
    }
    else
    {
        printf("slave\n");
        pinMode(_chipSelectPin, INPUT);
    }

    // data in (MISO)
    pinMode(_misoPin, INPUT);

    // data out (MOSI)
    pinMode(_mosiPin, OUTPUT);
    digitalWrite(_mosiPin, LOW);
}

bool rpiSoftSPI::isSelected()
{
    if (_masterDrivesChipSelect)
    {
        return true;
    }

    int cs = getChipSelect();
    //printf("%d\n", cs);
    return (cs == LOW);
}

void rpiSoftSPI::transfer(unsigned char* buffer, int size)
{
    printf("h");
    unsigned char recvByte = 0;
    if (_masterDrivesChipSelect)
    {
        // set chip select
        setChipSelect(LOW);
    }
    else
    {
        while (!isSelected())
        {
            delayMicroseconds(1);
        }
    }

    // set clock
    setClock(LOW);

    printf("yo");
    for (int b = 0; b < size; b++)
    {
        for (unsigned char bit = 0x80; bit != 0; bit >>= 1)
        {
            // present bit
            setMosi(buffer[b] & bit);
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
        buffer[b] = recvByte;
        delayMicroseconds(_byteDelay);
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
}

// private methods
void rpiSoftSPI::setClock(int value)
{
    digitalWrite(_clockPin, value);
}

void rpiSoftSPI::setChipSelect(int value)
{
    digitalWrite(_chipSelectPin, value);
}

int rpiSoftSPI::getChipSelect()
{
    return digitalRead(_chipSelectPin);
}

void rpiSoftSPI::setMosi(int value)
{
    digitalWrite(_mosiPin, value);
}

int rpiSoftSPI::getMiso()
{
    return digitalRead(_misoPin);
}
