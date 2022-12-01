/**
 * @file main.cpp
 *
 * @brief This is the main class for DynoControl, a firmware for Arduino to
 * control a 100k digital potentiometer to act as a surrogate throttle for BOLT
 *
 * @ingroup default
 *
 * @author Colton Tshudy [please add your names here!]
 *
 * @version 12/1/2022
 */

#include <Arduino.h>
#include <Application.h>
#include <HAL\HAL.h>
#include <HAL\Timer.h>
#include <X9C10X.h>

#define VERSION 0.62 // Added serial support for Python

Application app;       // Application struct
X9C10X pot(POT_MAX_R); // Digital potentiometer

/** =================================================
 * Setup before loop
 */
void setup()
{
  // Initializes the pins
  InitializePins();

  // Begins UART communication
  Serial.begin(BAUDRATE);

  // Print version number
  String startup_message = "";
  startup_message.concat("Throttle Mapper Ver. ");
  startup_message.concat(VERSION);
  Serial.println(startup_message);

  // Constructs the application struct
  app = Application_construct();

  // Potentiometer initialization
  pot.begin(INC_PIN, UD_PIN, CS_PIN);
  pot.setPosition(0, true);
}

/** =================================================
 * Primary loop
 */
void loop()
{
  // Should blink every second, if not, the Arduino is hung
  WatchdogLED(&app);

  // Primary loop for application
  Application_loop(&app);
}

/**
 * First time setup for the Application
 */
Application Application_construct()
{
  Application app;

  // Timer initialization
  app.watchdog_timer = SWTimer_construct(MS_IN_SECONDS);
  app.pot_test_timer = SWTimer_construct(100);                 // every 0.05 seconds
  app.wait_cmd_timer = SWTimer_construct(0);                   // default initialization
  app.linear_cmd_timer = SWTimer_construct(0);                 // default initialization
  app.adc_settling_timer = SWTimer_construct(ADC_SETTLE_TIME); // ADC timer for settling time
  app.data_step_timer = SWTimer_construct(S_DATA_TIMESTEP);    // time between data logs

  app.pot_v = 0;
  app.pot_ohms = 0;
  app.pot_pos = 0;

  app.target_pos = 0;
  app.ramping_time = 0;
  app.steps = 0;

  app.new_value_flag = 0;

  app.command = "";

  app.appState = Idle;

  return app;
}

/** =================================================
 * Code executed for each loop of the application
 */
void Application_loop(Application *app_p)
{
  // Track last potentiometer position
  static uint32_t old_pot_pos = 0;

  // Poll potentiometer
  pollPotentiometer(app_p);

  // Handles serial inputs
  primaryFSM(app_p);

  // Check for change in data
  if (app_p->pot_pos != old_pot_pos)
  {
    SWTimer_start(&app_p->adc_settling_timer);
    app_p->new_value_flag = 1;
  }

  // Wait for ADC to settle before reading
  if (SWTimer_expired(&app_p->adc_settling_timer) && app_p->new_value_flag)
  {
    serialPrintData(app_p);
    app_p->new_value_flag = 0;
  }

  old_pot_pos = app_p->pot_pos;
}

/** =================================================
 * Functions
 */

/**
 * Checks if serial input to Ardino.
 * Attempts to execute a command from serial input.
 * Does not execute if the wait command is in play.
 */
void primaryFSM(Application *app_p)
{
  _appStates state = app_p->appState;

  switch (state)
  {
  case Idle:
    if(checkSerialRX(app_p))
    {
      Serial.println(S_R_CHAR);
      executeCommand(app_p, app_p->command);
      state = Executing;
    }
    break;

  case Executing:
    if (!SWTimer_expired(&app_p->wait_cmd_timer))
    {
      Serial.println("waiting...");
      state = Waiting;
      break;
    }
    if (app_p->steps != 0)
    {
      state = Linear;
      break;
    }
    state = Idle;
    break;

  case Linear:
    if (SWTimer_expired(&app_p->linear_cmd_timer))
    {
      int direction = app_p->target_pos > app_p->pot_pos ? 1 : -1;
      pot.setPosition(app_p->pot_pos + direction);
      app_p->steps--;
      SWTimer_start(&app_p->linear_cmd_timer);
    }
    if (app_p->steps == 0){
      Serial.println(S_E_CHAR);
      state = Idle;
    }
    break;

  case Waiting:
    if (SWTimer_expired(&app_p->wait_cmd_timer))
    {
      Serial.println("done.");
      Serial.println(S_E_CHAR);
      state = Idle;
    }
    break;
  }

  app_p->appState = state;
}

// Checks serial RX pin for a new command, then attempts to execute it
bool checkSerialRX(Application *app_p)
{
  if (Serial.available() > 0)
  {
    String input = Serial.readString();

    if (ECHO_EN)
      Serial.print(input); // echo

    app_p->command = input;
    return true;
  }
  return false;
}

/**
 * Polls the potentiometer object for new values, and uses the ADC to measure
 * the actual voltage at the divider created by the potentiometer
 */
void pollPotentiometer(Application *app_p)
{
  // Poll for new potentiometer values
  app_p->pot_v = double(analogRead(POT_MES_PIN)) / ADC_MAX * V_POT_MAX;
  app_p->pot_ohms = pot.getOhm();
  app_p->pot_pos = pot.getPosition();
}

/**
 * Executs a command based on the serial input string
 */
void executeCommand(Application *app_p, String input)
{
  // Return string, if needed
  String output_text = "Fail";
  String arg1 = "";
  String arg2 = "";

  input.toLowerCase();

  // Gets first char of command, and reset index
  char cmd_type = nextWord(input, 1).charAt(0);

  // Executs the command based on the char, otherwise gives error message
  switch (cmd_type)
  {
  case 't': // Linear ramp to throttle
    arg1 = nextWord(input, 0);
    arg2 = nextWord(input, 0);
    if (isNumeric(arg1)) // nested ifs are ugly, change to function calls
    {
      int target = arg1.toInt();
      if (target >= 0 && target < 100)
      { 
        if (isNumeric(arg2))
        {
          int time = arg2.toInt();
          if (time > 0)
          {
            app_p->target_pos = target;
            app_p->ramping_time = time;
            app_p->steps = abs(target - app_p->pot_pos);
            int step_time = app_p->ramping_time / (app_p->steps);
            app_p->linear_cmd_timer = SWTimer_construct(step_time);
            SWTimer_start(&app_p->linear_cmd_timer);
          }
          else
            output_text = "  Cannot use negative time";
        }
        else if (arg2.equals("NULL"))
          pot.setPosition(target);
      }
      else
          output_text = "  Throttle out of bounds";
    }
    else
      output_text = "  Bad argument for command 't'";
    break;

  case 's': // Step command
    arg1 = nextWord(input, 0);
    if (isNumeric(arg1))
    {
      int new_pos = app_p->pot_pos + arg1.toInt();
      if (new_pos >= 0 && new_pos < 100)
        pot.setPosition(new_pos);
      else
        output_text = "  Throttle out of bounds";
    }
    else
      output_text = "  Bad argument for command 's'";
    break;

  case 'w': // Wait command
    arg1 = nextWord(input, 0);
    if (isNumeric(arg1))
    {
      int time = arg1.toInt();
      if (time > 0)
      {
        app_p->wait_cmd_timer = SWTimer_construct(time);
        SWTimer_start(&app_p->wait_cmd_timer);
      }
      else
        output_text = "  Time out of bounds";
    }
    else
      output_text = "  Bad argument for command 'w'";
    break;

  case 'r': // Read potentiometer command, effectively a dump
    app_p->new_value_flag = 1;
    break;

  default:
    output_text = "  Unknown command type";
    break;
  }

  if (!output_text.equals("Fail"))
    Serial.println(output_text);
}

/**
 * Gets the next word from the string
 */
String nextWord(String input, bool reset)
{
  static unsigned int cur = 0; // cursor for string index

  if (reset)
    cur = 0;

  if (cur >= input.length())
    return "NULL";

  // Attempt to find a word
  _parserStates state = Spaces; // initial state for the parser FSM
  for (unsigned int i = cur; i < input.length(); i++)
  {
    char c = input.charAt(i);

    switch (state)
    {
    case Spaces:
      if (c == ASCII_ENTER || c == ASCII_SPACE)
        ; // stay in Spaces state
      else
      {
        cur = i; // move cursor to after spaces
        state = Reading;
      }
      break;

    case Reading:
      if (c == ASCII_ENTER || c == ASCII_SPACE)
      {
        String word = input.substring(cur, i);
        cur = i + 1;
        return word;
      }
      break;
    }
  }

  return "NULL";
}

/**
 * Sets up pin states
 */
void InitializePins()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(INC_PIN, OUTPUT);
  pinMode(UD_PIN, OUTPUT);
}

// Blinks an LED once a second as a visual indicator of processor hang
void WatchdogLED(Application *app_p)
{
  static bool state = true;

  if (SWTimer_expired(&app_p->watchdog_timer))
  {
    digitalWrite(LED_PIN, state);
    state = !state;
    SWTimer_start(&app_p->watchdog_timer);
  }
}

void sprintln_uint(String pre, uint32_t val, String suf)
{
  String output = pre + val + suf;
  Serial.println(output);
}

void serialPrintData(Application *app_p)
{
  String data = "";
  data.concat(millis());
  data.concat(", ");
  data.concat(app_p->pot_pos); // pot position
  data.concat(", ");
  data.concat(app_p->pot_ohms); // pot ohms
  data.concat(", ");
  data.concat(app_p->pot_v); // voltage at divider

  Serial.println(data);
}

void sprintln_double(String pre, double val, String suf)
{
  String output = pre + val + suf;
  Serial.println(output);
}

/**
 * Checks if a string is made up of only numeric characters
 *
 * @param str String to check
 *
 * @returns true if string is numeric, false otherwise
 */
bool isNumeric(String str)
{
  for (unsigned int i = 0; i < str.length(); i++)
  {
    if (!(isdigit(str.charAt(i)) || str.charAt(i) == '-'))
      return false;
  }
  return true;
}

/**
 * Cycles potentiometer between 0 and 99% for testing
 */
void potSweep(Application *app_p)
{
  static uint8_t count = 0;

  // For now, cycles between 0% and 99% throttle
  if (SWTimer_expired(&app_p->pot_test_timer))
  {
    pot.incr();
    Serial.println(pot.getOhm());
    SWTimer_start(&app_p->pot_test_timer);
    count++;
  }

  if (count == 99)
  {
    count = 1;
    pot.setPosition(0, true);
  }
}