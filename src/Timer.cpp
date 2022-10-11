/*
 * Timer.c
 *
 *  Created on: 4/15/2022
 *      Author: Colton Tshudy
 */

#include <Timer.h>

// Construct a new timer with a wait time in microseconds
SWTimer SWTimer_construct(uint64_t waitTime)
{
    SWTimer timer;

    timer.startCounter = 0;
    timer.waitTime_us = waitTime;

    return timer;
}

// Start a timer
void SWTimer_start(SWTimer *timer_p)
{
    timer_p->startCounter = micros();
}

// Returns number of elapsed microseconds
uint64_t SWTimer_elapsedTimeUS(SWTimer *timer_p)
{
    uint64_t elapsed_us = micros()-timer_p->startCounter;

    return elapsed_us;
}

// Returns true if the timer is expired
bool SWTimer_expired(SWTimer *timer_p)
{
    uint64_t elapsed_us = SWTimer_elapsedTimeUS(timer_p);
    return elapsed_us >= timer_p->waitTime_us;
}

/**
 * Determines the progress percentage of time expired. A timer starts off at zero percent progress.
 * If, say, a timer needed to wait 10000 us and 7000 us have elapsed already since the timer
 * was started, the percentage returned is 0.7. For any timer which has already expired or which was
 * never started, the percentage returned is 1.0.
 *
 * @param timer_p:    The target timer used in determining the percent progress elapsed
 * @return the percentage of time which has elapsed since the timer was started.
 */
double SWTimer_percentElapsed(SWTimer *timer_p)
{
    if (timer_p->waitTime_us == 0)
    {
        return 1.0;
    }

    uint64_t elapsed_us = SWTimer_elapsedTimeUS(timer_p);

    double result = (double) elapsed_us / (double) timer_p->waitTime_us;

    if (result > 1.0)
    {
        return 1.0;
    }

    return result;
}
