//
//  YTVideo.h
//  
//
//  Created by Michael Hill on 9/2/17.
//
//

#ifndef YTVideo_h
#define YTVideo_h

#include <stdio.h>
#include <string>
#include <vector>

namespace YouTube
{
    class Video
    {
    public:
        Video(std::string id, std::string title)
        : _id(id)
        , _title(title)
        {};
        
        ~Video() {};
        
        std::string id()
        {
            return _id;
        }
        
        std::string title()
        {
            return _title;
        }
        
    private:
        std::string _id;
        std::string _title;
    };
    
    
    std::vector<Video> search(std::string term, int page);
};

#endif /* YTVideo_h */
