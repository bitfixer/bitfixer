#include "C64Dos.h"

bool C64Dos::open(const char* path, int channel)
{
    _channel = dosOpenFile(path, channel);
    if (!_channel.file && !_channel.buffer)
    {
        return false;
    }
    
    return true;
}

void C64Dos::close()
{
    if (_channel.file)
    {
        fclose(_channel.file);
        _channel.file = NULL;
    }
    
    if (_channel.buffer)
    {
        free(_channel.buffer);
        _channel.buffer = NULL;
        _channel.length = 0;
    }
}

int C64Dos::read(unsigned char* data, int size)
{
    if (_channel.file)
    {
        return fread(data, 1, size, _channel.file);
    }
    else if (_channel.buffer)
    {
        int bytesToSend = size;
        int bytesRemaining = _channel.length - _channel.sent;
        if (bytesToSend > bytesRemaining)
        {
            bytesToSend = bytesRemaining;
        }
        
        if (bytesToSend > 0)
        {
            memcpy(data, &_channel.buffer[sent], bytesToSend);
            sent += bytesToSend;
        }
        
        return bytesToSend;
    }
    
    return 0;
}

void C64Dos::write(unsigned char* data, int size)
{
    if (_channel.file)
    {
        fwrite(data, 1, size, _channel.file);
    }
    else
    {
        printf("error: trying to write to buffer\n");
    }
}
