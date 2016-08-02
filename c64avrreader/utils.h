#ifndef __UTILS_H__
#define __UTILS_H__

typedef unsigned char bool;
#define true 1
#define false 0

#define call(A, B)          (A.B)(&A)
#define call1(A, B, C)      (A.B)(&A, C)

/*
typedef struct
{
    void (*init)(void *thisp);
    void (*push)(void *thisp, unsigned char byte);
    int (*getSize)(void *thisp);
    unsigned char (*pop)(void *);
} circularBuffer_VTable;
*/
 
typedef struct
{
    unsigned char bytes[128];
    int capacity;
    int numBytes;
    int readIndex;
    int writeIndex;
    
    // function table
    void (*push)(void *thisp, unsigned char byte);
    int (*getSize)(void *thisp);
    unsigned char (*pop)(void *);
} circularBuffer;

void circularBuffer_init(void *thisp);

#endif