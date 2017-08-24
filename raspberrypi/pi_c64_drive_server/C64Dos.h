#ifndef __C64_DOS_H__
#define __C64_DOS_H__

#include "cbmdos.h"

class C64Dos
{
public:
    bool open(const char* path, int channel);
    void close();
    int read();
    void write(unsigned char* data, int size);
    
private:
    CBMDOSChannel _channel;
};

#endif
