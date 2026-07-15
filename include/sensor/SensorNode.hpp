#pragma once
#include "sensor/Sensor.hpp"                // Forward Declaration
#include "network/PacketSerializer.hpp"  // Forward Declaration
#include "network/IUdpSender.hpp"           // Forward Declaration
#include "monitor/ILogger.hpp"              // Forward Declaration
#include <chrono>                           // For std::chrono::milliseconds
#include <atomic>                           // For std::atomic


/**
 * @brief Runs a periodic sensor loop and handles telemetry transmission over UDP.
 *
 * SensorNode bridges the sensor layer and the networking layer.
 * It collects sensor data, converts it into telemetry packets, and sends them over UDP to a gateway.
 */
class SensorNode
{
private:
    Sensor& sensor;                      // External sensor used to collect telemetry data
    IUdpSender& sender;                  // Sends serialized telemetry packets over UDP
    std::chrono::milliseconds interval;  // Time between sensor updates
    std::atomic<bool> running;           // Controls main loop execution
    PacketSerializer serializer;         // Converts packet data into bytes
    ILogger& logger;                     // Reference to the shared system logger used for thread-safe system logging.

public:
    /**
     * @brief Constructs a SensorNode with a sensor and UDP destination.
     *
     * @param sensor Pointer to a sensor instance.
     * @param sender UDP sender responsible for transmission.
     * @param logger Reference to the shared system Logger.
     * @param intervalMs Time between telemetry transmissions.
     */
    SensorNode(Sensor& sensor, IUdpSender& sender, ILogger& logger, std::chrono::milliseconds intervalMs = std::chrono::milliseconds(500));
    
    /**
     * @brief Executes a single sensor cycle (one iteration of the pipeline).
     *
     * Collects sensor data, serializes it, and sends it over UDP.
     * Used for testing and controlled execution.
     *
     * @return The telemetry message produced during this cycle.
     */
    TelemetryMessage tick();

    /**
     * @brief Starts the sensor execution loop.
     *
     * Continuously collects sensor data, serializes it, and sends it over UDP.
     */
    void run();

    /**
     * @brief Stops the sensor loop safely.
     */
    void stop();
};