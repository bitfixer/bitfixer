#include "Directory.h"
#include "StringSplitter.h"
#include "picojson.h"
#include <algorithm>
#include <string>

using std::string;

void Directory::fetch(const char* dir)
{
    // clear original entries
    _entries.clear();
    _names.clear();

    MemoryStruct urlResponse;
    if (urlFetcher.fetchURL(dir, urlResponse))
    {
        //printf("dir got %d bytes %s\n", urlResponse.size, urlResponse.memory);
    }

    string json(urlResponse.memory, urlResponse.size);
    picojson::value v;
    string err = picojson::parse(v, json);

    if (v.is<picojson::array>())
    {
        printf("is array!\n");
        const picojson::array& arr = v.get<picojson::array>();
        picojson::array::const_iterator it;
        int i = 0;
        for (it = arr.begin(); it != arr.end(); ++it)
        {
            printf("%d\n", i++);
            const picojson::object& obj = it->get<picojson::object>();
            const string& name = obj.find("name")->second.get<string>();
            const string& path = obj.find("path")->second.get<string>();
            printf("name: %s\npath: %s\n", name.c_str(), path.c_str());

            string n = name;
            n.erase(remove_if(n.begin(), n.end(), isspace), n.end());
            std::transform(n.begin(), n.end(), n.begin(), ::tolower);

            _entries.push_back(path);
            _names.push_back(n);
        }
    }
}

int Directory::numEntries()
{
    return _entries.size();
}

string Directory::getEntry(int index)
{
    return _entries[index];
}

string Directory::getName(int index)
{
    return _names[index];
}

int Directory::find(const char* name)
{
    string n(name);
    printf("finding %s\n", name);
    n.erase(remove_if(n.begin(), n.end(), isspace), n.end());
    std::transform(n.begin(), n.end(), n.begin(), ::tolower);

    int i;
    for (i = 0; i < _names.size(); i++)
    {
        if (n.compare(_names[i]) == 0)
        {
            // found the entry
            break;
        }
    }

    if (i < _names.size())
    {
        return i;
    }

    return -1;
}
