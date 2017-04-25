#include "DataSource.h"
extern "C" {
    #include "SPI_routines.h"
}

void SDDataSource::init()
{
    spi_init();
}

void SDDataSource::read()
{
    // setup SPI port
    //SPI_CTL = (unsigned char)(~MISO & ~DATA0 & ~DATA1 & ~CASSETTE_READ & ~CASSETTE_WRITE);
    //SPI_PORT = 0x7f;
}

void SDDataSource::initializeStorage()
{

}
