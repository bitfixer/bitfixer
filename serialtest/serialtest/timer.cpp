//
//  timer.cpp
//  serialtest
//
//  Created by Michael Hill on 4/9/16.
//  Copyright © 2016 Michael Hill. All rights reserved.
//

#include "timer.hpp"
using namespace std::chrono;

void Timer::start()
{
    startTime = high_resolution_clock::now();
}

void Timer::end()
{
    endTime = high_resolution_clock::now();
}

double Timer::getTime()
{
    duration<double> dur = duration_cast<duration<double>>(endTime - startTime);
    return dur.count();
}