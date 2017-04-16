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
        Timer();
        ~Timer();
        virtual void start();
        virtual double getTime();
    private:
        Timer* _timerImpl;
    };
}

#endif /* timer_hpp */
