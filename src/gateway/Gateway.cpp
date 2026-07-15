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
    TelemetryMessage reading = serializer.deserializeTelemetry(packet);
    updateSensorInfo(reading);
}


// /************* UPDATE SENSOR INFO *************/

void Gateway::updateSensorInfo(const TelemetryMessage& reading)
{
    SensorInfo& info = sensors[reading.header.sensorId];
    
    info.lastReading = reading;
    info.lastUpdateTime = now();

    logger.log("[GATEWAY] Updated sensor " +
               std::to_string(reading.header.sensorId) +
               " (" + to_string(reading.type) + ")");
}


// /**************** GET SENSORS ****************/

const std::unordered_map<uint32_t, Gateway::SensorInfo>& Gateway::getSensors() const
{
    return sensors;
}