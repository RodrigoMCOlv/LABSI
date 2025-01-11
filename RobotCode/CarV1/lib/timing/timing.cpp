#include<timing.h>

TIMER timer_class;
uint32_t CurrentTime = 0;


ISR(TIMER1_COMPA_vect)                                                  //Interrupt handler for Timer 1
{
    timer_class.increment_ticks();
}


void TIMER::test()
{
    Serial.print("Triggered at: ");
    Serial.println(CurrentTime);
};


TIMER:: TIMER( ){  timer_init(); };
TIMER::~TIMER( ){ };

                                                   
uint32_t TIMER::get_ticks()     { return this-> ticks;}         // Gets ticks

void TIMER::reset_ticks()       { this-> ticks = 0;}            // Resets Ticks

void TIMER::increment_ticks()   { this-> ticks++;}              // Increment ticks

void TIMER::start_timer()       {TIMSK1 = (1 << OCIE1A); }      // Starts timer

void TIMER::stop_timer()        {TIMSK1 &= ~(1 << OCIE1A);}     // Clear the interrupt enable flag

//TRANSFERED
void TIMER::timer_init()                                        // Initializes the timer
{

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

    //TCCR1C =    (1 << FOC1A );                                  //Bit 7 – FOC1A: Force Output Compare for Channel A

    OCR1A  =                19;                                  //With a prescaler of 8 and a 10us tick the OCR1A should be set to 19

    TIMSK1 =    (0 << OCIE1A);                                  //Timer/Counter1, Output Compare A Match Interrupt Disable, this is here for future use

    sei();
}

void TIMER::delay_10us()                                        //Starts an optimized delay function for 10us
{

timer_class.reset_ticks();                                      //Making sure we start from 0                                                                                  
timer_class.start_timer();                                      //Enable interrupts

while(timer_class.get_ticks() < 1);                             //While 10us not elapsed                                                               

timer_class.stop_timer();                                       //Stop interrupts to free up the CPU
}

void TIMER::delay_us(uint32_t timeinus)                                 //Starts a "delay" function for timeinus
{
    volatile uint32_t tensofus = timeinus / 10;                         //We divide by 10 because the timer timebase is 10us 
    timer_class.reset_ticks();                                          //Making sure we start from 0
    volatile uint32_t  current_ticks = timer_class.get_ticks();         //Getting the current ticks in to a variable for easier use 
                                                                                                                                
    timer_class.start_timer();                                          //Enable interrupts
    

    while(current_ticks < tensofus)                                     //While time we want is not met
    {
        cli();                                                          //Disable interrupts                        
        current_ticks = timer_class.get_ticks();                        //Getting the current ticks
        sei();                                                          //Enable interrupts
    }                                                                   

    timer_class.stop_timer();                                           //Stop interrupts to free up the CPU
}