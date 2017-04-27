#include "DataSource.h"
extern "C" {
#include "UART_routines.h"
}

void SPIDataSource::init()
{
    // initialize software spi
    spi.init();
    test = 1;
    /*
    unsigned char temp[10];
    for (int i = 0; i < 10; i++)
    {
        temp[i] = i;
    }
    //spi.send(temp, 10);
    spi.receive(temp, 10);

    for (int i = 0; i < 10; i++)
    {
        transmitHex(CHAR, temp[i]);
        //transmitString((unsigned char*)"\r\n");
    }
    */
}

void SPIDataSource::read()
{
    //transmitString((unsigned char*)"yo yo");
}

unsigned char SPIDataSource::initializeStorage()
{
    /*
    transmitString((unsigned char*)"init storage");
    unsigned char temp[10];
    for (int i = 0; i < 10; i++)
    {
        temp[i] = i;
    }
    spi.transfer_asm(temp, 10);
    transmitString((unsigned char*)"transferred.");
    return 1;
    */
    return 1;
}

void* SPIDataSource::findFile(unsigned char* progname, unsigned long rootCluster)
{
    return 0;
}

unsigned char SPIDataSource::openFileForReading(unsigned char* filename, unsigned long dirCluster)
{
    //cmd.command_id = PD_CMD_OPEN_FILE_FOR_READING;
    for (int i = 0; i < 12; i++)
    {
        cmd.arg[i] = filename[i];
    }

    // send command to server
    //spi.send((unsigned char*)&cmd, sizeof(petDiskCommand));
    sendCommand(PD_CMD_OPEN_FILE_FOR_READING);

    // get file size
    unsigned char temp[2];
    //spi.transfer(temp, 2);
    spi.receive(temp, 2);

    fileSize = temp[0];
    fileSize = fileSize << 8;
    fileSize += temp[1];

    //transmitHex(INT, fileSize);
    return 1;
}

unsigned int SPIDataSource::getNextFileBlock(unsigned char* buffer)
{
    //cmd.command_id = PD_CMD_READ_BLOCK;
    //spi.send((unsigned char*)&cmd, sizeof(petDiskCommand));
    sendCommand(PD_CMD_READ_BLOCK);

    spi.receive(buffer, 512);
    return 512;
}

int SPIDataSource::getFileSize()
{
    return fileSize;
}

void SPIDataSource::openFileForWriting(unsigned char* filename, unsigned long dirCluster)
{
    transmitString(filename);
    transmitString((unsigned char*)" open for writing\r\n");
    for (int i = 0; i < 12; i++)
    {
        cmd.arg[i] = filename[i];
    }
    // send command to server
    sendCommand(PD_CMD_OPEN_FILE_FOR_WRITING);
}

void SPIDataSource::writeBufferToFile(unsigned char* buffer, unsigned int bytesToWrite)
{
    transmitString((unsigned char*)"writing buffer\r\n");
    sendCommand(PD_CMD_WRITE_BLOCK);

    spi.send(buffer, bytesToWrite);
}

void SPIDataSource::closeFile()
{
    transmitString((unsigned char*)"closing file\r\n");
    sendCommand(PD_CMD_CLOSE_FILE);
}

void SPIDataSource::openDirectory()
{
    transmitString((unsigned char*)"open directory\r\n");
    sendCommand(PD_CMD_DIRECTORY);
}

void SPIDataSource::sendCommand(unsigned char cmd_id)
{
    cmd.command_id = cmd_id;
    spi.send((unsigned char*)&cmd, sizeof(petDiskCommand));
}

DirectoryEntry* SPIDataSource::getNextDirectoryEntry(unsigned char* buffer)
{
    sendCommand(PD_CMD_GET_NEXT_DIRECTORY_ENTRY);
    spi.receive(buffer, sizeof(DirectoryEntry));

    DirectoryEntry* dirent = (DirectoryEntry*)buffer;
    if (dirent->valid != 0)
    {
        return dirent;
    }

    return 0;
}
