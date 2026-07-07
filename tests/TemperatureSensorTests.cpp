#include <gtest/gtest.h>
#include "sensor/TemperatureSensor.hpp"


TEST(TemperatureSensor, GeneratedTemperatureIsReasonable)
{
    TemperatureSensor sensor(1);
    double value = sensor.collectData();

    EXPECT_GT(value, -20);
    EXPECT_LT(value, 70);
}


TEST(TemperatureSensor, CreatesWithCorrectType)
{
    TemperatureSensor sensor(1);

    ASSERT_EQ(sensor.getType(), SensorType::Temperature);
}


TEST(TemperatureSensor, CollectDataUpdatesValue)
{
    TemperatureSensor sensor(1);

    double value = sensor.collectData();
    SensorReading reading = sensor.createTelemetry();

    ASSERT_EQ(reading.value, value);
}


TEST(TemperatureSensor, UpdatesStateCorrectly)
{
    TemperatureSensor sensor(1);

    double temperature = sensor.collectData();
    SensorReading reading = sensor.createTelemetry();

    if (temperature < 30)
    {
        ASSERT_EQ(reading.state, SensorState::ACTIVE);
    }
    else if (temperature < 40)
    {
        ASSERT_EQ(reading.state, SensorState::WARNING);
    }
    else
    {
        ASSERT_EQ(reading.state, SensorState::ERROR);
    }
}


TEST(TemperatureSensor, TelemetryFieldsAreValid)
{
    TemperatureSensor sensor(42);

    sensor.collectData();
    SensorReading reading = sensor.createTelemetry();

    EXPECT_EQ(reading.sensorId, 42);
    EXPECT_EQ(reading.type, SensorType::Temperature);
    EXPECT_GT(reading.value, -20);
    EXPECT_LT(reading.value, 70);
    EXPECT_NE(reading.timestamp_ms, 0);
}