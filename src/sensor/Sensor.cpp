#include "sensor/Sensor.hpp"
#include "common/TimeUtils.hpp"          // For now()
#include "common/SensorTypesString.hpp"  // For using strings functions
#include <thread>                        // For std::this_thread::sleep_for()
#include <iostream>                      // For debugging     


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
    const std::chrono::milliseconds tickRate(500);
    bool running = true;
    
    while (running)
    {
        collectData();
        SensorReading reading = createTelemetry();

        std::cout
            << "[Sensor " << sensorId << "] "
            << "Type: " << stringType(reading.type)
            << " Value: " << reading.value
            << " State: " << stringState(reading.state)
            << std::endl;

        std::this_thread::sleep_for(tickRate);

        // TODO:
        // send to gateway (UDP)
        // update heartbeat
        // Add stop condition
    }
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