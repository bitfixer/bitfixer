#ifndef __DATASOURCE_H__
#define __DATASOURCE_H__

#include "3WireSPI.h"
extern "C" {
    #include "petDiskCommand.h"
}

class DataSource
{
public:
    virtual void read() = 0;
    virtual void init() = 0;
    virtual unsigned char initializeStorage() = 0;
    virtual void* findFile(unsigned char* progname, unsigned long rootCluster) = 0;
    virtual void deleteFile() {};
    virtual void openFileForWriting(unsigned char* filename, unsigned long dirCluster) = 0;
    virtual void writeBufferToFile(unsigned char* buffer, unsigned int bytesToWrite) = 0;
    virtual void closeFile() = 0;
    virtual unsigned char openFileForReading(unsigned char* filename, unsigned long dirCluster) = 0;
    virtual unsigned int getNextFileBlock(unsigned char* buffer) = 0;
    virtual int getFileSize() = 0;
    virtual void openDirectory() = 0;
    virtual DirectoryEntry* getNextDirectoryEntry(unsigned char* buffer) = 0;

    // TEMP?
    virtual unsigned long getRootCluster() {};

    static DataSource* createSPIDataSource();
};

class SPIDataSource : public DataSource
{
public:
    SPIDataSource()
    : fileSize(0)
    {};
    ~SPIDataSource() {};
    void init();
    void read();
    unsigned char initializeStorage();
    void* findFile(unsigned char* progname, unsigned long rootCluster);
    unsigned char openFileForReading(unsigned char* filename, unsigned long dirCluster);
    unsigned int getNextFileBlock(unsigned char* buffer);
    int getFileSize();
    void openFileForWriting(unsigned char* filename, unsigned long dirCluster);
    void writeBufferToFile(unsigned char* buffer, unsigned int bytesToWrite);
    void closeFile();
    void openDirectory();
    DirectoryEntry* getNextDirectoryEntry(unsigned char* buffer);

private:
    void sendCommand(unsigned char cmd_id);
    ThreeWireSPI spi;
    int fileSize;
    petDiskCommand cmd;
    int test;
};

class SDDataSource : public DataSource
{
public:
    SDDataSource() {};
    ~SDDataSource() {};
    void init();
    void read();
    unsigned char initializeStorage();
};

#endif
