#include <gtest/gtest.h>
#include "network/TcpServer.hpp"
#include "fakes/FakeLogger.hpp"


TEST(TcpServer, ListCommandReturnsSensorIds)
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
        2,
        200
    },
    SensorType::Motion,
    SensorState::ACTIVE,
    0.7
    };

    gateway.updateSensorInfo(r1);
    gateway.updateSensorInfo(r2);

    TcpServer server(gateway, logger, 8080);

    std::string response = server.processRequest("list");

    ASSERT_TRUE(response.find("1") != std::string::npos);
    ASSERT_TRUE(response.find("2") != std::string::npos);
}


TEST(TcpServer, ProcessRequest_GetValidSensor)
{
    FakeLogger logger;
    Gateway gateway(logger);

    TelemetryMessage r{
    {
        MessageType::TELEMETRY,
        1,
        200
    },
    SensorType::Motion,
    SensorState::ACTIVE,
    0.9
    };

    gateway.updateSensorInfo(r);

    TcpServer server(gateway, logger, 8080);

    std::string response = server.processRequest("get 1");

    ASSERT_TRUE(response.find("messageType : TELEMETRY") != std::string::npos);
    ASSERT_TRUE(response.find("sensorId    : 1") != std::string::npos);
    ASSERT_TRUE(response.find("timestamp   : 200") != std::string::npos);
    ASSERT_TRUE(response.find("sensorType  : Motion") != std::string::npos);
    ASSERT_TRUE(response.find("state       : ACTIVE") != std::string::npos);
    ASSERT_TRUE(response.find("value       : 0.9") != std::string::npos);
}


TEST(TcpServer, UnknownCommand)
{
    FakeLogger logger;
    Gateway gateway(logger);
    TcpServer server(gateway, logger, 8080);

    std::string response = server.processRequest("hello");

    ASSERT_EQ(response, "unknown command\n\n");
}


TEST(TcpServer, ProcessRequest_SensorNotFound)
{
    FakeLogger logger;
    Gateway gateway(logger);
    TcpServer server(gateway, logger, 8080);

    std::string response = server.processRequest("get 99");

    ASSERT_EQ(response, "sensor not found\n\n");
}