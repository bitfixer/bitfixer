#include "DataSource.h"
#include <string.h>
extern "C" {
#include "UART_routines.h"
}

void SPIDataSource::init()
{
    // initialize software spi
    spi.init();
    test = 1;
}

void SPIDataSource::read()
{
}

unsigned char SPIDataSource::initializeStorage()
{
    return 1;
}

void* SPIDataSource::findFile(unsigned char* progname)
{
    return 0;
}

unsigned char SPIDataSource::openFileForReading(unsigned char* filename)
{
    memset(&cmd, 0, sizeof(petDiskCommand));
    strcpy((char*)cmd.arg, (const char*)filename);

    // send command to server
    sendCommand(PD_CMD_OPEN_FILE_FOR_READING);

    // get file size
    unsigned char temp[2];
    spi.receive(temp, 2);

    fileSize = temp[0];
    fileSize = fileSize << 8;
    fileSize += temp[1];
    return 1;
}

unsigned int SPIDataSource::getNextFileBlock(unsigned char* buffer)
{
    sendCommand(PD_CMD_READ_BLOCK);
    spi.receive(buffer, 512);
    return 512;
}

int SPIDataSource::getFileSize()
{
    return fileSize;
}

void SPIDataSource::openFileForWriting(unsigned char* filename)
{
    transmitString(filename);
    memset(&cmd, 0, sizeof(petDiskCommand));
    strcpy((char*)cmd.arg, (const char*)filename);

    // send command to server
    sendCommand(PD_CMD_OPEN_FILE_FOR_WRITING);
}

void SPIDataSource::writeBufferToFile(unsigned char* buffer, unsigned int bytesToWrite)
{
    sendCommand(PD_CMD_WRITE_BLOCK);
    spi.send(buffer, bytesToWrite);
}

void SPIDataSource::closeFile()
{
    sendCommand(PD_CMD_CLOSE_FILE);
}

void SPIDataSource::changeDirectory(unsigned char* filename)
{
    memset(&cmd, 0, sizeof(petDiskCommand));
    strcpy((char*)cmd.arg, (const char*)filename);
    sendCommand(PD_CMD_CHANGE_DIRECTORY);
}

void SPIDataSource::openDirectory()
{
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
