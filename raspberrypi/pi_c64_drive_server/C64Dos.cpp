#include "C64Dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void C64Dos::init()
{
    for (int i = 0; i < 15; i++)
    {
        _buffers[i] = NULL;
        _bufferSize[i] = 0;
    }
    
    dosInitDrives();
    dosMountDisk(".", 0);
}

void C64Dos::mount(const char* path, int drive)
{
    dosMountDisk(path, drive);
}

bool C64Dos::open(const char* path, int channel)
{
    if (channel == 15)
    {
        _channel = dosSendError();
    }
    else if (channel < 2)
    {
        _channel = dosOpenFile(path, channel);
    }
    else
    {
        // create buffer if it doesn't exist
        if (_buffers[channel] == NULL)
        {
            _buffers[channel] = (unsigned char*)malloc(10000);
        }
    }
    
    if (!_channel.file && !_channel.buffer)
    {
        _fileOpen = false;
        return false;
    }
    
    _fileOpen = true;
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
    
    _fileOpen = false;
}

int C64Dos::read(unsigned char* data, int size, bool& last)
{
    int bytesRead = 0;
    last = false;
    if (_channel.file)
    {
        bytesRead = fread(data, 1, size, _channel.file);
    }
    else if (_channel.buffer)
    {
        bytesRead = size;
        int bytesRemaining = _channel.length - _channel.sent;
        if (bytesRead >= bytesRemaining)
        {
            bytesRead = bytesRemaining;
            last = true;
        }
        
        if (bytesRead > 0)
        {
            memcpy(data, &_channel.buffer[_channel.sent], bytesRead);
        }
    }
    
    _channel.sent += bytesRead;
    if (_channel.sent >= _channel.length)
    {
        last = true;
    }
    
    return bytesRead;
}

void C64Dos::write(unsigned char* data, int size)
{
    if (_channel.file)
    {
        fwrite(data, 1, size, _channel.file);
        fflush(_channel.file);
    }
    else
    {
        printf("error: trying to write to buffer\n");
    }
}
