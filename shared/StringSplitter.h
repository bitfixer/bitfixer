#ifndef __STRING_SPLITTER_H__
#define __STRING_SPLITTER_H__

#include <string>
using std::string;

class StringSplitter
{
public:
    StringSplitter(const char* str)
    : _delim(0)
    , _index(0)
    , _str(str)
    {};

    StringSplitter(const string& str)
    : _delim(0)
    , _index(0)
    , _str(str)
    {};

    void split(unsigned char delim)
    {
        _delim = delim;
    }

    string getNextChunk()
    {
        int splitIndex;
        for (splitIndex = _index+1; splitIndex < _str.length(); splitIndex++)
        {
            if (_delim == _str.at(splitIndex))
            {
                break;
            }
        }

        if (splitIndex >= _str.length())
        {
            return string();
        }

        int pos = _index+1;
        int count = splitIndex - pos;
        _index = splitIndex;
        return _str.substr(pos, count);
    }

private:
    string _str;
    unsigned char _delim;
    int _index;
};

#endif
