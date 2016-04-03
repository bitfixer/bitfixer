#include "utils.h"

void circularBuffer_push(void *thisp, unsigned char byte);
int circularBuffer_getSize(void *thisp);
unsigned char circularBuffer_pop(void *thisp);

void circularBuffer_push(void *thisp, unsigned char byte)
{
    circularBuffer *cbuf = (circularBuffer *)thisp;
    cbuf->bytes[cbuf->writeIndex] = byte;
    cbuf->writeIndex++;
    if (cbuf->writeIndex >= cbuf->capacity)
    {
        cbuf->writeIndex = 0;
    }
    
    cbuf->numBytes++;
}

int circularBuffer_getSize(void *thisp)
{
    circularBuffer *cbuf = (circularBuffer *)thisp;
    return cbuf->numBytes;
}

unsigned char circularBuffer_pop(void *thisp)
{
    unsigned char byte = 0;
    circularBuffer *cbuf = (circularBuffer *)thisp;
    byte = cbuf->bytes[cbuf->readIndex];
    cbuf->readIndex++;
    if (cbuf->readIndex >= cbuf->capacity)
    {
        cbuf->readIndex = 0;
    }
    
    cbuf->numBytes--;
    return byte;
}

// initialize circular buffer struct
void circularBuffer_init(void *thisp)
{
    circularBuffer *cbuf = (circularBuffer *)thisp;
    cbuf->capacity = 128;
    cbuf->numBytes = 0;
    cbuf->readIndex = 0;
    cbuf->writeIndex = 0;
    
    cbuf->push = &circularBuffer_push;
    cbuf->getSize = &circularBuffer_getSize;
    cbuf->pop = &circularBuffer_pop;
}