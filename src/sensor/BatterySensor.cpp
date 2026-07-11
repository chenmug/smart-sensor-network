#include "sensor/BatterySensor.hpp"
#include <random>


// /*************** CONSTRUCTOR ***************/

BatterySensor::BatterySensor(uint32_t id)
    : Sensor(id)
{}


// /************** COLLECT DATA ***************/

double BatterySensor::collectData()
{
    // generate random battery level
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> batteryDrain(0.2, 0.5);

    batteryLevel -= batteryDrain(gen);
    if (batteryLevel < 0.0)
    {
        batteryLevel = 0.0;
    }

    currentValue = batteryLevel;

    if (batteryLevel > WARNING_THRESHOLD)
    {
        currentState = SensorState::ACTIVE;
    }
    else if (batteryLevel > ERROR_THRESHOLD)
    {
        currentState = SensorState::WARNING;
    }
    else
    {
        currentState = SensorState::ERROR;
    }

    return batteryLevel;
}


// /**************** GET TYPE *****************/

SensorType BatterySensor::getType() const
{
    return SensorType::Battery;
}