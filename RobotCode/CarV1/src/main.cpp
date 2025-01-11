/*
#include <Arduino.h>
#include <Motor.h>
//#include <US_sensor.h>




//US_distance us_readings = {0, 0, 0, 0};
Motor_readings motor_readings;


void setup() 
{
  Serial.begin(9600);
  motor.Motors_init(true);

}

void loop() 
{ 
  motor.move(150);
}
*/




#include <Arduino.h>
#include <US_sensor.h>
#include <timing.h>



US_sensor us_sensor;

void setup() 
{
  Serial.begin(9600);
  
  timer_class.timer_init();
  us_sensor.init_US_sensor(1, 0, 1, 2, 3, 4, 5);



}

void loop() 
{ 
  //uint16_t dist;

  //dist = us_sensor.US_get_reading(0);

  /*
  Serial.print("Distance: ");
  Serial.println(dist);
  */ 
/*
  PIN_SET_HIGH(PORTB, 1);

  timer_class.delay_us(10);

  PIN_SET_LOW(PORTB, 1);

  Serial.print("Ticks: ");
  Serial.println(timer_class.get_ticks());

  delay(1000);
*/

  long int start, end;

  start = millis();
  timer_class.delay_10us();
  end = millis();

  Serial.print("Time: ");
  Serial.println(end - start);

}