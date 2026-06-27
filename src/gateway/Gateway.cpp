#include "gateway/Gateway.hpp"
#include "common/TimeUtils.hpp"
#include "network/TelemetrySerializer.hpp"


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
}