#ifndef US_sensor_H
#define US_sensor_H

#include <core.h>
#include <timing.h>

struct US_distance
{
    uint16_t distance1;
    uint16_t distance2;
    uint16_t distance3;
    uint16_t distance4;
};

class US_sensor
{
    public:

        US_sensor();
        ~US_sensor();

        void init_US_sensor(uint8_t trig_pin, uint8_t echo_pin0, uint8_t echo_pin1, uint8_t echo_pin2, uint8_t echo_pin3, uint8_t mux0, uint8_t mux1);

        uint16_t US_get_reading( uint8_t num);                                  //Returns the distance in mm
        US_distance get4Readings();


        void US_test();


    private:
        uint16_t get_speed_of_sound();
        
        uint8_t  last_distance;                                     //Stores the last distance recorded 
        uint8_t  PIN_trig;                                          //The echo pin for a given US_sensor
        uint8_t  PIN_echo0;                                         //PIN in the PortB where echo is located
        uint8_t  PIN_echo1;                                         //PIN in the PortB where echo is located
        uint8_t  PIN_echo2;                                         //PIN in the PortB where echo is located
        uint8_t  PIN_echo3;                                         //PIN in the PortB where echo is located
        uint8_t  mux0;                                              //The pin 0 in the Mux selector
        uint8_t  mux1;                                              //The pin 1 in the Mux selector
                                           
};

#endif
