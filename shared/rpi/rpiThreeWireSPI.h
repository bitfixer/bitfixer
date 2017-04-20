#ifndef __RPI_THREE_WIRE_H__
#define __RPI_THREE_WIRE_H__

class rpiThreeWireSPI
{
public:
    rpiThreeWireSPI(int clockPin,
               int chipSelectPin,
               int misoPin,
               int mosiPin)
    : _clockPin(clockPin)
    , _chipSelectPin(chipSelectPin)
    , _misoPin(misoPin)
    , _mosiPin(mosiPin)
    , _clockDelay(5)
    , _byteDelay(2)
    {};

    ~rpiThreeWireSPI() {};
    void init();
    bool isSelected();
    int transfer(unsigned char* buffer, int size);

private:
    int _clockPin;
    int _chipSelectPin;
    int _misoPin;
    int _mosiPin;
    int _clockDelay;
    int _byteDelay;

    void setClock(int value);
    void setOutput();
    void setInput();

    void receiveCommand(unsigned char& cmd, int& size);
    void recvBytes(unsigned char* buffer, int size);
    void sendBytes(unsigned char* buffer, int size);

    void setChipSelect(int value);
    int getChipSelect();
    void setMosi(int value);
    void setData(int value);
    int getMiso();
};

#endif
