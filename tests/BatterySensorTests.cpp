#include <gtest/gtest.h>
#include "sensor/BatterySensor.hpp"


TEST(BatterySensor, CreatesWithCorrectType)
{
    BatterySensor sensor(1);

    ASSERT_EQ(sensor.getType(), SensorType::Battery);
}


TEST(BatterySensor, BatteryDescending)
{
    BatterySensor sensor(1);
    double first = sensor.collectData();
    double second = sensor.collectData();

    EXPECT_LT(first, 100.0);
    EXPECT_GT(first, 0.0);

    EXPECT_LT(second, first);
}


TEST(BatterySensor, BatteryLevelDecreasesOverTime)
{
    BatterySensor sensor(1);

    double previous = sensor.collectData();

    for (int i = 0; i < 20; ++i)
    {
        double current = sensor.collectData();
        EXPECT_LE(current, previous);
        previous = current;
    }
}


TEST(BatterySensor, BatteryNeverBecomesNegative)
{
    BatterySensor sensor(1);

    double value = 0.0;

    for (int i = 0; i < 500; ++i)
    {
        value = sensor.collectData();
    }

    EXPECT_GE(value, 0.0);
}


TEST(BatterySensor, CollectDataUpdatesValue)
{
    BatterySensor sensor(1);

    double value = sensor.collectData();
    TelemetryMessage reading = sensor.createTelemetry();

    ASSERT_EQ(reading.value, value);
}


TEST(BatterySensor, UpdatesStateCorrectly)
{
    BatterySensor sensor(1);

    double battery = sensor.collectData();
    TelemetryMessage reading = sensor.createTelemetry();

    if (battery > 30)
    {
        ASSERT_EQ(reading.state, SensorState::ACTIVE);
    }
    else if (battery > 10)
    {
        ASSERT_EQ(reading.state, SensorState::WARNING);
    }
    else
    {
        ASSERT_EQ(reading.state, SensorState::ERROR);
    }
}


TEST(BatterySensor, TelemetryFieldsAreValid)
{
    BatterySensor sensor(42);

    sensor.collectData();
    TelemetryMessage reading = sensor.createTelemetry();

    EXPECT_EQ(reading.header.type, MessageType::TELEMETRY);
    EXPECT_EQ(reading.header.sensorId, 42);
    EXPECT_EQ(reading.type, SensorType::Battery);
    EXPECT_GT(reading.value, 0.0);
    EXPECT_LT(reading.value, 100.0);
    EXPECT_NE(reading.header.timestamp_ms, 0);
}