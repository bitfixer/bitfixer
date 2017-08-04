#ifndef __URL_FETCHER_H__
#define __URL_FETCHER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class MemoryStruct
{
public:
    MemoryStruct()
    : memory(NULL)
    , size(0)
    {};

    ~MemoryStruct()
    {
        if (memory)
        {
            free(memory);
            memory = NULL;
        }
    }

    void append(void* buffer, int buffersize)
    {
        memory = (char*)realloc(memory, size + buffersize + 1);
        if (memory == NULL)
        {
          /* out of memory! */
          printf("not enough memory (realloc returned NULL)\n");
          return;
        }

        memcpy(&(memory[size]), buffer, buffersize);
        size += buffersize;
        memory[size] = 0;
    }

    char *memory;
    size_t size;
    int pos;
};

class URLFetcher
{
public:
    URLFetcher();
    ~URLFetcher();

    bool fetchURL(const char* url, MemoryStruct& mem);
    static size_t WriteMemoryCallback(void* contents,
                                      size_t size,
                                      size_t nmemb,
                                      void* userp);
private:
};

#endif
