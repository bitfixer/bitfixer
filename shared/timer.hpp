//
//  timer.hpp
//  serialtest
//
//  Created by Michael Hill on 4/9/16.
//  Copyright © 2016 Michael Hill. All rights reserved.
//

#ifndef timer_hpp
#define timer_hpp

namespace Tools
{
    class Timer
    {
    public:
        static Timer* createTimer();
        virtual void start() = 0;
        virtual double getTime() = 0;
    };
}

#endif /* timer_hpp */
