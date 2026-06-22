#pragma once
#include "sensor/Sensor.hpp"  // Forward Declaration

/**
 * @brief Motion sensor class representing a sensor type in the system
 */
class MotionSensor : public Sensor
{
private:
    float motionLevel;  // The motion level

private:
    /**
     * @brief Checks whether motion is detected based on sensor data
     *
     * @return true if motion is detected, false otherwise
     */
    bool detectMotion();

public:
    /**
     * @brief Construct a new MotionSensor
     *
     * @param id The unique sensor identifier
     */
    MotionSensor(uint32_t id);

    /**
     * @brief Generates a telemetry reading for the motion sensor
     *
     * @return The generated telemetry packet
     */
    SensorReading generateReading() override;
};