/*
 * Application.h
 *
 *  Created on: 4/13/2022
 *      Author: Colton Tshduy
 */

/* Arduino Includes */
#include <Arduino.h>

/* HAL Includes */
#include <HAL.h>
#include <Timer.h>

#ifndef APPLICATION_H_
#define APPLICATION_H_

/* Macros */
#define US_IN_SECONDS 1000000 // microseconds in a second

struct _Application
{
    // Holds the variables used in main.cpp
    // =========================================================================
    // Timer initialization
    SWTimer watchdog_timer;
};
typedef struct _Application Application;

/** Constructs the application construct */
Application Application_construct();

/** Primary loop */
void Application_loop(Application *app_p);

/** Initializes the pins */
void InitializePins();

/** Heatbeat of the Arduino */
void WatchdogLED(Application *app_p);

/** Prints red/inf ac/dc measurements to the screen for verification purposes */
void PrintDebug(Application *app_p);

#endif /* APPLICATION_H_ */