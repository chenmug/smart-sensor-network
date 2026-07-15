#include <gtest/gtest.h>
#include "network/UdpReceiver.hpp"
#include "fakes/FakeLogger.hpp"


TEST(UdpReceiver, ForwardsPacketToGateway)
{
    FakeLogger logger;
    Gateway gateway(logger);

    UdpReceiver receiver(gateway, logger, 9000);

    TelemetryMessage r{
    {
        MessageType::TELEMETRY,
        10,
        123
    },
    SensorType::Motion,
    SensorState::ACTIVE,
    0.42
    };

    PacketSerializer serializer;
    auto packet = serializer.serialize(r);

    // simulate receive (instead of real socket)
    gateway.handlePacket(packet);
    auto sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    EXPECT_EQ(sensors.at(10).lastTelemetry.value, 0.42);
}


TEST(UdpReceiver, HandlesMultiplePackets)
{
    FakeLogger logger;
    Gateway gateway(logger);

    PacketSerializer serializer;

    TelemetryMessage r1{
    {
        MessageType::TELEMETRY,
        1,
        100
    },
    SensorType::Motion,
    SensorState::ACTIVE,
    0.1
    };
    
    TelemetryMessage r2{
    {
        MessageType::TELEMETRY,
        2,
        200
    },
    SensorType::Motion,
    SensorState::ACTIVE,
    0.9
    };

    gateway.handlePacket(serializer.serialize(r1));
    gateway.handlePacket(serializer.serialize(r2));

    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 2);
    ASSERT_EQ(sensors.at(1).lastTelemetry.header.sensorId, 1);
    ASSERT_EQ(sensors.at(2).lastTelemetry.header.sensorId, 2);
}


TEST(UdpReceiver, PacketOverwriteBehavior)
{
    FakeLogger logger;
    Gateway gateway(logger);
    PacketSerializer serializer;

    TelemetryMessage r1{
    {
        MessageType::TELEMETRY,
        1,
        100
    },
    SensorType::Motion,
    SensorState::ACTIVE,
    0.1
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

    gateway.handlePacket(serializer.serialize(r1));
    gateway.handlePacket(serializer.serialize(r2));

    auto sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    EXPECT_EQ(sensors.at(1).lastTelemetry.value, 0.9);
}


TEST(Gateway, MultipleSensorsAreStored)
{
    FakeLogger logger;
    Gateway gateway(logger);

    gateway.updateSensorInfo({{MessageType::TELEMETRY, 1, 100}, SensorType::Motion, SensorState::ACTIVE, 0.1});
    gateway.updateSensorInfo({{MessageType::TELEMETRY, 2, 200}, SensorType::Motion, SensorState::WARNING, 0.2});
    gateway.updateSensorInfo({{MessageType::TELEMETRY, 3, 300}, SensorType::Motion, SensorState::ACTIVE, 0.3});

    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 3);

    EXPECT_EQ(sensors.at(1).lastTelemetry.header.sensorId, 1);
    EXPECT_EQ(sensors.at(2).lastTelemetry.header.sensorId, 2);
    EXPECT_EQ(sensors.at(3).lastTelemetry.header.sensorId, 3);
}


TEST(Gateway, LastUpdateTimeIsRecorded)
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
    0.5
    };

    gateway.updateSensorInfo(r);

    const auto& sensors = gateway.getSensors();

    EXPECT_GT(sensors.at(1).lastTelemetryTime, 0u);
}


TEST(Gateway, HandlePacketStoresCorrectReading)
{
    FakeLogger logger;
    Gateway gateway(logger);
    PacketSerializer serializer;

    TelemetryMessage r{
    {
        MessageType::TELEMETRY,
        42,
        555
    },
    SensorType::Motion,
    SensorState::WARNING,
    0.87
    };

    auto packet = serializer.serialize(r);

    gateway.handlePacket(packet);

    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);

    const auto& stored = sensors.at(42).lastTelemetry;

    EXPECT_EQ(stored.header.type, MessageType::TELEMETRY);
    EXPECT_EQ(stored.header.sensorId, 42);
    EXPECT_EQ(stored.type, SensorType::Motion);
    EXPECT_EQ(stored.state, SensorState::WARNING);
    EXPECT_DOUBLE_EQ(stored.value, 0.87);
    EXPECT_EQ(stored.header.timestamp_ms, 555);
}