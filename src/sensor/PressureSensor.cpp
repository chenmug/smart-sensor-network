#include "sensor/PressureSensor.hpp"
#include <random>


// /*************** CONSTRUCTOR ***************/

PressureSensor::PressureSensor(uint32_t id)
    : Sensor(id)
{}


// /************** COLLECT DATA ***************/

double PressureSensor::collectData()
{
    // generate random pressure level
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<double> pressureRange(101.3, 2.0);

    pressureLevel = pressureRange(gen);
    currentValue = pressureLevel;

    if (pressureLevel >= LOWER_WARNING_THRESHOLD || pressureLevel <= UPPER_WARNING_THRESHOLD)
    {
        currentState = SensorState::ACTIVE;
    }
    else if ((pressureLevel >= LOWER_ERROR_THRESHOLD && pressureLevel < LOWER_WARNING_THRESHOLD) ||
             (pressureLevel > UPPER_WARNING_THRESHOLD && pressureLevel <= UPPER_ERROR_THRESHOLD))
    {
        currentState = SensorState::WARNING;
    }
    else
    {
        currentState = SensorState::ERROR;
    }

    return pressureLevel;
}


// /**************** GET TYPE *****************/

SensorType PressureSensor::getType() const
{
    return SensorType::Pressure;
}
