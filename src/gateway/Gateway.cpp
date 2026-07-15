#include "gateway/Gateway.hpp"
#include "common/TimeUtils.hpp"
#include "network/PacketSerializer.hpp"
#include "common/SensorTypesString.hpp"  // For debugging 
#include <iostream>                      // For debugging


// /**************** CONSTRUCTOR *****************/

Gateway::Gateway(ILogger& logger)
    : logger(logger)
{}


// /*************** HANDLE PACKET ***************/

void Gateway::handlePacket(const std::vector<uint8_t>& packet)
{
    switch (serializer.peekMessageType(packet))
    {
    case MessageType::TELEMETRY:
    {
        TelemetryMessage msg = serializer.deserializeTelemetry(packet);
        updateSensorInfo(msg);
        break;
    }

    case MessageType::HEARTBEAT:
    {
        HeartbeatMessage hb = serializer.deserializeHeartbeat(packet);
        handleHeartbeat(hb);
        break;
    }

    default:
        logger.log("[GATEWAY] Unknown message type");
        break;
    }
}


// /************* UPDATE SENSOR INFO *************/

void Gateway::updateSensorInfo(const TelemetryMessage& message)
{
    SensorInfo& info = sensors[message.header.sensorId];
    
    info.lastTelemetry = message;
    info.lastTelemetryTime = now();

    logger.log("[GATEWAY] Updated sensor " +
               std::to_string(message.header.sensorId) +
               " (" + to_string(message.type) + ")");
}


// /************** HANDLE HEARTBEAT **************/

void Gateway::handleHeartbeat(const HeartbeatMessage& message)
{
    SensorInfo& info = sensors[message.header.sensorId];

    info.lastHeartbeatTime = now();

    logger.log( "[GATEWAY] Heartbeat received from sensor " +
                 std::to_string(message.header.sensorId));
}


// /**************** GET SENSORS ****************/

const std::unordered_map<uint32_t, Gateway::SensorInfo>& Gateway::getSensors() const
{
    return sensors;
}


// /********** GET LAST HEARTBEAT TIME **********/

uint64_t Gateway::getLastHeartbeatTime(uint32_t sensorId) const
{
    return sensors.at(sensorId).lastHeartbeatTime;
}