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
 * @version 10/12/2022
 */

#include <Arduino.h>
#include <Application.h>
#include <HAL\HAL.h>
#include <HAL\Timer.h>
#include <X9C10X.h>

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
  Serial.begin(9600);

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
  app.pot_test_timer = SWTimer_construct(100);            // every 0.05 seconds
  app.pot_test_timer = SWTimer_construct(MS_IN_SECONDS);  // default initialization
  app.adc_settling_timer = SWTimer_construct(ADC_SETTLE); // ADC timer for settling time

  app.pot_v = 0;
  app.pot_ohms = 0;
  app.pot_pos = 0;

  app.new_value_flag = 0;

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

  // Check for pending serial command
  if (Serial.available() > 0 && SWTimer_expired(&app_p->wait_command_timer)) // TODO turn off serial when WAITIING since serial commands queue then execute all at once!
  {
    String input = Serial.readString();
    Serial.print(input); // echo

    executeCommand(app_p, input);
  }

  if (app_p->pot_pos != old_pot_pos)
  {
    SWTimer_start(&app_p->adc_settling_timer);
    app_p->new_value_flag = 1;
  }

  if (SWTimer_expired(&app_p->adc_settling_timer) && app_p->new_value_flag)
  {
    app_p->new_value_flag = 0;
    sprintln_uint("  New pot pos: ", app_p->pot_pos, "%");
    sprintln_uint("  New pot pos: ", app_p->pot_ohms, " Ohms");
    sprintln_double("  New throttle voltage: ", app_p->pot_v, " V");
  }

  old_pot_pos = app_p->pot_pos;
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
  String arg = "";

  input.toLowerCase();

  // Gets first char of command, and reset index
  char cmd_type = nextWord(input, 1).charAt(0);

  // Executs the command based on the char, otherwise gives error message
  switch (cmd_type)
  {
  case 't': // Throttle set command
    arg = nextWord(input, 0);
    if (isNumeric(arg))
      pot.setPosition(arg.toInt());
    else if (arg.charAt(0) == 'u')
      pot.incr();
    else if (arg.charAt(0) == 'd')
      pot.decr();
    else
      output_text = "  Bad argument for command 't': " + arg;
    break;

  case 'i': // Increment command
    arg = nextWord(input, 0);
    if (isNumeric(arg))
      pot.setPosition(app_p->pot_pos + arg.toInt());
    else
      output_text = "  Bad argument for command 'i': " + arg;
    break;

  case 'd': // Decrement command
    arg = nextWord(input, 0);
    if (isNumeric(arg))
      pot.setPosition(app_p->pot_pos - arg.toInt());
    else
      output_text = "  Bad argument for command 'd': " + arg;
    break;

  case 'w': // Wait command
    arg = nextWord(input, 0);
    if (isNumeric(arg))
    {
      app_p->wait_command_timer = SWTimer_construct(arg.toInt());
      SWTimer_start(&app_p->wait_command_timer);
    }
    else
      output_text = "  Bad argument for command 'w': " + arg;
    break;

  default:
    output_text = "  Unknown command type: " + cmd_type;
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
    if (!isdigit(str.charAt(i)))
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