#include <gtest/gtest.h>
#include "sensor/SensorNode.hpp"
#include "sensor/MotionSensor.h"
#include "fakes/FakeUdpSender.hpp"


TEST(SensorNode, TickProducesValidReading)
{
    MotionSensor sensor(1);
    FakeUdpSender sender;

    SensorNode node(sensor, sender, std::chrono::milliseconds(100));

    SensorReading r = node.tick();

    EXPECT_EQ(r.sensorId, 1);
    EXPECT_GE(r.value, 0.0);
    EXPECT_LE(r.value, 1.0);
}


TEST(SensorNode, SendsPacket)
{
    MotionSensor sensor(1);
    FakeUdpSender sender;
    SensorNode node(sensor, sender);
    const auto& packets = sender.getPackets();

    node.tick();

    EXPECT_EQ(packets.size(), 1);
    EXPECT_FALSE(packets[0].empty());
}


TEST(SensorNode, TickUpdatesStateCorrectly)
{
    MotionSensor sensor(1);
    FakeUdpSender sender;

    SensorNode node(sensor, sender, std::chrono::milliseconds(100));

    SensorReading r = node.tick();

    EXPECT_TRUE(
        r.state == SensorState::ACTIVE ||
        r.state == SensorState::WARNING
    );
}


TEST(SensorNode, PreservesSensorIdentityAcrossTicks)
{
    MotionSensor sensor(1);
    FakeUdpSender sender;

    SensorNode node(sensor, sender);

    SensorReading first = node.tick();
    SensorReading second = node.tick();

    EXPECT_EQ(first.sensorId, second.sensorId);
    EXPECT_EQ(first.type, second.type);
    EXPECT_NE(first.value, second.value);
}


TEST(SensorNode, TickProducesValidTimestamp)
{
    MotionSensor sensor(1);
    FakeUdpSender sender;

    SensorNode node(sensor, sender);

    SensorReading r1 = node.tick();
    SensorReading r2 = node.tick();

    EXPECT_TRUE(r2.timestamp_ms >= r1.timestamp_ms);
}


TEST(SensorNode, SendsMultiplePackets)
{
    MotionSensor sensor(1);
    FakeUdpSender sender;
    SensorNode node(sensor, sender);
    const auto& packets = sender.getPackets();

    node.tick();
    node.tick();

    EXPECT_EQ(packets.size(), 2);
}


