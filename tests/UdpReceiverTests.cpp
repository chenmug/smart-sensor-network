#include <gtest/gtest.h>
#include "network/UdpReceiver.hpp"


TEST(UdpReceiver, ForwardsPacketToGateway)
{
    Gateway gateway;

    UdpReceiver receiver(gateway, 9000);

    SensorReading r{10, SensorType::Motion, SensorState::ACTIVE, 0.42, 123};

    TelemetrySerializer serializer;
    auto packet = serializer.serialize(r);

    // simulate receive (instead of real socket)
    gateway.handlePacket(packet);
    auto sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    EXPECT_EQ(sensors.at(10).lastReading.value, 0.42);
}


TEST(UdpReceiver, HandlesMultiplePackets)
{
    Gateway gateway;

    TelemetrySerializer serializer;

    SensorReading r1{1, SensorType::Motion, SensorState::ACTIVE, 0.1, 100};
    SensorReading r2{2, SensorType::Motion, SensorState::ACTIVE, 0.9, 200};

    gateway.handlePacket(serializer.serialize(r1));
    gateway.handlePacket(serializer.serialize(r2));

    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 2);
    ASSERT_EQ(sensors.at(1).lastReading.sensorId, 1);
    ASSERT_EQ(sensors.at(2).lastReading.sensorId, 2);
}


TEST(UdpReceiver, PacketOverwriteBehavior)
{
    Gateway gateway;
    TelemetrySerializer serializer;

    SensorReading r1{1, SensorType::Motion, SensorState::ACTIVE, 0.1, 100};
    SensorReading r2{1, SensorType::Motion, SensorState::WARNING, 0.9, 200};

    gateway.handlePacket(serializer.serialize(r1));
    gateway.handlePacket(serializer.serialize(r2));

    auto sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    EXPECT_EQ(sensors.at(1).lastReading.value, 0.9);
}


TEST(Gateway, MultipleSensorsAreStored)
{
    Gateway gateway;

    gateway.updateSensorInfo({1, SensorType::Motion, SensorState::ACTIVE, 0.1, 100});
    gateway.updateSensorInfo({2, SensorType::Motion, SensorState::WARNING, 0.2, 200});
    gateway.updateSensorInfo({3, SensorType::Motion, SensorState::ACTIVE, 0.3, 300});

    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 3);

    EXPECT_EQ(sensors.at(1).lastReading.sensorId, 1);
    EXPECT_EQ(sensors.at(2).lastReading.sensorId, 2);
    EXPECT_EQ(sensors.at(3).lastReading.sensorId, 3);
}


TEST(Gateway, LastUpdateTimeIsRecorded)
{
    Gateway gateway;

    SensorReading r{1, SensorType::Motion, SensorState::ACTIVE, 0.5, 100};

    gateway.updateSensorInfo(r);

    const auto& sensors = gateway.getSensors();

    EXPECT_GT(sensors.at(1).lastUpdateTime, 0u);
}


TEST(Gateway, HandlePacketStoresCorrectReading)
{
    Gateway gateway;
    TelemetrySerializer serializer;

    SensorReading r{42, SensorType::Motion, SensorState::WARNING, 0.87, 555};

    auto packet = serializer.serialize(r);

    gateway.handlePacket(packet);

    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);

    const auto& stored = sensors.at(42).lastReading;

    EXPECT_EQ(stored.sensorId, 42);
    EXPECT_EQ(stored.type, SensorType::Motion);
    EXPECT_EQ(stored.state, SensorState::WARNING);
    EXPECT_DOUBLE_EQ(stored.value, 0.87);
    EXPECT_EQ(stored.timestamp_ms, 555);
}