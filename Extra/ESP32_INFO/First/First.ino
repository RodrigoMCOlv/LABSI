#include "soc/soc.h"       // Main SOC functions and definitions
#include "soc/timer_periph.h"   // GPIO-specific register definitions





void setup() {
  // put your setup code here, to run once:
  REG_WRITE(TIMG_T0CONFIG_REG(TIMER_GROUP_0),
                       (1 << TIMG0_T0_EN)                       //When set, the timer time-base counter is enabled. (R/W)
                       (0 << TIMG0_T0_INCREASE)                 //When set, the timer time-base counter will increment every clock tick.
                       (0 << TIMG0_T0_AUTORELOAD)               //When set, timer auto-reload at alarm is enabled. (R/W)
                       (0b0000000011111111 << TIMG0_T0_DIVIDER) // Timer clock (T0_clk) prescale value. (R/W) (255)
                       (1 << TIMG0_T0_EDGE_INT_EN)              //When set, an alarm will generate an edge type interrupt. (R/W)
                       (0 << TIMG0_T0_LEVEL_INT_EN)             //When set, an alarm will generate a level type interrupt. (R/W)
                       (1 << TIMG0_T0_ALARM_EN)                 //When set, the alarm is enabled. This bit is automatically cleared once an alarm occurs. (R/W)
                        );

 

}

void loop() {
  // put your main code here, to run repeatedly:

  timg_wdtconfig0_reg_t.wdt_en=1
}
