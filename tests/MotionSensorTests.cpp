#include <gtest/gtest.h>
#include "sensor/MotionSensor.hpp"


TEST(MotionSensor, ValueInRange)
{
    MotionSensor sensor(1);
    double value = sensor.collectData();

    EXPECT_GE(value, 0.0);
    EXPECT_LE(value, 1.0);
}


TEST(MotionSensor, StateIsValid)
{
    MotionSensor sensor(1);

    sensor.collectData();
    SensorReading reading = sensor.createTelemetry();

    EXPECT_TRUE(
        reading.state == SensorState::ACTIVE ||
        reading.state == SensorState::WARNING
    );
}


TEST(MotionSensor, TelemetryFieldsAreValid)
{
    MotionSensor sensor(42);

    sensor.collectData();
    SensorReading reading = sensor.createTelemetry();

    EXPECT_EQ(reading.sensorId, 42);
    EXPECT_EQ(reading.type, SensorType::Motion);
    EXPECT_GE(reading.value, 0.0);
    EXPECT_LE(reading.value, 1.0);
    EXPECT_NE(reading.timestamp_ms, 0);
}


TEST(MotionSensor, ReturnsMotionType)
{
    MotionSensor sensor(1);

    EXPECT_EQ(sensor.getType(), SensorType::Motion);
}


TEST(MotionSensor, MultipleReadsProduceValidRange)
{
    MotionSensor sensor(1);

    for (int i = 0; i < 10; ++i)
    {
        double value = sensor.collectData();

        EXPECT_GE(value, 0.0);
        EXPECT_LE(value, 1.0);
    }
}