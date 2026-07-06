#include "gateway/Gateway.hpp"
#include "common/TimeUtils.hpp"
#include "network/TelemetrySerializer.hpp"
#include "common/SensorTypesString.hpp"  // For debugging 
#include <iostream>                      // For debugging


// /**************** CONSTRUCTOR *****************/

Gateway::Gateway(ILogger& logger)
    : logger(logger)
{}


// /*************** HANDLE PACKET ***************/

void Gateway::handlePacket(const std::vector<uint8_t>& packet)
{
    SensorReading reading = serializer.deserialize(packet);
    updateSensorInfo(reading);
}


// /************* UPDATE SENSOR INFO *************/

void Gateway::updateSensorInfo(const SensorReading& reading)
{
    SensorInfo& info = sensors[reading.sensorId];
    
    info.lastReading = reading;
    info.lastUpdateTime = now();

    logger.log("[GATEWAY] Updated sensor " +
               std::to_string(reading.sensorId) +
               " (" + stringType(reading.type) + ")");
}


// /**************** GET SENSORS ****************/

const std::unordered_map<uint32_t, Gateway::SensorInfo>& Gateway::getSensors() const
{
    return sensors;
}