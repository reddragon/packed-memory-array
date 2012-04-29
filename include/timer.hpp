#if !defined TIMER_HPP
#define TIMER_HPP

#include <time.h>
#include <sys/time.h>

struct Timer {
    // time_t begin;
    timeval begin;
    Timer()
    { }

    void
    start() {
        // begin = time(NULL);
        gettimeofday(&begin, NULL);
    }

    double
    stop() {
        // time_t now = time(NULL);
        // time_t diff = now - begin;
        timeval now;
        gettimeofday(&now, NULL);
        double l = begin.tv_sec * 1000000.0 + begin.tv_usec;
        double r = now.tv_sec * 1000000.0 + now.tv_usec;
        double diff = r - l;
        return diff;
    }
};

#endif // TIMER_HPP
