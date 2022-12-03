/**
 * @file HAL.h
 *
 * @brief Header containing macros for hardware components
 *
 * @ingroup default
 *
 * @author Colton Tshudy [please add your names here!]
 *
 * @version 10/12/2022
 */

/* Arduino Includes */
#include <Arduino.h>

#ifndef HAL_H_a
#define HAL_H_

// Serial recieved and end transmission characters
#define S_R_CHAR '<'        // transmission recieved char
#define S_E_CHAR '>'        // transmission terminated char
#define S_D_CHAR '['        // data begin char
#define S_HP_CHAR '!'       // high priority command char

// Pins for LEDs
#define LED_PIN 13

// Pins for 100k Potentiometer
#define CS_PIN 4
#define INC_PIN 3
#define UD_PIN 2

// Analog measurement macros
#define POT_MES_PIN A0     // potentiometer voltage monitoring pin
#define ADC_MAX 1024       // steps
#define V_POT_MAX 4.71     // V
#define ADC_SETTLE_TIME 10 // ms

// Settings for potentiometer
#define POT_MAX_R 100000 // maximum resistance of the X9C104 potentiometer

#endif /* HAL_H_ */