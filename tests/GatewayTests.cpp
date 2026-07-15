#include <gtest/gtest.h>
#include "gateway/Gateway.hpp"
#include "fakes/FakeLogger.hpp"


TEST(Gateway, CreatesNewSensor)
{
    FakeLogger logger;
    Gateway gateway(logger);
    
    TelemetryMessage r{
    {
        MessageType::TELEMETRY,
        1,
        100
    },
    SensorType::Motion,
    SensorState::ACTIVE,
    0.5};

    gateway.updateSensorInfo(r);
    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    ASSERT_EQ(sensors.at(1).lastReading.header.sensorId, 1);
    ASSERT_EQ(r.header.type, MessageType::TELEMETRY);
}


TEST(Gateway, OverwritesExistingSensor)
{
    FakeLogger logger;
    Gateway gateway(logger);

    TelemetryMessage r1{
    {
        MessageType::TELEMETRY,
        1,
        100
    },
    SensorType::Motion,
    SensorState::ACTIVE,
    0.5
    };

    TelemetryMessage r2{
    {
        MessageType::TELEMETRY,
        1,
        200
    },
    SensorType::Motion,
    SensorState::ACTIVE,
    0.9
    };

    gateway.updateSensorInfo(r1);

    ASSERT_EQ(gateway.getSensors().at(1).lastReading.value, 0.5);
    ASSERT_EQ(gateway.getSensors().at(1).lastReading.header.timestamp_ms, 100);

    gateway.updateSensorInfo(r2); 

    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    ASSERT_EQ(sensors.at(1).lastReading.value, 0.9);
    ASSERT_EQ(sensors.at(1).lastReading.header.timestamp_ms, 200);
}


TEST(Gateway, UpdatesTimestamp)
{
    FakeLogger logger;
    Gateway gateway(logger);

    TelemetryMessage r{
    {
        MessageType::TELEMETRY,
        2,
        100
    },
    SensorType::Motion,
    SensorState::ACTIVE,
    0.5
    };

    gateway.updateSensorInfo(r);

    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    EXPECT_GT(sensors.at(2).lastUpdateTime, 0);
}


TEST(Gateway, HandlePacketUpdatesSensor)
{
    FakeLogger logger;
    Gateway gateway(logger);

    PacketSerializer serializer;
    TelemetryMessage r{
    {
        MessageType::TELEMETRY,
        5,
        999
    },
    SensorType::Motion,
    SensorState::ACTIVE,
    0.77
    };

    auto packet = serializer.serialize(r);

    gateway.handlePacket(packet);

    auto sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    EXPECT_EQ(sensors.at(5).lastReading.header.sensorId, 5);
}