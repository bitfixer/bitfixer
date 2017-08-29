//
//  Fifo.h
//  
//
//  Created by Michael Hill on 8/28/17.
//
//

#ifndef Fifo_h
#define Fifo_h

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

class Fifo {
public:
    
    Fifo() {};
    ~Fifo() {};
    
    void init(const char* outName, const char* inName, bool server = false)
    {
        if (server)
        {
            mkfifo(outName, 0666);
            mkfifo(inName, 0666);
            _fdOut = open(outName, O_WRONLY);
            _fdIn = open(inName, O_RDONLY);
        }
        else
        {
            _fdIn = open(inName, O_RDONLY);
            _fdOut = open(outName, O_WRONLY);
        }
    }
    
    void send(unsigned char* data, int size)
    {
        write(_fdOut, data, size);
    }
    
    int recv(unsigned char* data, int size)
    {
        return read(_fdIn, data, size);
    }
    
private:
    int _fdOut;
    int _fdIn;
};

#endif /* Fifo_h */
