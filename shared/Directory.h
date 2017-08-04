#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__

#include "URLFetcher.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

class Directory
{
public:
    Directory() {};
    virtual ~Directory() {};

    void fetch(const char* dir);
    int numEntries();
    string getEntry(int index);
    string getName(int index);
    int find(const char* name);
private:
    URLFetcher urlFetcher;
    vector<string> _entries;
    vector<string> _names;
};

#endif
