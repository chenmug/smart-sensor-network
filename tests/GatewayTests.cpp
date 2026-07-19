#include <gtest/gtest.h>
#include "gateway/Gateway.hpp"
#include "fakes/FakeLogger.hpp"
#include "common/TimeUtils.hpp"
#include "common/SensorTypesString.hpp"


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
    ASSERT_EQ(sensors.at(1).lastTelemetry.header.sensorId, 1);
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

    ASSERT_EQ(gateway.getSensors().at(1).lastTelemetry.value, 0.5);
    ASSERT_EQ(gateway.getSensors().at(1).lastTelemetry.header.timestamp_ms, 100);

    gateway.updateSensorInfo(r2); 

    const auto& sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);
    ASSERT_EQ(sensors.at(1).lastTelemetry.value, 0.9);
    ASSERT_EQ(sensors.at(1).lastTelemetry.header.timestamp_ms, 200);
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
    EXPECT_GT(sensors.at(2).lastTelemetryReceivedTime, 0);
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
    EXPECT_EQ(sensors.at(5).lastTelemetry.header.sensorId, 5);
}


TEST(Gateway, HeartbeatDoesNotOverwriteTelemetry)
{
    FakeLogger logger;
    Gateway gateway(logger);

    TelemetryMessage telemetry;
    telemetry.header.sensorId = 5;
    telemetry.header.type = MessageType::TELEMETRY;
    telemetry.type = SensorType::Temperature;
    telemetry.value = 25.0;

    gateway.updateSensorInfo(telemetry);

    HeartbeatMessage hb;
    hb.header.type = MessageType::HEARTBEAT;
    hb.header.sensorId = 5;

    gateway.handleHeartbeat(hb);

    const auto& sensors = gateway.getSensors();

    EXPECT_EQ(sensors.at(5).lastTelemetry.value,25.0);
}


TEST(Gateway, HandlePacketProcessesHeartbeat)
{
    FakeLogger logger;
    Gateway gateway(logger);

    PacketSerializer serializer;

    HeartbeatMessage hb;
    hb.header.type = MessageType::HEARTBEAT;
    hb.header.sensorId = 7;
    hb.header.timestamp_ms = 123;

    auto packet = serializer.serialize(hb);

    gateway.handlePacket(packet);

    const auto sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);

    const auto& sensor = sensors.at(7);

    EXPECT_EQ(sensor.health, SensorHealth::ONLINE);
    EXPECT_GT(sensor.lastHeartbeatReceivedTime, 0);
}


TEST(Gateway, HeartbeatUpdatesExistingSensor)
{
    FakeLogger logger;
    Gateway gateway(logger);

    TelemetryMessage telemetry{
        {MessageType::TELEMETRY, 3, 100},
        SensorType::Temperature,
        SensorState::ACTIVE,
        22.5
    };

    gateway.updateSensorInfo(telemetry);

    uint64_t oldTelemetryTime = gateway.getSensors().at(3).lastTelemetryReceivedTime;


    HeartbeatMessage hb;
    hb.header.type = MessageType::HEARTBEAT;
    hb.header.sensorId = 3;

    gateway.handleHeartbeat(hb);


    const auto& sensor = gateway.getSensors().at(3);

    EXPECT_EQ(sensor.lastTelemetry.value, 22.5);

    EXPECT_GE(sensor.lastHeartbeatReceivedTime, oldTelemetryTime);
}


TEST(Gateway, HeartbeatOnline)
{
    FakeLogger logger;
    Gateway gateway(logger);

    HeartbeatMessage hb;
    hb.header.type = MessageType::HEARTBEAT;
    hb.header.sensorId = 1;
    hb.header.timestamp_ms = 123;

    gateway.handleHeartbeat(hb);
    const auto& sensor = gateway.getSensors().at(1);

    ASSERT_EQ(sensor.health, SensorHealth::ONLINE);
    EXPECT_GT(sensor.lastHeartbeatReceivedTime, 0);
}


TEST(Gateway, SensorWithoutHeartbeatRemainsUnknown)
{
    FakeLogger logger;
    Gateway gateway(logger);

    TelemetryMessage telemetry{
        {MessageType::TELEMETRY, 7, 100},
        SensorType::Temperature,
        SensorState::ACTIVE,
        30
    };

    gateway.updateSensorInfo(telemetry);

    const auto& sensor = gateway.getSensors().at(7);

    ASSERT_EQ(sensor.health, SensorHealth::UNKNOWN);
}


TEST(Gateway, HandlePacketHeartbeatDoesNotOverwriteTelemetry)
{
    FakeLogger logger;
    Gateway gateway(logger);

    PacketSerializer serializer;

    TelemetryMessage telemetry{
        {MessageType::TELEMETRY, 5, 100},
        SensorType::Temperature,
        SensorState::ACTIVE,
        25.0
    };

    auto telemetryPacket = serializer.serialize(telemetry);

    gateway.handlePacket(telemetryPacket);


    HeartbeatMessage hb;
    hb.header.type = MessageType::HEARTBEAT;
    hb.header.sensorId = 5;

    auto heartbeatPacket = serializer.serialize(hb);

    gateway.handlePacket(heartbeatPacket);


    const auto sensors = gateway.getSensors();

    ASSERT_EQ(sensors.size(), 1);

    const auto& sensor = sensors.at(5);

    EXPECT_EQ(sensor.lastTelemetry.value, 25.0);
    EXPECT_EQ(sensor.health, SensorHealth::ONLINE);
}


TEST(Gateway, SensorBecomesOnlineAfterHeartbeat)
{
    FakeLogger logger;
    Gateway gateway(logger);


    TelemetryMessage telemetry{
        {MessageType::TELEMETRY, 8, 100},
        SensorType::Motion,
        SensorState::ACTIVE,
        0.3
    };

    gateway.updateSensorInfo(telemetry);


    EXPECT_EQ(gateway.getSensors().at(8).health,SensorHealth::UNKNOWN);

    HeartbeatMessage hb;
    hb.header.type = MessageType::HEARTBEAT;
    hb.header.sensorId = 8;

    gateway.handleHeartbeat(hb);

    const auto& sensor = gateway.getSensors().at(8);

    EXPECT_EQ(sensor.health, SensorHealth::ONLINE);
    EXPECT_GT(sensor.lastHeartbeatReceivedTime, 0);
}