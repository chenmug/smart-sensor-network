#include "sensor/SensorNode.hpp"
#include "common/SensorTypesString.hpp"
#include <thread>
#include <iostream>   // For debugging   


// /*************** CONSTRUCTOR ***************/

SensorNode::SensorNode(Sensor& sensor, IUdpSender& sender, ILogger& logger, std::chrono::milliseconds intervalMs)
    : sensor(sensor), sender(sender), logger(logger), interval(intervalMs), running(false)
{}


// /****************** TICK *******************/

TelemetryMessage SensorNode::tick()
{
    sensor.collectData();
    const TelemetryMessage reading = sensor.createTelemetry();

    logger.log("[SENSOR " + std::to_string(reading.header.sensorId) + "] Sending telemetry...");
    
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