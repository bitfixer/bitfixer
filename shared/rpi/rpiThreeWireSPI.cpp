#include "rpiThreeWireSPI.h"
#include <stdio.h>
#include <wiringPi.h>

#define TW_SENDCMD 0x01
#define TW_RECVCMD 0x02

void rpiThreeWireSPI::init()
{
    // clock
    pinMode(_clockPin, OUTPUT);
    digitalWrite(_clockPin, LOW);

    pinMode(_chipSelectPin, INPUT);

    // data in (MISO)
    pinMode(_misoPin, INPUT);

    // data out (MOSI)
    pinMode(_mosiPin, OUTPUT);
    digitalWrite(_mosiPin, LOW);
}

void rpiThreeWireSPI::setOutput()
{
    pinMode(_misoPin, OUTPUT);
}

void rpiThreeWireSPI::setInput()
{
    pinMode(_misoPin, INPUT);
}

bool rpiThreeWireSPI::isSelected()
{
    int cs = getChipSelect();
    return (cs == LOW);
}

void rpiThreeWireSPI::receiveCommand(unsigned char& cmd, int& size)
{
    unsigned char cmdbytes[3];
    recvBytes(cmdbytes, 3);

    //printf("%X %X %X\n", cmdbytes[0], cmdbytes[1], cmdbytes[2]);

    // parse bytes
    cmd = cmdbytes[0];
    int s = (int)cmdbytes[1] << 8;
    s = s + cmdbytes[2];
    size = s;
}

void rpiThreeWireSPI::recvBytes(unsigned char* buffer, int size)
{
    setClock(LOW);
    for (int b = 0; b < size; b++)
    {
        unsigned char recvByte = 0;
        for (int bit = 0; bit < 8; bit++)
        {
            delayMicroseconds(_clockDelay);
            setClock(HIGH);

            // read bit
            recvByte <<= 1;
            if (getMiso() == HIGH)
            {
                recvByte = recvByte | 0x01;
            }

            delayMicroseconds(_clockDelay);
            setClock(LOW);
        }
        buffer[b] = recvByte;
        //printf("recv %d: %X\n", b, buffer[b]);
        delayMicroseconds(_byteDelay);
    }
    //printf("recv done\n");
}

void rpiThreeWireSPI::sendBytes(unsigned char* buffer, int size)
{
    setClock(LOW);
    for (int b = 0; b < size; b++)
    {
        unsigned char sendByte = buffer[b];
        for (unsigned char bit = 0x80; bit != 0; bit >>= 1)
        {
            // present bit
            setData(buffer[b] & bit);

            delayMicroseconds(_clockDelay);
            setClock(HIGH);

            delayMicroseconds(_clockDelay);
            setClock(LOW);
        }
        delayMicroseconds(_byteDelay);
    }
}

int rpiThreeWireSPI::transfer(unsigned char* buffer, int size)
{
    unsigned char recvByte = 0;

    // wait for chip select
    while (!isSelected())
    {
        delayMicroseconds(1);
    }
    // set clock
    setClock(LOW);

    unsigned char cmd = 0;
    int cmd_size = 0;
    receiveCommand(cmd, cmd_size);
    //printf("cmd %d, size %d\n", cmd, cmd_size);

    // delay to allow master (mcu) to set up its io lines.
    // possibly replace this with explicit handshaking
    delayMicroseconds(1000);

    // send or receive
    if (cmd == TW_SENDCMD)
    {
        recvBytes(buffer, cmd_size);
    }
    else if (cmd == TW_RECVCMD)
    {
        delayMicroseconds(_byteDelay);
        setOutput();
        sendBytes(buffer, cmd_size);
        setInput();
    }

    while (getChipSelect() == LOW)
    {
        delayMicroseconds(1);
    }

    // do one clock cycle
    setClock(HIGH);
    delayMicroseconds(_clockDelay);
    setClock(LOW);

    return cmd_size;
}

// private methods
void rpiThreeWireSPI::setClock(int value)
{
    digitalWrite(_clockPin, value);
}

void rpiThreeWireSPI::setChipSelect(int value)
{
    digitalWrite(_chipSelectPin, value);
}

int rpiThreeWireSPI::getChipSelect()
{
    return digitalRead(_chipSelectPin);
}

void rpiThreeWireSPI::setMosi(int value)
{
    digitalWrite(_mosiPin, value);
}

void rpiThreeWireSPI::setData(int value)
{
    digitalWrite(_misoPin, value);
}

int rpiThreeWireSPI::getMiso()
{
    return digitalRead(_misoPin);
}
