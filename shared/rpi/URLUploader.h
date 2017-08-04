#ifndef __URL_UPLOADER_H__
#define __URL_UPLOADER_H__

#include <string>
#include "URLFetcher.h"
using std::string;

class URLUploader
{
public:
    URLUploader(const string& base);
    ~URLUploader();

    bool upload(const char* name, MemoryStruct& mem);
    static size_t ReadCallback(void* ptr,
                               size_t size,
                               size_t nmemb,
                               void* stream);
private:
    string _base;
};


#endif
