#pragma once
#include "sensor/Sensor.hpp"  // Forward Declaration


/**
 * @brief Pressure sensor class representing a sensor type in the system
 */
class PressureSensor : public Sensor
{
private:

    static constexpr double LOWER_WARNING_THRESHOLD = 98.0;   // The minimum value that when exceeded, the result is a warning
    static constexpr double UPPER_WARNING_THRESHOLD = 104.0;  // The maximum value that when exceeded, the result is a warning
    static constexpr double LOWER_ERROR_THRESHOLD = 95.0;     // The minimum value that when exceeded, the result is a error
    static constexpr double UPPER_ERROR_THRESHOLD = 107.0;    // The maximum value that when exceeded, the result is a error
    double pressureLevel = 0.0;                               // The pressure level of the sensor

public:
    /**
     * @brief Construct a new PressureSensor
     *
     * @param id The unique sensor identifier
     */
    PressureSensor(uint32_t id);

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