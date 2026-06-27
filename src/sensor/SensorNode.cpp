#include "sensor/SensorNode.hpp"
#include "common/SensorTypesString.hpp"
#include <thread>
#include <iostream>   // For debugging   


// /*************** CONSTRUCTOR ***************/

SensorNode::SensorNode(Sensor& sensor, IUdpSender& sender, std::chrono::milliseconds intervalMs)
    : sensor(sensor), sender(sender), interval(intervalMs), running(false)
{}


// /****************** TICK *******************/

SensorReading SensorNode::tick()
{
    sensor.collectData();
    const SensorReading reading = sensor.createTelemetry();

    // Temporary debug logging - will be replaced by Monitor module
    std::cout
            << "[Sensor " << reading.sensorId << "] "
            << "Type: " << stringType(reading.type)
            << " Value: " << reading.value
            << " State: " << stringState(reading.state)
            << " Timestamp: " << reading.timestamp_ms
            << std::endl;
    
    auto data = serializer.serialize(reading);
    sender.send(data); 

    return reading;
}


// /****************** RUN *******************/

void SensorNode::run()
{
    running = true;

    while(running)
    {
        tick();
        std::this_thread::sleep_for(interval);

        // TODO:
        // update heartbeat
        // Add stop condition
    }
}


// /****************** STOP ******************/

void SensorNode::stop()
{
    running = false;
}