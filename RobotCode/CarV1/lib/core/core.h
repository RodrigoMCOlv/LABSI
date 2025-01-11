#ifndef CORE_H
#define CORE_H


#include <avr/interrupt.h>
#include <avr/io.h>
#include <Arduino.h> //MUST BE REMOVED

#ifndef F_CPU
#define F_CPU 16000000UL // Define CPU frequency
#endif


#define PIN_INPUT(DDR, pin)             (DDR  &= ~(1 << pin))   // Set pin as input
#define PIN_OUTPUT(DDR, pin)            (DDR  |=  (1 << pin))   // Set pin as output
#define PULL_UP_RESISTOR(PORT, pin)     (PORT |=  (1 << pin))   // Enable pull-up resistor
#define PIN_SET_HIGH(PORT, pin)         (PORT |=  (1 << pin))   // Set pin high
#define PIN_SET_LOW(PORT, pin)          (PORT &= ~(1 << pin))   // Set pin low

#endif