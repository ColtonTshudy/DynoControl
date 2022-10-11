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
 * @version 10/11/2022
 */

#include <Arduino.h>
#include <Application.h>
#include <HAL.h>
#include <X9C10X.h>

Application app;       // Application struct
X9C10X pot(POT_MAX_R); // Digital potentiometer

/**
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
  pot.begin(CS_PIN, INC_PIN, UD_PIN);
  pot.setPosition(0, true);
}

/**
 * Primary loop
 */
void loop()
{
  // Should blink every second, if not, the Arduino is hung
  WatchdogLED(&app);

  // Primary loop for application
  Application_loop(&app);
}

Application Application_construct()
{
  Application app;

  // Timer initialization
  app.watchdog_timer = SWTimer_construct(US_IN_SECONDS);

  return app;
}

void Application_loop(Application *app_p)
{
  // TODO
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