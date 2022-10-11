/*
 * Application.h
 *
 *  Created on: 4/13/2022 Author: Colton Tshduy
 */

/* Arduino Includes */
#include <Arduino.h>
#include <X9C10X.h>


/* HAL Includes */
#include <HAL.h>
#include <Timer.h>

#ifndef APPLICATION_H_
#define APPLICATION_H_

/* Macros */
#define US_IN_SECONDS 1000000 // microseconds in a second
#define POT_MAX_R 96000 // maximum resistance of the X9C104 potentiometer

struct _Application
{
    // Holds the variables used in main.cpp
    // =========================================================================
    // Timers
    SWTimer watchdog_timer;

    // Potentiometer
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