#include "sensor/MotionSensor.hpp"
#include <random>


// /*************** CONSTRUCTOR ***************/

MotionSensor::MotionSensor(uint32_t id)
    : Sensor(id)
{}


// /************** DETECT MOTION **************/

bool MotionSensor::detectMotion() const
{
    return motionLevel > threshold;
}


// /************ GENERATE READING *************/

double MotionSensor::collectData()
{
    // generate random motion level
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);

    motionLevel = dist(gen);

    currentValue = motionLevel;
    currentState = detectMotion() ? SensorState::ACTIVE : SensorState::WARNING;
    
   return motionLevel;
}


// /**************** GET TYPE *****************/

SensorType MotionSensor::getType() const
{
    return SensorType::Motion;
}