/*
 * Timer.h
 *
 *  Created on: 4/15/2022
 *      Author: Colton Tshduy
 */

/* Arduino Driver Includes */
#include <Arduino.h>

#ifndef TIMER_H_
#define TIMER_H_

struct _SWTimer
{
    // The number of microseconds which must elapse before the timer expires
    uint64_t waitTime_ms;

    // The starting counter value of the hardware timer, set when the timer is started
    uint32_t startCounter;
};
typedef struct _SWTimer SWTimer;

// Constructs a Software timer. All timers must be constructed before starting them.
SWTimer SWTimer_construct(uint64_t waitTime_ms);

// Starts a software timer. All constructed timers must be started before use.
void SWTimer_start(SWTimer *timer);

// A helper function used for determining how many cycles have elapsed since the
// timer was started. You do not need to call this function outside of Timer.c.
uint64_t SWTimer_elapsedTime(SWTimer *timer);

// Returns true if the timer has expired, and false otherwise
bool SWTimer_expired(SWTimer *timer);

// Elapsed percentage of time on current timer
double SWTimer_percentElapsed(SWTimer *timer_p);

#endif /* TIMER_H_ */