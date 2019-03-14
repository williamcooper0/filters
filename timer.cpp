#include "timer.h"

#include <iostream>

using namespace std::chrono;

system_clock::time_point Timer::_time;

void Timer::start()
{
    _time = high_resolution_clock::now();
}

void Timer::end()
{
    std::cerr << duration_cast<milliseconds>(high_resolution_clock::now() - _time).count() << '\n';
}
