//
//  StringTools.h
//  imconv
//
//  Created by Michael Hill on 3/19/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#ifndef StringTools_h
#define StringTools_h

#include <list>
#include <string>
#include <string.h>

using std::list;
using std::string;

class StringTools
{
public:
    static void Split(const char* str, const char* delimiters, list<string>& v)
    {
        char* temp = (char*)malloc(strlen(str) + 1);
        strcpy(temp, str);
        char* part = NULL;
        part = strtok(temp, delimiters);
        while (part != NULL)
        {
            v.push_back(string(part));
            part = strtok(NULL, delimiters);
        }
        
        free(temp);
    }
};

#endif /* StringTools_h */
