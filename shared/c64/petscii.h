//
//  petscii.h
//  
//
//  Created by Michael Hill on 9/3/17.
//
//

#ifndef petscii_h
#define petscii_h

#include <string>

namespace Petscii {
    
    std::string asciiToPetscii(std::string str)
    {
        char* strout = new char[str.length() + 1];
        strout[str.length()] = 0;
        
        const char* instr = str.c_str();
        for (int c = 0; c < str.length(); c++)
        {
            // flip upper and lowercase
            if (instr[c] >= 'A' && instr[c] <= 'Z')
            {
                strout[c] = tolower(instr[c]);
            }
            else if (instr[c] >= 'a' && instr[c] <= 'z')
            {
                strout[c] = toupper(instr[c]);
            }
            else
            {
                strout[c] = instr[c];
            }
        }
        
        std::string ret(strout);
        delete(strout);
        printf("result string: %s len %d\n", ret.c_str(), ret.length());
        return ret;
    }
}

#endif /* petscii_h */
