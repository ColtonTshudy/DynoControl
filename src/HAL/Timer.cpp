/*
 * Timer.c
 *
 *  Created on: 4/15/2022
 *      Author: Colton Tshudy
 */

#include <HAL\Timer.h>

// Construct a new timer with a wait time in milliseconds
SWTimer SWTimer_construct(uint64_t waitTime)
{
    SWTimer timer;

    timer.startCounter = 0;
    timer.waitTime_ms = waitTime;

    return timer;
}

// Start a timer
void SWTimer_start(SWTimer *timer_p)
{
    timer_p->startCounter = millis();
}

// Returns number of elapsed milliseconds
uint64_t SWTimer_elapsedTimeMS(SWTimer *timer_p)
{
    uint64_t elapsed_us = millis() - timer_p->startCounter;

    return elapsed_us;
}

// Returns true if the timer is expired
bool SWTimer_expired(SWTimer *timer_p)
{
    uint64_t elapsed_ms = SWTimer_elapsedTimeMS(timer_p);
    return elapsed_ms >= timer_p->waitTime_ms;
}

/**
 * Determines the progress percentage of time expired. A timer starts off at zero percent progress.
 * If, say, a timer needed to wait 10000 ms and 7000 ms have elapsed already since the timer
 * was started, the percentage returned is 0.7. For any timer which has already expired or which was
 * never started, the percentage returned is 1.0.
 *
 * @param timer_p:    The target timer used in determining the percent progress elapsed
 * @return the percentage of time which has elapsed since the timer was started.
 */
double SWTimer_percentElapsed(SWTimer *timer_p)
{
    if (timer_p->waitTime_ms == 0)
    {
        return 1.0;
    }

    uint64_t elapsed_ms = SWTimer_elapsedTimeMS(timer_p);

    double result = (double)elapsed_ms / (double)timer_p->waitTime_ms;

    if (result > 1.0)
    {
        return 1.0;
    }

    return result;
}
