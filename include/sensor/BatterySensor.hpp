#pragma once
#include "sensor/Sensor.hpp"  // Forward Declaration


/**
 * @brief Battery sensor class representing a sensor type in the system
 */
class BatterySensor : public Sensor
{
private:

    static constexpr double FULL_BATTERY = 100.0;      // The maximum battery level
    static constexpr double WARNING_THRESHOLD = 30.0;  // The minimum battery level to get a warning
    static constexpr double ERROR_THRESHOLD = 10.0;    // The minimum battery level to get an error
    double batteryLevel = FULL_BATTERY;                // The battery level of the sensor

public:
    /**
     * @brief Construct a new BatterySensor
     *
     * @param id The unique sensor identifier
     */
    BatterySensor(uint32_t id);

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