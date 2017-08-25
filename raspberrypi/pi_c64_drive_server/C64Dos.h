#ifndef __C64_DOS_H__
#define __C64_DOS_H__

#include "cbmdos.h"

class C64Dos
{
public:
    C64Dos()
    : _fileOpen(false)
    {};
    
    ~C64Dos() {};
    void init();
    bool open(const char* path, int channel);
    void close();
    int read(unsigned char* data, int size, bool& last);
    void write(unsigned char* data, int size);
    
    int getFileLength()
    {
        return _channel.length;
    }
    
    int getFileBytesSent()
    {
        return _channel.sent;
    }
    
    bool fileIsOpen()
    {
        return _fileOpen;
    }
    
private:
    CBMDOSChannel _channel;
    bool _fileOpen;
    
};

#endif
