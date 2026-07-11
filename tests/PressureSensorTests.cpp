#include <gtest/gtest.h>
#include "sensor/PressureSensor.hpp"


TEST(PressureSensor, CreatesWithCorrectType)
{
    PressureSensor sensor(1);

    ASSERT_EQ(sensor.getType(), SensorType::Pressure);
}


TEST(PressureSensor, GeneratesReasonablePressureRange)
{
    PressureSensor sensor(1);

    double pressure = sensor.collectData();

    EXPECT_GT(pressure, 80.0);
    EXPECT_LT(pressure, 120.0);
}


TEST(PressureSensor, CollectDataUpdatesValue)
{
    PressureSensor sensor(1);

    double value = sensor.collectData();
    SensorReading reading = sensor.createTelemetry();

    ASSERT_EQ(reading.value, value);
}


TEST(PressureSensor, UpdatesStateCorrectly)
{
    PressureSensor sensor(1);

    double pressure = sensor.collectData();
    SensorReading reading = sensor.createTelemetry();

    if (pressure >= 98.0 && pressure <= 104.0)
    {
        ASSERT_EQ(reading.state, SensorState::ACTIVE);
    }
    else if ((pressure >= 95.0 && pressure < 98.0) || 
             (pressure > 104.0 && pressure <= 107.0))
    {
        ASSERT_EQ(reading.state, SensorState::WARNING);
    }
    else
    {
        ASSERT_EQ(reading.state, SensorState::ERROR);
    }
}


TEST(PressureSensor, TelemetryFieldsAreValid)
{
    PressureSensor sensor(42);

    sensor.collectData();
    SensorReading reading = sensor.createTelemetry();

    EXPECT_EQ(reading.sensorId, 42);
    EXPECT_EQ(reading.type, SensorType::Pressure);
    EXPECT_GT(reading.value, 80.0);
    EXPECT_LT(reading.value, 120.0);
    EXPECT_NE(reading.timestamp_ms, 0);
}