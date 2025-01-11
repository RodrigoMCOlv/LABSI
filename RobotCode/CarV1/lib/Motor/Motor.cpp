#include "MOTOR.h"    

volatile MOTOR motor;

MOTOR::MOTOR() {}
MOTOR::~MOTOR() {}

ISR(INT0_vect) {
  // Handle the interrupt from the left Motor
  motor.stepsL ++;
}

ISR(INT1_vect) {
  // Handle the interrupt from the left Motor
  motor.stepsR ++;
}



//TIMER and INTERRUPTS TRANSFERED and PINS
void MOTOR::Motors_init(bool debug) 
{
    //initialize PWM
    DEBUG = debug;

    TCCR2A =    (1 << COM2A1)|          // Clear OC2A on Compare Match, set OC2A at BOTTOM,(non-inverting mode).
                (0 << COM2A0)|          // Clear OC2A on Compare Match, set OC2A at BOTTOM,(non-inverting mode).
                (0 << COM2B1)|          // Normal port operation, OC2B disconnected.
                (0 << COM2B0)|          // Normal port operation, OC2B disconnected.
                (0 << WGM21 )|          // Phase Correct PWM mode
                (1 << WGM20 );          // Phase Correct PWM mode

    TCCR2B =    (0 << FOC2A )|          // This bit must be set to zero when TCCR2B is written when operating in PWM mode
                (0 << FOC2B )|          // This bit must be set to zero when TCCR2B is written when operating in PWM mode
                (0 << WGM22 )|          // Phase Correct PWM mode
                (1 << CS22  )|          // Prescaler 64, will give a 976Hz pwm ()
                (0 << CS21  )|          // Prescaler 64, will give a 976Hz pwm
                (0 << CS20  );          // Prescaler 64, will give a 976Hz pwm

    OCR2A = 0;                          // The pwm for L motor starts at 0%

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

    pinMode(6, OUTPUT);                 // Setting PD6 to output for use with left Motor PWM
    pinMode(11, OUTPUT);                // Setting PB3 to output for use with right Motor PWM
    pinMode(3, INPUT_PULLUP);           //Setting PD3 to input for use with external interrupt 1
    pinMode(2, INPUT_PULLUP);           //Setting PD2 to input for use with external interrupt 0


/*
    PIN_OUTPUT(PINB, 3);                // Setting PB3 to output for use with left Motor PWM
    PIN_OUTPUT(PIND, 6);                // Setting PD6 to output for use with right Motor PWM
    
    
    PIN_INPUT(PORTD, 3);                //Setting PD3 to input for use with external interrupt 1
    PULL_UP_RESISTOR(PORTD, 3);         //Setting PD3 to input for use with external interrupt 1
    PIN_INPUT(PORTD, 2);                //Setting PD2 to input for use with external interrupt 0
    PULL_UP_RESISTOR(PORTD, 2);         //Setting PD2 to input for use with external interrupt 0
*/    

    //initialize External Interrupts
    EICRA =     (1 << ISC11 )|          //Trigger on rising edge
                (1 << ISC10 )|          //Trigger on rising edge
                (1 << ISC01 )|          //Trigger on rising edge
                (1 << ISC00 );          //Trigger on rising edge

    EIMSK =     (1 << INT1 )|           //Enable External Interrupt 1
                (1 << INT0 );           //Enable External Interrupt 0


    sei();

    if (DEBUG) Serial.println("Motors Initialized");
}


void MOTOR::Motor_set(int16_t percentage, char motor)
{   //Input percentage from 0 to 100
    //input motor 'R' or 'L'
    if (percentage > 100) percentage = 100; 
    if (percentage < 0) percentage = 0;

    int16_t pwm = (int)255*percentage/100.0;


    

    if (motor == 'L') 
    {
        OCR0A = pwm;

        if (DEBUG)
        {
            Serial.print("Left Motor PWM: ");
            Serial.println(pwm);
        }

    }
    if (motor == 'R')
    {
        OCR2A = pwm;

        if (DEBUG)
        {   
            Serial.print("Right Motor PWM: ");
            Serial.println(pwm);
        }
    }
}

void MOTOR::incrementR()
{
    stepsR++;
}

void MOTOR::incrementL()
{
    stepsL++;
}

void MOTOR::resetDistance()
{
    stepsR = 0;
    stepsL = 0;
}

void MOTOR::GetDistance()
{
    
    int16_t delta = stepsR - stepsL;                                       //The difference between the number of steps for both wheels is responsible for the angle    
    int16_t forward = 0;

    if (stepsR < stepsL)    forward = (stepsL + delta) ;
    if (stepsR > stepsL)    forward = (stepsR - delta) ;

    
    if(forward < 0) forward = -forward;  

    uint16_t distance = (int) (forward/ stepsPerRev)* mmPerRev;              //distance in mm (only movement from both wheels is considered)

    int16_t angle = (int)(delta / stepDeltaPerDeg);                         //angle in hundreds of degrees
    
    hundredsOfDeg = angle;
    distanceInmm = distance;
    
    /*
    if (DEBUG) 
    {
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.print(" mm, Angle: ");
        Serial.println(angle);

        Serial.print("Steps R: ");
        Serial.print(stepsR);
        Serial.print(" Steps L: ");
        Serial.println(stepsL);

        Serial.print("Delta: ");
        Serial.println(delta);
        Serial.print("Forward: ");
        Serial.println(forward);
    }
    */

}

void MOTOR::rotate(int16_t hundredsOfDegToMove)
{
    GetDistance();

    if (hundredsOfDegToMove > 0)
    {
        while(hundredsOfDeg < hundredsOfDegToMove)
        {
            GetDistance();
            uint8_t pwmPercent = (80*exp(0.00005*(hundredsOfDegToMove - hundredsOfDeg)));
            if (pwmPercent > 100) pwmPercent = 100;

            Motor_set(pwmPercent, 'R');
            Motor_set(pwmPercent, 'L');

            if (DEBUG) 
            {
                Serial.print("Hunderds of Degrees to move positive: ");
                Serial.println(hundredsOfDegToMove);
                Serial.print("Hunderds of Degrees moved: ");
                Serial.println(hundredsOfDeg);
                Serial.print("Right Motor step count: ");
                Serial.println(stepsR);
                Serial.print("Left Motor step count: ");
                Serial.println(stepsL);
                Serial.print("PWM: ");
                Serial.println(pwmPercent);
            }
        }
        

    }

    if (hundredsOfDegToMove < 0)
    {
        while(hundredsOfDeg > hundredsOfDegToMove)
        {
            GetDistance();
            uint8_t pwmPercent = (80*exp(0.00005*(hundredsOfDeg - hundredsOfDegToMove)));
            if (pwmPercent > 100) pwmPercent = 100;

            Motor_set(pwmPercent, 'R');
            Motor_set(pwmPercent, 'L');

            if (DEBUG) 
            {
                Serial.print("Hunderds of Degrees to move negative: ");
                Serial.println(hundredsOfDegToMove);
                Serial.print("Hunderds of Degrees moved: ");
                Serial.println(hundredsOfDeg);
                Serial.print("Right Motor step count: ");
                Serial.println(stepsR);
                Serial.print("Left Motor step count: ");
                Serial.println(stepsL);
                Serial.print("PWM: ");
                Serial.println(pwmPercent);
            }
        }

        
    }

    resetDistance();


}

int16_t MOTOR::distanceToSteps(uint16_t distanceInmm)
{
    int16_t steps = distanceInmm * stepsPerRev / mmPerRev;
    return steps;
}

void MOTOR::move(uint16_t distanceToMove)
{
    int16_t StepstoMove = 0;
    StepstoMove = distanceToSteps(distanceToMove);
    resetDistance();


    while(stepsR < StepstoMove || stepsL < StepstoMove)
    {
        int16_t pwmPercentR = (int16_t)(0.04*(StepstoMove - stepsR) + 30);
        int16_t pwmPercentL = (int16_t)(0.04*(StepstoMove - stepsL) + 30);  

        if (pwmPercentR > 100) pwmPercentR = 100;
        if (pwmPercentL > 100) pwmPercentL = 100;
        if (pwmPercentR < 30) pwmPercentR = 0;
        if (pwmPercentL < 30) pwmPercentL = 0;

        
        Motor_set(pwmPercentR, 'R');
        Motor_set(pwmPercentL, 'L');


        if (DEBUG)
        {
            Serial.print("Steps to move: ");
            Serial.println(StepstoMove);
            Serial.print("Steps R: ");  
            Serial.println(stepsR);
            Serial.print("Steps L: ");
            Serial.println(stepsL); 
        }


    }

    Serial.print("I moved in the R: ");
    Serial.println(stepsR);
    Serial.print("I moved in the L: ");    
    Serial.println(stepsL);

    Motor_set(0, 'R');
    Motor_set(0, 'L');
}

void MOTOR::moveAndRotate(uint8_t distanceInmm, int16_t hundredsOfDeg)
{
    rotate(hundredsOfDeg);
    move(distanceInmm);
    
    resetDistance();
}

void MOTOR::printDistance()
{
    Serial.print("Distance: ");
    Serial.print(distanceInmm);
    Serial.print(" mm, Angle: ");
    Serial.println(hundredsOfDeg);
}

