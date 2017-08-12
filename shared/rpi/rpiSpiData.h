#ifndef __RPI_SPI_DATA_H__
#define __RPI_SPI_DATA_H__

class rpiSpiData
{
public:
    rpiSpiData(int deviceReadyPin, int spi)
    : _deviceReadyPin(deviceReadyPin)
    , _spi(spi)
    {};
    ~rpiSpiData() {};

    // receive a packet from a device
    int receive(unsigned char* buffer);
    void send(unsigned char* buffer, int send_size);

private:
    int _deviceReadyPin;
    int _spi;
};

#endif
