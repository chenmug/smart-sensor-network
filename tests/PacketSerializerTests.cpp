#include <gtest/gtest.h>
#include "network/PacketSerializer.hpp"


TEST(PacketSerializer, SerializeDeserializeTelemetry)
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

    auto buffer = PacketSerializer::serialize(original);
    auto result = PacketSerializer::deserializeTelemetry(buffer);

    EXPECT_EQ(result.header.type, MessageType::TELEMETRY);
    EXPECT_EQ(result.header.sensorId, 5);
    EXPECT_EQ(result.header.timestamp_ms, 12345);

    EXPECT_EQ(result.type, SensorType::Temperature);
    EXPECT_EQ(result.state, SensorState::ACTIVE);
    EXPECT_DOUBLE_EQ(result.value, 25.5);
}


TEST(PacketSerializer, SerializeDeserializeHeartbeat)
{
    HeartbeatMessage original{
        {
            MessageType::HEARTBEAT,
            42,
            123456789
        }
    };

    auto buffer = PacketSerializer::serialize(original);
    auto result = PacketSerializer::deserializeHeartbeat(buffer);

    EXPECT_EQ(result.header.type, MessageType::HEARTBEAT);
    EXPECT_EQ(result.header.sensorId, 42);
    EXPECT_EQ(result.header.timestamp_ms, 123456789);
}


TEST(PacketSerializer, TelemetryPacketHasExpectedSize)
{
    TelemetryMessage message{
        {
            MessageType::TELEMETRY,
            1,
            100
        },
        SensorType::Motion,
        SensorState::ACTIVE,
        0.5
    };

    auto buffer = PacketSerializer::serialize(message);

    EXPECT_EQ(buffer.size(), 23);
}


TEST(PacketSerializer, HeartbeatPacketHasExpectedSize)
{
    HeartbeatMessage message{
        {
            MessageType::HEARTBEAT,
            1,
            100
        }
    };

    auto buffer = PacketSerializer::serialize(message);

    EXPECT_EQ(buffer.size(), 13);
}


TEST(PacketSerializer, SerializesProtocolEnumsUsingExpectedWireValues)
{
    TelemetryMessage message{
        {
            MessageType::TELEMETRY,
            1,
            100
        },
        SensorType::Pressure,
        SensorState::ERROR,
        10.0
    };

    auto buffer = PacketSerializer::serialize(message);

    EXPECT_EQ(buffer[0], 1);   // TELEMETRY
    EXPECT_EQ(buffer[13], 4);  // Pressure
    EXPECT_EQ(buffer[14], 3);  // ERROR
}


TEST(PacketSerializer, DeserializesDoubleCorrectly)
{
    TelemetryMessage original{
        {
            MessageType::TELEMETRY,
            1,
            100
        },
        SensorType::Temperature,
        SensorState::WARNING,
        123.456
    };

    auto buffer = PacketSerializer::serialize(original);
    auto result = PacketSerializer::deserializeTelemetry(buffer);

    EXPECT_DOUBLE_EQ(result.value, original.value);
}



TEST(PacketSerializer, PeekMessageTypeReturnsTelemetry)
{
    TelemetryMessage message{
        {
            MessageType::TELEMETRY,
            7,
            500
        },
        SensorType::Motion,
        SensorState::ACTIVE,
        0.75
    };

    auto buffer = PacketSerializer::serialize(message);

    EXPECT_EQ(PacketSerializer::peekMessageType(buffer), MessageType::TELEMETRY);
}


TEST(PacketSerializer, PeekMessageTypeReturnsHeartbeat)
{
    HeartbeatMessage message{
        {
            MessageType::HEARTBEAT,
            7,
            500
        }
    };

    auto buffer = PacketSerializer::serialize(message);

    EXPECT_EQ(PacketSerializer::peekMessageType(buffer), MessageType::HEARTBEAT);
}


TEST(PacketSerializer, ThrowsWhenPeekingIncompletePacket)
{
    std::vector<uint8_t> buffer(12, 0);

    EXPECT_THROW(PacketSerializer::peekMessageType(buffer), std::runtime_error);
}


TEST(PacketSerializer, ThrowsOnIncompleteHeartbeatPacket)
{
    std::vector<uint8_t> buffer(12, 0);

    EXPECT_THROW(PacketSerializer::deserializeHeartbeat(buffer), std::runtime_error);
}


TEST(PacketSerializer, ThrowsOnIncompleteTelemetryPacket)
{
    std::vector<uint8_t> buffer(22, 0);

    EXPECT_THROW(PacketSerializer::deserializeTelemetry(buffer), std::runtime_error);
}