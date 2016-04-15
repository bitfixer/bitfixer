//
//  timer.hpp
//  serialtest
//
//  Created by Michael Hill on 4/9/16.
//  Copyright © 2016 Michael Hill. All rights reserved.
//

#ifndef timer_hpp
#define timer_hpp

#include <iostream>
#include <chrono>

class Timer
{
public:
    Timer() {};
    
    void start();
    double getTime();
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
};


#endif /* timer_hpp */
