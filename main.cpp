#include "sensor/MotionSensor.h"


int main()
{
    MotionSensor sensor(1);
    sensor.runLoop();
    
    return 0;
}