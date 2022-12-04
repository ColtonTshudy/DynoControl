/**
 * @file Application.h
 *
 * @brief Header containing macros, variables, and function initializations for
 * main.cpp
 *
 * @ingroup default
 *
 * @author Colton Tshudy
 *
 * @version 12/4/2022
 */

/* Arduino Includes */
#include <Arduino.h>
#include <X9C10X.h>

/* HAL Includes */
#include <HAL\HAL.h>
#include <HAL\Timer.h>

#ifndef APPLICATION_H_
#define APPLICATION_H_

/* Settings */
#define ECHO_EN 1
#define S_DATA_TIMESTEP 500 // ms

/* Parameters */
#define BAUDRATE 115200 // baud/s
#define S_TIMEOUT 1000  // ms between serial characters until timeout

/* Macros */
#define MS_IN_SECONDS 1000 // milliseconds in a second
#define ASCII_SPACE 32
#define ASCII_LF 10
#define ASCII_CR 13
#define CMD_CHAR_LEN 20    // maximum length of commands in chars

typedef enum
{
    Idle,
    Executing,
    Linear,
    Waiting
} _appStates; // states for the serial reader

typedef enum
{
    Spaces,
    Reading
} _parserStates; // states for the string word parser

/** =================================================
 * Primary struct for the application
 */
struct _Application
{
    // Variables used in main.cpp Timers
    SWTimer watchdog_timer;
    SWTimer wait_cmd_timer;
    SWTimer pot_test_timer;
    SWTimer adc_settling_timer;
    SWTimer linear_cmd_timer;
    SWTimer data_step_timer;
    SWTimer serial_timeout_timer;

    uint32_t pot_ohms;
    double pot_v;
    uint8_t pot_pos;
    unsigned long mes_timestamp;

    int target_pos;
    uint64_t ramping_time;
    int steps;

    _appStates appState;

    bool new_value_flag;
    bool cmd_finished_flag;
    bool cmd_high_priority;
    
    String command;
};
typedef struct _Application Application;

/** Constructs the application construct */
Application Application_construct();

/** Primary loop */
void Application_loop(Application *app_p);

/** Initializes the pins */
void InitializePins();

/** Store new potentiometer values from the pot object and real measurements */
void pollPot(Application *app_p);

/** Heatbeat of the Arduino */
void WatchdogLED(Application *app_p);

/** Serial input state handler */
void primaryFSM(Application *app_p);

/** Check serial RX and attempt to execute command */
bool checkSerialRX(Application *app_p);

/** Prints data from the application struct */
void serialPrintData(Application *app_p);

/** Cycles potentiometer for testing */
void potSweep(Application *app_p);

/** Parses an input for valid commands */
void executeCommand(Application *app_p, String input);

/** Parses an input for valid commands */
String nextWord(String input, bool reset);

/** Returns true if the given string is only numeric */
bool isNumeric(String str);

/** Prints a single char terminated by '\0'*/
void serialPrintChar(char c);

/** Raises specific flags for high priority commands */
void checkPriority(Application *app_p, char* cmd);

/** Resets the application variables and states */
void resetApplication(Application *app_p);

#endif /* APPLICATION_H_ */