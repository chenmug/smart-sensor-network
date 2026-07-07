#include "sensor/TemperatureSensor.hpp"
#include <random>


// /*************** CONSTRUCTOR ***************/

TemperatureSensor::TemperatureSensor(uint32_t id)
    : Sensor(id)
{}


// /************** COLLECT DATA ***************/

double TemperatureSensor::collectData()
{
    // generate random temperature level
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<double> dist(25.0, 5.0);

    temperatureLevel = dist(gen);
    currentValue = temperatureLevel;

    if (temperatureLevel < WARNING_THRESHOLD)
    {
        currentState = SensorState::ACTIVE;
    }
    else if (temperatureLevel < ERROR_THRESHOLD)
    {
        currentState = SensorState::WARNING;
    }
    else
    {
        currentState = SensorState::ERROR;
    }

    return temperatureLevel;
}


// /**************** GET TYPE *****************/

SensorType TemperatureSensor::getType() const
{
    return SensorType::Temperature;
}