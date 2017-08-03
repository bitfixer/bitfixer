//
//  timer.cpp
//  serialtest
//
//  Created by Michael Hill on 4/9/16.
//  Copyright © 2016 Michael Hill. All rights reserved.
//

#include "timer.hpp"
#include <iostream>
#include <chrono>

using namespace std::chrono;
using namespace Tools;

class TimerImpl : public Timer
{
public:
    TimerImpl() {};
    void start()
    {
        startTime = high_resolution_clock::now();
    }
    
    double getTime()
    {
        std::chrono::high_resolution_clock::time_point now = high_resolution_clock::now();
        duration<double> dur = duration_cast<duration<double> >(now - startTime);
        return dur.count();
    }
    
private:
    high_resolution_clock::time_point startTime;
};

Timer::Timer()
: _timerImpl(NULL)
{
}

Timer::~Timer()
{
    if (_timerImpl)
    {
        delete _timerImpl;
        _timerImpl = NULL;
    }
}

void Timer::start()
{
    if (!_timerImpl)
    {
        _timerImpl = new TimerImpl();
    }
    _timerImpl->start();
}

double Timer::getTime()
{
    if (_timerImpl)
    {
        return _timerImpl->getTime();
    }
    return 0.0;
}
