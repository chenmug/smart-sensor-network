#include "sensor/SensorNode.hpp"
#include "common/SensorTypesString.hpp"
#include "common/TimeUtils.hpp"
#include <thread>
#include <iostream>   // For debugging   


// /*************** CONSTRUCTOR ***************/

SensorNode::SensorNode(Sensor& sensor, IUdpSender& sender, ILogger& logger, 
        std::chrono::milliseconds telemetryInterval, std::chrono::milliseconds heartbeatInterval
    )
    : sensor(sensor), sender(sender), logger(logger), telemetryInterval(telemetryInterval), 
      heartbeatInterval(heartbeatInterval), running(false)
{}


// /****************** TICK *******************/

TelemetryMessage SensorNode::tick()
{
    sensor.collectData();
    const TelemetryMessage reading = sensor.createTelemetry();

    // logger.log("[SENSOR " + std::to_string(reading.header.sensorId) + "] Sending telemetry...");
    
    auto data = serializer.serialize(reading);
    sender.send(data); 

    return reading;
}


// /****************** RUN *******************/

void SensorNode::run()
{
    running = true;

    auto lastTelemetry = std::chrono::steady_clock::now();
    auto lastHeartbeat = std::chrono::steady_clock::now();

    while(running)
    {
        auto nowTime = std::chrono::steady_clock::now();

        // Telemetry timer
        if (nowTime - lastTelemetry >= telemetryInterval)
        {
            tick();
            lastTelemetry = nowTime;
        }

        // Heartbeat timer
        if (nowTime - lastHeartbeat >= heartbeatInterval)
        {
            sendHeartbeat();
            lastHeartbeat = nowTime;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


// /****************** STOP ******************/

void SensorNode::stop()
{
    running = false;
}


// /************* SEND HEARTBEAT *************/

void SensorNode::sendHeartbeat()
{
    HeartbeatMessage heartbeat;

    heartbeat.header.type = MessageType::HEARTBEAT;
    heartbeat.header.sensorId = sensor.getId();
    heartbeat.header.timestamp_ms = now();

    auto data = serializer.serialize(heartbeat);

    sender.send(data);
}