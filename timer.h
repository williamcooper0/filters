#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer
{
public:
    static void start();
    static void end();

private:
    static std::chrono::system_clock::time_point _time;
};

#endif // TIMER_H
