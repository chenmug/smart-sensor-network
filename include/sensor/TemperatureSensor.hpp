#pragma once
#include "sensor/Sensor.hpp"  // Forward Declaration

/**
 * @brief Temperature sensor class representing a sensor type in the system
 */
class TemperatureSensor : public Sensor
{
private:
    double temperatureLevel = 0.0;                     // The temperature level of the sensor
    static constexpr double WARNING_THRESHOLD = 30.0;  // The mininum temperature to get a warning
    static constexpr double ERROR_THRESHOLD = 40.0;    // The minimum temperature to get an error

public:
    /**
     * @brief Construct a new TemperatureSensor
     *
     * @param id The unique sensor identifier
     */
    TemperatureSensor(uint32_t id);

    /**
     * @brief Collect the sensor data and update the current state.
     * 
     * @return The data of the sensor
     */
    double collectData() override;

    /**
     * @brief Returns the sensor type.
     *
     * @return SensorType Type identifier of the sensor.
     */
    SensorType getType() const override;
};