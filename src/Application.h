/**
 * @file Application.h
 *
 * @brief Header containing macros, variables, and function initializations for
 * main.cpp
 *
 * @ingroup default
 *
 * @author Colton Tshudy [please add your names here!]
 *
 * @version 10/12/2022
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

/** =================================================
 * Primary struct for the application
 */
struct _Application
{
    // Variables used in main.cpp Timers
    SWTimer watchdog_timer;
    SWTimer pot_test_timer;
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

/** Prints a value with a given prefix and suffix */
void sprintln(String pre, uint32_t Val, String suf);

/** Cycles potentiometer for testing */
void potSweep(Application *app_p);

#endif /* APPLICATION_H_ */