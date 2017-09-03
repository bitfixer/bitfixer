//
//  YTVideo.cpp
//  
//
//  Created by Michael Hill on 9/2/17.
//
//

#include "YTVideo.h"
#include "URLFetcher.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <regex>

namespace YouTube
{
    struct InvalidChar
    {
        bool operator()(char c) const {
            return !isprint((unsigned)c);
        }
    };
    
    std::vector<Video> search(std::string term, int page)
    {
        URLFetcher fetcher;
        MemoryStruct mem;
        
        char url[1024];
        sprintf(url, "https://www.youtube.com/results?search_query=%s&page=%d", term.c_str(), page);
        
        std::vector<Video> videos;
        
        bool ret = fetcher.fetchURL(url, mem);
        if (ret)
        {
            // parse the results
            std::stringstream ss((char*)mem.memory);
            std::string to;
            
            std::string watchStr("/watch?v=");
            std::string titleStr("title=\"");
            
            int i = 0;
            while (std::getline(ss, to, '\n'))
            {
                if (to.find(watchStr) != std::string::npos)
                {
                    if (to.find(titleStr) != std::string::npos)
                    {
                        // get yt id
                        std::string::size_type n;
                        n = to.find(watchStr);
                        std::string rem = to.substr(n);
                        std::string::size_type end;
                        end = rem.find("\"");
                        
                        int len = end - watchStr.length();
                        std::string yt_id = to.substr(n + watchStr.length(), len);
                        
                        n = to.find(titleStr);
                        n += titleStr.length();
                        rem = to.substr(n);
                        end = rem.find("\"");
                        
                        std::string title = to.substr(n, end);
                        title.erase(std::remove_if(title.begin(), title.end(), InvalidChar()), title.end());
                        
                        title = std::regex_replace(title, std::regex("&amp;"), "&");
                        title = std::regex_replace(title, std::regex("&#39;"), "'");
                        // create new video and add to response
                        videos.push_back(Video(yt_id, title));
                    }
                }
                i++;
            }
        }
        else
        {
            printf("failed.\n");
        }
        
        return videos;
    }
}
