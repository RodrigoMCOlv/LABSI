#ifndef MOTOR_H
#define MOTOR_H

#include <core.h>

typedef struct
{ 
  uint16_t US1distance = 0;
  const uint16_t US1Angle = 0;

  uint16_t US12istance = 0;
  const uint16_t US2Angle = 90;

  uint16_t US3distance = 0;
  const uint16_t US3Angle = 180;

  uint16_t US4distance = 0; 
  const uint16_t US4Angle = 270;

  int16_t currentAngle = 0;
  uint16_t currentDistance = 0;

} Motor_readings;

class MOTOR
{
    public:        

        MOTOR();
        ~MOTOR();

        void Motors_init(bool debug = false);             //
        void Motor_set(int16_t percentage, char motor);
        void GetDistance();
        
        int16_t distanceToSteps(uint16_t distanceInmm);

        void resetDistance();
        void move(uint16_t distanceInmm);
        void rotate(int16_t hundredsOfDeg);
        void incrementR();
        void incrementL();

        void moveAndRotate(uint8_t distanceInmm, int16_t hundredsOfDeg);
    
        volatile int16_t stepsR = 0;
        volatile int16_t stepsL = 0;

        void printDistance();

    private:
        
        const uint16_t stepsPerRev = 372;

        const uint16_t mmPerRev = 67;
        const int16_t stepDeltaPerDeg = 1; 
        bool DEBUG;
        uint16_t distanceInmm = 0;
        int16_t hundredsOfDeg = 0;

};

extern volatile MOTOR motor;

#endif