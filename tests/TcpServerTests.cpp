#include <gtest/gtest.h>
#include "network/TcpServer.hpp"


TEST(TcpServer, ListCommandReturnsSensorIds)
{
    Gateway gateway;

    SensorReading r1{1, SensorType::Motion, SensorState::ACTIVE, 0.5, 100};
    SensorReading r2{2, SensorType::Motion, SensorState::ACTIVE, 0.7, 200};

    gateway.updateSensorInfo(r1);
    gateway.updateSensorInfo(r2);

    TcpServer server(gateway, 8080);

    std::string response = server.processRequest("list");

    ASSERT_TRUE(response.find("1") != std::string::npos);
    ASSERT_TRUE(response.find("2") != std::string::npos);
}


TEST(TcpServer, ProcessRequest_GetValidSensor)
{
    Gateway gateway;

    SensorReading r{1, SensorType::Motion, SensorState::ACTIVE, 0.9, 200};
    gateway.updateSensorInfo(r);

    TcpServer server(gateway, 8080);

    std::string response = server.processRequest("get 1");

    ASSERT_TRUE(response.find("id=1") != std::string::npos);
    ASSERT_TRUE(response.find("value=0.9") != std::string::npos);
    ASSERT_TRUE(response.find("timestamp=200") != std::string::npos);
}


TEST(TcpServer, UnknownCommand)
{
    Gateway gateway;
    TcpServer server(gateway, 8080);

    std::string response = server.processRequest("hello");

    ASSERT_EQ(response, "unknown command\n");
}


TEST(TcpServer, ProcessRequest_SensorNotFound)
{
    Gateway gateway;
    TcpServer server(gateway, 8080);

    std::string response = server.processRequest("get 99");

    ASSERT_EQ(response, "sensor not found\n");
}