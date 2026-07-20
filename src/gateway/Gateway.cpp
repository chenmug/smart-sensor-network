#include "gateway/Gateway.hpp"
#include "common/TimeUtils.hpp"
#include "network/PacketSerializer.hpp"
#include "common/SensorTypesString.hpp" 
#include <algorithm>


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
    std::lock_guard<std::mutex> lock(mtx);

    SensorInfo& info = sensors[message.header.sensorId];
    
    info.lastTelemetry = message;
    info.lastTelemetryReceivedTime = now();
    ++telemetryPacketsReceived;
}


// /************** HANDLE HEARTBEAT **************/

void Gateway::handleHeartbeat(const HeartbeatMessage& message)
{
    std::lock_guard<std::mutex> lock(mtx);
    
    SensorInfo& info = sensors[message.header.sensorId];

    info.lastHeartbeatReceivedTime = now();
    info.health = SensorHealth::ONLINE;
    ++heartbeatPacketsReceived;
}


// /**************** GET SENSORS ****************/

std::unordered_map<uint32_t, Gateway::SensorInfo> Gateway::getSensors() const
{
    std::lock_guard<std::mutex> lock(mtx);
    
    return sensors;
}


// /********* CHECK FOR OFFLINE SENSORS *********/

void Gateway::detectOfflineSensors()
{   
    const uint64_t currentTime = now();
    std::vector<uint32_t> offlineSensors;

    std::lock_guard<std::mutex> lock(mtx);

    for (auto& [id, info] : sensors)
    {
        if (info.health == SensorHealth::UNKNOWN || info.lastHeartbeatReceivedTime == 0)
        {
            continue;
        }

        if (currentTime - info.lastHeartbeatReceivedTime > SENSOR_HEARTBEAT_TIMEOUT_MS)
        {
            if (info.health != SensorHealth::OFFLINE)
            {
                offlineSensors.push_back(id);
                info.health = SensorHealth::OFFLINE;

                logger.log("[GATEWAY] Sensor " + std::to_string(id) + " is offline");
            }
            
        }
    }
}


// /****** GET SECONDS SINCE LAST HEARTBEAT ******/

uint64_t Gateway::getSecondsSinceLastHeartbeat(uint32_t sensorId) const
{
    std::lock_guard<std::mutex> lock(mtx);

    const auto it = sensors.find(sensorId);
    if (it == sensors.end())
    {
        return 0;
    }

    const auto& info = it->second;

    if (info.lastHeartbeatReceivedTime == 0)
    {
        return 0;
    }

    return (now() - info.lastHeartbeatReceivedTime) / 1000;
}


// /******* GET TELEMETRY PACKETS RECEIVED *******/

uint64_t Gateway::getTelemetryPacketsReceived() const
{
    std::lock_guard<std::mutex> lock(mtx);
    
    return telemetryPacketsReceived;
}


// /******* GET HEARTBEAT PACKETS RECEIVED *******/

uint64_t Gateway::getHeartbeatPacketsReceived() const
{
    std::lock_guard<std::mutex> lock(mtx);

    return heartbeatPacketsReceived;
}