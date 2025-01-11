#ifndef TIMING_H
#define TIMING_H

#include <core.h>


class TIMER
{
    public:        

        TIMER();
        ~TIMER();

        uint32_t get_ticks();   //
        void reset_ticks();     //      
        void increment_ticks(); //
        void start_timer();     //
        void stop_timer();      //
        void timer_init();      //      
        void delay_10us();
        void delay_us(uint32_t timeinus);
        void test();

    private:
        volatile uint32_t ticks;  
};

extern TIMER timer_class;

#endif