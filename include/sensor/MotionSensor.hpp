#pragma once
#include "sensor/Sensor.hpp"  // Forward Declaration

/**
 * @brief Motion sensor class representing a sensor type in the system
 */
class MotionSensor : public Sensor
{
private:
    double motionLevel = 0.0;      // The motion level
    const double threshold = 0.5;  // Minimum motion level required to detect movement

private:
    /**
     * @brief Checks whether motion is detected based on sensor data
     *
     * @return true if motion is detected, false otherwise
     */
    bool detectMotion() const;

public:
    /**
     * @brief Construct a new MotionSensor
     *
     * @param id The unique sensor identifier
     */
    MotionSensor(uint32_t id);

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