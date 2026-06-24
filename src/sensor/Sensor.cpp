#include "sensor/Sensor.hpp"
#include "common/TimeUtils.hpp"


// /*************** CONSTRUCTOR ***************/

Sensor::Sensor(uint32_t id)
    : sensorId(id), currentState(SensorState::ACTIVE), lastHeartbeatTime(0)
{}


// /*************** UPDATE STATE **************/

void Sensor::updateState(SensorState state)
{
    currentState = state;
}


// /**************** RUN LOOP *****************/

void Sensor::runLoop()
{
    while (true)
    {
        SensorReading reading = createTelemetry();

        // TODO:
        // send to gateway (UDP)
        // sleep interval
        // update heartbeat
    }
}


// /************* CREATE TELEMETRY *************/

SensorReading Sensor::createTelemetry() 
{
    SensorReading reading;

    reading.sensorId = sensorId;
    reading.state = currentState;
    reading.type = getType();
    reading.value = collectData();
    reading.timestamp_ms = now();

    return reading;
}