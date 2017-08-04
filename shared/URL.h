#ifndef __URL_H__
#define __URL_H__

#include <string>
using std::string;

class URL
{
public:
    URL(const char* url)
    {
        string u(url);
        init(u);
    }

    URL(const string& url)
    {
        init(url);
    }

    string getFullPath()
    {
        return _base + _path + _lastPathComponent;
    }
private:

    void init(const string& url)
    {
        printf("init: %s\n", url.c_str());
        string scheme = "http://";
        size_t found = url.find(scheme);
        if (found != string::npos)
        {
            int start = found + scheme.length();
            size_t sep_pos = url.find("/", start);
            printf("found at: %d\n", sep_pos);

            _base = url.substr(0, sep_pos);
            printf("found at: %d %s\n", sep_pos, _base.c_str());
        }
    }

    string _base;
    string _path;
    string _lastPathComponent;
};

#endif
