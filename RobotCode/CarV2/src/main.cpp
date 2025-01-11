#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/io.h>


// #region DEFINES

//MACROS
#define PIN_INPUT(DDR, pin)             (DDR  &= ~(1 << pin))   // Set pin as input
#define PIN_OUTPUT(DDR, pin)            (DDR  |=  (1 << pin))   // Set pin as output
#define PULL_UP_RESISTOR(PORT, pin)     (PORT |=  (1 << pin))   // Enable pull-up resistor
#define PIN_SET_HIGH(PORT, pin)         (PORT |=  (1 << pin))   // Set pin high
#define PIN_SET_LOW(PORT, pin)          (PORT &= ~(1 << pin))   // Set pin low

//DEFINES
#define mux0 5
#define mux1 4

//Global variables

//MOTORS
const volatile uint16_t stepsPerRev = 372;

const volatile uint16_t mmPerRev = 250;
const volatile int16_t stepDeltaPerDeg = 7; 

volatile int16_t stepsL = 0;
volatile int16_t stepsR = 0;

uint16_t DistanceToMove = 0;
int16_t AngleToMove = 0;

uint32_t Hz = 0;


//TIMER
volatile uint32_t ticks = 0;

// Function declarations START
//INITS
void initTimersandInterrupts();
void initPins();

//TIMER
void TimerIncrementTicks();
void TimerStart();
void TimerStop();
void TimerResetTicks();
void TimerDelayTensOfus(uint32_t delay);
//US SENSOR
uint16_t USSensorGetReading(uint8_t num);
uint8_t  USSensorGetSpeedOfSound();


//MOTORS
void MotorResetSteps();
void MotorSetSpeed(int16_t percentage, char motor);
void MotorMove(uint16_t distanceToMove);
void MotorRotate(int16_t hundredsOfDeg);

// Function declarations END


// #endregion


//INTERRUPTS
ISR(INT0_vect) {
  // Handle the interrupt from the left Motor
  stepsL ++;
}
ISR(INT1_vect) {
  // Handle the interrupt from the left Motor
  stepsR ++;
}


//TIMER 
ISR(TIMER1_COMPA_vect)                                                  //Interrupt handler for Timer 1
{
    TimerIncrementTicks();

    Hz++;

    if (Hz < 100000)                     // 0.5s LOW
    {
     PIN_SET_LOW(PORTD, 7);               // Set PD7 low  
    }
    else if (Hz < 200000)                     // The other 0.5s HIGH
    {
      PIN_SET_HIGH(PORTD, 7);              // Set PD7 high
    }
    else Hz = 0;
}




void setup() 
{
  Serial.begin(9600);

  initTimersandInterrupts();
  initPins();

  TimerStart();
}

void loop() 
{
  
  while (Serial.available() == 0) 
    {
        // Do nothing, just wait
    }

  if (Serial.available() > 0) 
{
    String dataFromESP32 = Serial.readStringUntil('\n'); // Read data sent by ESP32
    dataFromESP32.trim(); // Remove any leading/trailing whitespace or newline characters

    // Ensure the string is correctly formatted
    if (dataFromESP32.startsWith("[") && dataFromESP32.endsWith("]")) 
    {
        // Remove the square brackets
        dataFromESP32 = dataFromESP32.substring(1, dataFromESP32.length() - 1);
        
        // Find the comma position
        int commaIndex = dataFromESP32.indexOf(',');
        if (commaIndex != -1) 
        {
            // Extract the substrings for distance and angle
            String distanceString = dataFromESP32.substring(0, commaIndex);
            String angleString = dataFromESP32.substring(commaIndex + 1);

            // Convert to numbers
            DistanceToMove = distanceString.toInt();
            AngleToMove = angleString.toInt();

            DistanceToMove = (uint16_t)DistanceToMove;
            AngleToMove = (int16_t)AngleToMove;
        }
    }

}


  //USING THE MOTORS

  //MotorMove(134);

  //delay(1000000);
  
  //USING THE US SENSOR

  uint16_t distance0 = 0, distance1 = 0, distance2 = 0, distance3 = 0;

  MotorRotate(AngleToMove);
  TimerDelayTensOfus(50000);

  MotorMove(DistanceToMove);
  TimerDelayTensOfus(50000);
  
  distance0 = USSensorGetReading(0);
  TimerDelayTensOfus(1000);


  distance1 = USSensorGetReading(1);
  TimerDelayTensOfus(1000);
  	
  distance2 = USSensorGetReading(2);
  TimerDelayTensOfus(1000);

  distance3 = USSensorGetReading(3);
  TimerDelayTensOfus(1000);


  Serial.print("[");
  Serial.print(DistanceToMove);
  Serial.print(",");
  Serial.print(AngleToMove);
  Serial.print(",");
  Serial.print(distance0);
  Serial.print(",");
  Serial.print(distance1);
  Serial.print(",");
  Serial.print(distance2);
  Serial.print(",");
  Serial.print(distance3);
  Serial.println("]");
  TimerDelayTensOfus(1000);


}


// #region INITS

//INITS
void initTimersandInterrupts() 
{

/*-------------------TIMER 0------------------*/

  TCCR0A =    (1 << COM0A1)|          // Clear OC0A on Compare Match, set OC0A at BOTTOM,(non-inverting mode).
              (0 << COM0A0)|          // Clear OC0A on Compare Match, set OC0A at BOTTOM,(non-inverting mode).
              (0 << COM0B1)|          // Normal port operation, OC0B disconnected.
              (0 << COM0B0)|          // Normal port operation, OC0B disconnected.
              (0 << WGM01 )|          // Phase Correct PWM mode
              (1 << WGM00 );          // Phase Correct PWM mode

  TCCR0B =    (0 << FOC0A )|          // This bit must be set to zero when TCCR0B is written when operating in PWM mode
              (0 << FOC0B )|          // This bit must be set to zero when TCCR0B is written when operating in PWM mode
              (0 << WGM02 )|          // Phase Correct PWM mode
              (0 << CS02  )|          // Prescaler 64, will give a 976Hz pwm ()
              (1 << CS01  )|          // Prescaler 64, will give a 976Hz pwm
              (1 << CS00  );          // Prescaler 64, will give a 976Hz pwm

  OCR0A = 0;                          // The pwm for R motor starts at 0%

/*-------------------TIMER 1------------------*/
  
  TCCR1A =    (0 << COM1A1)|                                  //Normal port operation, OC1A disconnected.                        
              (0 << COM1A0)|                                  //Normal port operation, OC1A disconnected. 
              (0 << COM1B1)|                                  //Normal port operation, OC1B disconnected.    
              (0 << COM1B0)|                                  //Normal port operation, OC1B disconnected.   
              (0 << WGM11 )|                                  //Set to CTC top at OCR1A (mode 4)
              (0 << WGM10 );                                  //Set to CTC top at OCR1A (mode 4)


  TCCR1B =    (1 << WGM12 )|                                  //Set to CTC top at OCR1A (mode 4)
              (0 << WGM13 )|                                  //Set to CTC top at OCR1A (mode 4)
              (0 << CS12  )|                                  //clkI/O /8 (From prescaler)
              (1 << CS11  )|                                  //clkI/O /8 (From prescaler)
              (0 << CS10  );                                  //clkI/O /8 (From prescaler)

  //TCCR1C =    (1 << FOC1A );                                //Bit 7 – FOC1A: Force Output Compare for Channel A

  OCR1A  =                19;                                 //With a prescaler of 8 and a 10us tick the OCR1A should be set to 19

  TIMSK1 =    (0 << OCIE1A);                                  //Timer/Counter1, Output Compare A Match Interrupt Disable, this is here for future use

/*-------------------TIMER 2------------------*/

  TCCR2A =    (1 << COM2A1)|          // Clear OC2A on Compare Match, set OC2A at BOTTOM,(non-inverting mode).
              (0 << COM2A0)|          // Clear OC2A on Compare Match, set OC2A at BOTTOM,(non-inverting mode).
              (0 << COM2B1)|          // Normal port operation, OC2B disconnected.
              (0 << COM2B0)|          // Normal port operation, OC2B disconnected.
              (0 << WGM21 )|          // Phase Correct PWM mode
              (1 << WGM20 );          // Phase Correct PWM mode

  TCCR2B =    (0 << FOC2A )|          // This bit must be set to zero when TCCR2B is written when operating in PWM mode
              (0 << FOC2B )|          // This bit must be set to zero when TCCR2B is written when operating in PWM mode
              (0 << WGM22 )|          // Phase Correct PWM mode
              (1 << CS22  )|          // Prescaler 64, will give a 976Hz pwm 
              (0 << CS21  )|          // Prescaler 64, will give a 976Hz pwm
              (0 << CS20  );          // Prescaler 64, will give a 976Hz pwm

  OCR2A = 0;                          // The pwm for L motor starts at 0%  

//-------------------Interrupts------------------*/

  EICRA =     (1 << ISC11 )|          //Trigger on rising edge
              (1 << ISC10 )|          //Trigger on rising edge
              (1 << ISC01 )|          //Trigger on rising edge
              (1 << ISC00 );          //Trigger on rising edge

  EIMSK =     (1 << INT1 )|           //Enable External Interrupt 1
              (1 << INT0 );           //Enable External Interrupt 0



sei();



}

void initPins()
{
  //US SENSOR

  PIN_OUTPUT(DDRB, 1);            // Set PB1 as OUTPUT

  PIN_OUTPUT(DDRB, mux0);         //The pin 0 in the Mux selector
  PIN_OUTPUT(DDRB, mux1);         //The pin 1 in the Mux selector

  PIN_INPUT(DDRC, 0);             // Set PCO as INPUT
  PULL_UP_RESISTOR(PORTC, 0);     // Enable pull-up resistor
    
  PIN_INPUT(DDRC, 1);             // Set PC1 as INPUT
  PULL_UP_RESISTOR(PORTC, 1);     // Enable pull-up resistor
        
  PIN_INPUT(DDRC, 2);             // Set PC2 as INPUT
  PULL_UP_RESISTOR(PORTC, 2);     // Enable pull-up resistor
    
  PIN_INPUT(DDRC, 3);             // Set PC3 as INPUT
  PULL_UP_RESISTOR(PORTC, 3);     // Enable pull-up resistor

  //MOTORS
  
  PIN_OUTPUT(DDRD, 6);            // Set PD6 as OUTPUT
  PIN_OUTPUT(DDRB, 3);            // Set PB3 as OUTPUT

  PIN_INPUT(DDRD, 2);             // Set PD2 as INPUT
  PULL_UP_RESISTOR(PORTD, 2);     // Enable pull-up resistor
  
  PIN_INPUT(DDRD, 3);             // Set PD3 as INPUT
  PULL_UP_RESISTOR(PORTD, 3);     // Enable pull-up resistor

  //LED

  PIN_OUTPUT(DDRD, 7);            // Set PD7 as OUTPUT


}

// #endregion



// #region TIMER

//TIMER
void TimerIncrementTicks(){ticks ++;}         //Increments the number of ticks
void TimerStart(){TIMSK1 |= (1 << OCIE1A);}   //Turns on the timer
void TimerStop(){TIMSK1 &= ~(1 << OCIE1A);}   //Turns off the timer
void TimerResetTicks(){ticks = 0;}            //Resets the number of ticks
void TimerDelayTensOfus(uint32_t delay)
{
  TimerResetTicks();
  
  //TimerStart();
  while(ticks < delay);

  //TimerStop();

  TimerResetTicks();
}
// #endregion



// #region US SENSOR

//US SENSOR
uint16_t USSensorGetReading(uint8_t num)
{

  if (num == 0)
  { 
    PIN_SET_LOW(PORTB, mux0);           //The pin 0 in the Mux selector
    PIN_SET_LOW(PORTB, mux1);           //The pin 1 in the Mux selector
  }
  else if (num == 1)
  {
    PIN_SET_HIGH(PORTB, mux0);          //The pin 0 in the Mux selector
    PIN_SET_LOW(PORTB,  mux1);           //The pin 1 in the Mux selector
  }
  else if (num == 2)
  {
    PIN_SET_LOW(PORTB,  mux0);           //The pin 0 in the Mux selector
    PIN_SET_HIGH(PORTB, mux1);          //The pin 1 in the Mux selector
  }
  else if (num == 3)
  {
    PIN_SET_HIGH(PORTB, mux0);          //The pin 0 in the Mux selector
    PIN_SET_HIGH(PORTB, mux1);          //The pin 1 in the Mux selector
  }

  
  uint16_t speed_of_sound = 343;
  uint16_t timeout = 4 * 100000 / speed_of_sound;    // Maximum waiting time


  TimerResetTicks();
  // Send a 10us pulse to trigger the sensor
  PIN_SET_HIGH(PORTB, 1);

  //TimerStart();
  while (ticks < 2);

  PIN_SET_LOW(PORTB, 1);

  //TimerStop();
  TimerResetTicks();
  
  //TimerStart();

  while((PINC & (1 << num)) == 0 and ticks < timeout){};

  //TimerStop();
  TimerResetTicks();

  //TimerStart();   

  while((PINC & (1 << num)) != 0 and ticks < timeout){};
  
  //TimerStop();



  if (ticks >= timeout) return 0; // Return 0 if timeout

  return ticks * speed_of_sound / 200; // Return distance in mm


}

// #endregion


// #region MOTORS

//MOTORS
void MotorResetSteps()
{
    stepsL = 0;
    stepsR = 0;
}

void MotorSetSpeed(int16_t percentage, char motor)
{   //Input percentage from 0 to 100
    //input motor 'R' or 'L'
    if (percentage > 100) percentage = 100; 
    if (percentage < 0) percentage = 0;

    int16_t pwm = (int)255*percentage/100.0;


    if (motor == 'L') 
    {
        OCR0A = pwm;
    }
    if (motor == 'R')
    {
        OCR2A = pwm;
    }
}

void MotorMove(uint16_t distanceToMove)
{
    if (distanceToMove == 0) return;

    int16_t StepstoMove = 0;
    int16_t pwmPercentR = 100;
    int16_t pwmPercentL = 100;
    StepstoMove = distanceToMove * stepsPerRev / mmPerRev;
    MotorResetSteps();

    while(pwmPercentR != 0 || pwmPercentL != 0)
    {

        pwmPercentR = (int16_t)(0.04*(StepstoMove - stepsR) + 30);
        pwmPercentL = (int16_t)(0.04*(StepstoMove - stepsL) + 30);  

        if (pwmPercentR > 100) pwmPercentR = 100;
        if (pwmPercentL > 100) pwmPercentL = 100;
        if (pwmPercentR < 30) pwmPercentR = 0;
        if (pwmPercentL < 30) pwmPercentL = 0;

        MotorSetSpeed(pwmPercentR, 'R');
        MotorSetSpeed(pwmPercentL, 'L');

    }

    MotorSetSpeed(0, 'R');
    MotorSetSpeed(0, 'L');
}

void MotorRotate(int16_t degToMove)
{

    if (degToMove < 0)
    {

      int16_t StepstoMove = 0;
      int16_t pwmPercentL = 100;

      StepstoMove = degToMove* stepDeltaPerDeg;

      MotorResetSteps();

      while(pwmPercentL != 0 )
      {

          pwmPercentL = (int16_t)(0.4*(StepstoMove - stepsR) + 30);

          if (pwmPercentL > 100) pwmPercentL = 100;
          if (pwmPercentL < 30) pwmPercentL = 0;


          MotorSetSpeed(pwmPercentL, 'R');
      }

      MotorSetSpeed(0, 'L');
    }

    if (degToMove > 0)
    {

      int16_t StepstoMove = 0;
      int16_t pwmPercentR = 100;

      StepstoMove = degToMove* stepDeltaPerDeg;

      MotorResetSteps();

      while(pwmPercentR != 0 )
      {

          pwmPercentR = (int16_t)(0.4*(StepstoMove - stepsR) + 30);

          if (pwmPercentR > 100) pwmPercentR = 100;
          if (pwmPercentR < 30) pwmPercentR = 0;


          MotorSetSpeed(pwmPercentR, 'R');
      }

      MotorSetSpeed(0, 'R');
    }


}

// #endregion