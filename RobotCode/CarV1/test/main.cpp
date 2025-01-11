#include <Arduino.h>
#include <Robotlib.h>

// put function declarations here:
int myFunction(int, int);

volatile int result;

void setup() {
  Serial.begin(9600);  // Initialize the serial communication at 9600 baud
}

void loop() {



  
}

// put function definitions here:
int myFunction(int x, int y) {
  int var = x + y;
  Serial.println("Hello");    // Print a message each time the function is called
  return var;                 // Return the calculated sum
}
