#include <gtest/gtest.h>
#include "gateway/Gateway.hpp"


TEST(Gateway, CreatesNewSensor)
{
    Gateway gateway;
    
    SensorReading r{1, SensorType::Motion, SensorState::ACTIVE, 0.5, 100};

    gateway.updateSensorInfo(r);
    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    ASSERT_EQ(sensors.at(1).lastReading.sensorId, 1);
}


TEST(Gateway, OverwritesExistingSensor)
{
    Gateway gateway;

    SensorReading r1{1, SensorType::Motion, SensorState::ACTIVE, 0.5, 100};
    SensorReading r2{1, SensorType::Motion, SensorState::ACTIVE, 0.9, 200};

    gateway.updateSensorInfo(r1);

    ASSERT_EQ(gateway.getSensors().at(1).lastReading.value, 0.5);
    ASSERT_EQ(gateway.getSensors().at(1).lastReading.timestamp_ms, 100);

    gateway.updateSensorInfo(r2); 

    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    ASSERT_EQ(sensors.at(1).lastReading.value, 0.9);
    ASSERT_EQ(sensors.at(1).lastReading.timestamp_ms, 200);
}


TEST(Gateway, UpdatesTimestamp)
{
    Gateway gateway;

    SensorReading r{2, SensorType::Motion, SensorState::ACTIVE, 0.5, 100};
    gateway.updateSensorInfo(r);

    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    EXPECT_GT(sensors.at(2).lastUpdateTime, 0);
}


TEST(Gateway, HandlePacketUpdatesSensor)
{
    Gateway gateway;

    TelemetrySerializer serializer;
    SensorReading r{5, SensorType::Motion, SensorState::ACTIVE, 0.77, 999};

    auto packet = serializer.serialize(r);

    gateway.handlePacket(packet);

    auto sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    EXPECT_EQ(sensors.at(5).lastReading.sensorId, 5);
}