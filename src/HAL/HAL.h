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

#ifndef HAL_H_
#define HAL_H_

// General HAL Macros
#define BAUDRATE 9600

// --Pin Macros-- Pins for LEDs
#define LED_PIN 13

// Pins for 100k Potentiometer
#define CS_PIN 4
#define INC_PIN 3
#define UD_PIN 2

// Analog measurement macros
#define POT_MES_PIN A0
#define ADC_MAX 1024
#define V_POT_MAX 5

// Settings for potentiometer
#define POT_MAX_R 100000 // maximum resistance of the X9C104 potentiometer


#endif /* HAL_H_ */