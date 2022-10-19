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
#include <HAL\HAL.h>
#include <HAL\Timer.h>

#ifndef APPLICATION_H_
#define APPLICATION_H_

typedef enum
{
    Spaces,
    Reading
} _parserStates; // states for the string word parser

/* Macros */
#define MS_IN_SECONDS 1000 // milliseconds in a second
#define ASCII_SPACE 32
#define ASCII_ENTER 10

/** =================================================
 * Primary struct for the application
 */
struct _Application
{
    // Variables used in main.cpp Timers
    SWTimer watchdog_timer;
    SWTimer wait_command_timer;
    SWTimer pot_test_timer;
    SWTimer adc_settling_timer;

    uint32_t pot_ohms;
    double pot_v;
    uint8_t pot_pos;

    bool new_value_flag;
};
typedef struct _Application Application;

/** Constructs the application construct */
Application Application_construct();

/** Primary loop */
void Application_loop(Application *app_p);

/** Initializes the pins */
void InitializePins();

/** Store new potentiometer values from the pot object and real measurements */
void pollPotentiometer(Application *app_p);

/** Heatbeat of the Arduino */
void WatchdogLED(Application *app_p);

/** Prints a value with a given prefix and suffix */
void sprintln_uint(String pre, uint32_t val, String suf);

/** Prints a value with a given prefix and suffix */
void sprintln_double(String pre, double val, String suf);

/** Cycles potentiometer for testing */
void potSweep(Application *app_p);

/** Parses an input for valid commands */
void executeCommand(Application *app_p, String input);

/** Parses an input for valid commands */
String nextWord(String input, bool reset);

/** Returns true if the given string is only numeric */
bool isNumeric(String str);

#endif /* APPLICATION_H_ */