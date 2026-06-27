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

SensorReading Sensor::createTelemetry() 
{
    SensorReading reading;

    reading.sensorId = sensorId;
    reading.state = currentState;
    reading.type = getType();
    reading.value = currentValue;
    reading.timestamp_ms = now();

    currentReading = reading;
    return reading;
}