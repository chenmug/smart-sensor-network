#include "sensor/Sensor.hpp"
#include "common/TimeUtils.hpp"          // For now()
#include "common/SensorTypesString.hpp"  // For using strings functions   


// /*************** CONSTRUCTOR ***************/

Sensor::Sensor(uint32_t id)
    : sensorId(id), currentState(SensorState::ACTIVE), lastHeartbeatTime(0)
{}


// /*************** UPDATE STATE **************/

void Sensor::updateState(SensorState state)
{
    currentState = state;
}


// /************* CREATE TELEMETRY *************/

TelemetryMessage Sensor::createTelemetry() 
{
    TelemetryMessage message;

    message.header.type = MessageType::TELEMETRY;
    message.header.sensorId = sensorId;
    message.state = currentState;
    message.type = getType();
    message.value = currentValue;
    message.header.timestamp_ms = now();

    currentReading = message;
    return message;
}