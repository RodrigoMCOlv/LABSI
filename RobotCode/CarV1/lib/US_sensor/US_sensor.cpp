#include <US_sensor.h>

US_sensor:: US_sensor( ){ };
US_sensor::~US_sensor( ){ };


//TRANSFERED PINS
void US_sensor::init_US_sensor(uint8_t trig_pin, uint8_t echo_pin0, uint8_t echo_pin1, uint8_t echo_pin2, uint8_t echo_pin3, uint8_t mux0, uint8_t mux1) 
{
    this->PIN_trig = trig_pin;
    this->PIN_echo0 = echo_pin0;
    this->PIN_echo1 = echo_pin1;
    this->PIN_echo2 = echo_pin2;
    this->PIN_echo3 = echo_pin3;

    this->mux0 = mux0;
    this->mux1 = mux1;

    // Set trigger as output
    PIN_OUTPUT(DDRB, trig_pin);    // Set trigPin as OUTPUT


    // Set echos as inputs with pull-up resistor enabled

    PIN_OUTPUT(DDRB, mux0);       //The pin 0 in the Mux selector
    PIN_OUTPUT(DDRB, mux1);       //The pin 1 in the Mux selector

    PIN_INPUT(DDRC, PIN_echo0);             // Set echoPin0 as INPUT
    PULL_UP_RESISTOR(PORTC, PIN_echo0);     // Enable pull-up resistor
    
    PIN_INPUT(DDRC, PIN_echo1);             // Set echoPin1 as INPUT
    PULL_UP_RESISTOR(PORTC, PIN_echo1);     // Enable pull-up resistor
        
    PIN_INPUT(DDRC, PIN_echo2);             // Set echoPin2 as INPUT
    PULL_UP_RESISTOR(PORTC, PIN_echo2);     // Enable pull-up resistor
    
    PIN_INPUT(DDRC, PIN_echo3);             // Set echoPin3 as INPUT
    PULL_UP_RESISTOR(PORTC, PIN_echo3);     // Enable pull-up resistor

}
    
uint16_t US_sensor::get_speed_of_sound() {return 343;}

uint16_t US_sensor::US_get_reading(uint8_t num) 
{   
    /**************************************/
    //Takes a reading from a given US_sensor
    //INPUT:
    //  num: The number of the US_sensor to take a reading from
    //OUTPUT:
    //  distance: The distance in mm
    /**************************************/

    if(num == 0)
    {
        PIN_SET_LOW(PORTB, mux0);           //The pin 0 in the Mux selector
        PIN_SET_LOW(PORTB, mux1);           //The pin 1 in the Mux selector
    }

    if(num == 1)
    {
        PIN_SET_HIGH(PORTB, mux0);          //The pin 0 in the Mux selector
        PIN_SET_LOW(PORTB, mux1);           //The pin 1 in the Mux selector
    }  

    if(num == 2)
    {
        PIN_SET_LOW(PORTB, mux0);           //The pin 0 in the Mux selector
        PIN_SET_HIGH(PORTB, mux1);          //The pin 1 in the Mux selector
    }

    if(num == 3)    
    {
        PIN_SET_HIGH(PORTB, mux0);          //The pin 0 in the Mux selector
        PIN_SET_HIGH(PORTB, mux1);          //The pin 1 in the Mux selector
    }


    // Reset timer ticks and initialize variables
    timer_class.reset_ticks();
    uint16_t speed_of_sound = get_speed_of_sound();             // Get speed of sound in cm/us
    volatile uint32_t timeout = 4 * 100000 / speed_of_sound;    // Maximum waiting time

    // Send a 10us pulse to trigger the sensor
    PIN_SET_HIGH(PORTB, PIN_trig);
    
    timer_class.delay_10us(); // Wait for 10 microseconds   

    PIN_SET_LOW(PORTB, PIN_trig);
    

    // Start timer to measure how long it takes for the echo to go high

    timer_class.start_timer();
    

    while((PINC & (1 << num)) == 0 and timer_class.get_ticks() < timeout){Serial.print((PINC & (1 << num)));};

    while((PINC & (1 << num)) != 0 and timer_class.get_ticks() < timeout){Serial.print((PINC & (1 << num)));};
    
    Serial.print((PINC & (1 << num)));

    // Stop the timer as soon as the echo pin goes low
    timer_class.stop_timer();

    // Calculate the distance based on time measured while echo pin was high
    /*
    Serial.println();       
    Serial.print("Ticks: ");
    Serial.println(timer_class.get_ticks());
    */
   
    return timer_class.get_ticks()* speed_of_sound / 200; // Return distance in mm
    
}

US_distance US_sensor::get4Readings()
{   
    PIN_SET_LOW(PORTB, 5);
    PIN_SET_LOW(PORTB, 4);
    uint16_t dist0 = US_get_reading(0);

    PIN_SET_HIGH(PORTB, 5);
    PIN_SET_LOW(PORTB, 4);
    uint16_t dist1 = US_get_reading(1);

    PIN_SET_LOW(PORTB, 5);
    PIN_SET_HIGH(PORTB, 4);
    uint16_t dist2 = US_get_reading(2);

    PIN_SET_HIGH(PORTB, 5);
    PIN_SET_HIGH(PORTB, 4);
    uint16_t dist3 = US_get_reading(3);  

    US_distance readings = {dist0, dist1, dist2, dist3};
    return readings;
}

void US_sensor::US_test()
{
    

    uint16_t duration;
    uint16_t distance;



    digitalWrite(9, LOW);  
    PIN_SET_LOW(PORTB, 1);
    delayMicroseconds(2);
    PIN_SET_HIGH(PORTB, 1);

    delayMicroseconds(10); 

    PIN_SET_LOW(PORTB, 1);  

    // Measure the time it takes for the pulse to return
    // The Echo pin changes based on the selected channel
    int echoPin = A0;  // A0 for channel 0, A1 for channel 1, etc.

    // Measure the duration of the pulse on the selected Echo pin
    duration = pulseIn(echoPin, HIGH);

    // Calculate distance in cm (duration * 0.034 / 2)
    distance = duration * 0.034 / 2;

    // Print the sensor number and corresponding distance to the Serial Monitor
    Serial.print("Sensor: A");
    Serial.print(A0);  // Indicate which sensor (A0, A1, A2, or A3)
    Serial.print(" - Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

}