#include <gtest/gtest.h>
#include "sensor/SensorNode.hpp"
#include "sensor/MotionSensor.hpp"
#include "fakes/FakeUdpSender.hpp"
#include "fakes/FakeLogger.hpp"


TEST(SensorNode, TickProducesValidReading)
{
    MotionSensor sensor(1);
    FakeUdpSender sender;
    FakeLogger logger;

    SensorNode node(sensor, sender, logger, std::chrono::milliseconds(100));

    TelemetryMessage r = node.tick();

    EXPECT_EQ(r.header.sensorId, 1);
    EXPECT_GE(r.value, 0.0);
    EXPECT_LE(r.value, 1.0);
}


TEST(SensorNode, SendsPacket)
{
    MotionSensor sensor(1);
    FakeUdpSender sender;
    FakeLogger logger;
    
    SensorNode node(sensor, sender, logger);
    const auto& packets = sender.getPackets();

    node.tick();

    EXPECT_EQ(packets.size(), 1);
    EXPECT_FALSE(packets[0].empty());
}


TEST(SensorNode, TickUpdatesStateCorrectly)
{
    MotionSensor sensor(1);
    FakeUdpSender sender;
    FakeLogger logger;

    SensorNode node(sensor, sender, logger, std::chrono::milliseconds(100));

    TelemetryMessage r = node.tick();

    EXPECT_TRUE(
        r.state == SensorState::ACTIVE ||
        r.state == SensorState::WARNING
    );
}


TEST(SensorNode, PreservesSensorIdentityAcrossTicks)
{
    MotionSensor sensor(1);
    FakeUdpSender sender;
    FakeLogger logger;

    SensorNode node(sensor, sender, logger);

    TelemetryMessage first = node.tick();
    TelemetryMessage second = node.tick();

    EXPECT_EQ(first.header.sensorId, second.header.sensorId);
    EXPECT_EQ(first.type, second.type);
    EXPECT_NE(first.value, second.value);
}


TEST(SensorNode, TickProducesValidTimestamp)
{
    MotionSensor sensor(1);
    FakeUdpSender sender;
    FakeLogger logger;

    SensorNode node(sensor, sender, logger);

    TelemetryMessage r1 = node.tick();
    TelemetryMessage r2 = node.tick();

    EXPECT_TRUE(r2.header.timestamp_ms >= r1.header.timestamp_ms);
}


TEST(SensorNode, SendsMultiplePackets)
{
    MotionSensor sensor(1);
    FakeUdpSender sender;
    FakeLogger logger;
    
    SensorNode node(sensor, sender, logger);
    const auto& packets = sender.getPackets();

    node.tick();
    node.tick();

    EXPECT_EQ(packets.size(), 2);
}


TEST(TelemetrySerializer, SerializeDeserializeTelemetry)
{
    TelemetryMessage original{
        {
            MessageType::TELEMETRY,
            5,
            12345
        },
        SensorType::Temperature,
        SensorState::ACTIVE,
        25.5
    };


    auto buffer = TelemetrySerializer::serialize(original);
    auto result = TelemetrySerializer::deserialize(buffer);

    EXPECT_EQ(result.header.type, MessageType::TELEMETRY);
    EXPECT_EQ(result.header.sensorId, 5);
    EXPECT_EQ(result.header.timestamp_ms, 12345);

    EXPECT_EQ(result.type, SensorType::Temperature);
    EXPECT_EQ(result.state, SensorState::ACTIVE);
    EXPECT_DOUBLE_EQ(result.value, 25.5);
}