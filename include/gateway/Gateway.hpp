#pragma once
#include "network/PacketSerializer.hpp"  // Forward Declaration
#include "common/ProtocolTypes.hpp"      // Forward Declaration
#include "monitor/ILogger.hpp"           // Forward Declaration
#include <unordered_map>                 // For std::unordered_map
#include <cstdint>                       // For uint8_t, uint64_t
#include <vector>                        // for std::vector
#include <mutex>                         // For std::mutex  


/**
 * @brief Central component responsible for receiving and processing sensor messages.
 *
 * The Gateway acts as the entry point for all communication from sensor nodes.
 * It receives incoming network packets, dispatches them according to their
 * message type, and maintains the latest known state of every sensor.
 *
 * Responsibilities:
 * - Receiving incoming network packets
 * - Decoding telemetry and heartbeat messages
 * - Maintaining the latest telemetry for each sensor
 * - Tracking the most recent heartbeat received from each sensor
 */
class Gateway
{
private:

    /**
     * @brief Internal representation of the latest information known about a sensor.
     *
     * Stores the most recent telemetry data, communication timestamps, and current
     * health status of a sensor as observed by the Gateway.
     */
    struct SensorInfo
    {
        TelemetryMessage lastTelemetry{};             // Most recent telemetry message received from this sensor.
        uint64_t lastTelemetryReceivedTime = 0;       // Timestamp when the latest telemetry message was received.
        uint64_t lastHeartbeatReceivedTime = 0;       // Timestamp when the latest heartbeat message was received.
        SensorHealth health = SensorHealth::UNKNOWN;  // Initial state before receiving the first heartbeat.
    };

    std::unordered_map<uint32_t, SensorInfo> sensors;  // Registry of all sensors known to the Gateway.
    PacketSerializer serializer;                       // Serializes and deserializes protocol packets.
    ILogger& logger;                                   // Shared thread-safe logger.
    mutable std::mutex mtx;                            // Protect Gateway sensor registry with mutex.
    uint64_t telemetryPacketsReceived = 0;             // Total number of telemetry packets received.
    uint64_t heartbeatPacketsReceived = 0;             // Total number of heartbeat packets received.

public:

    /**
     * @brief Constructs a Gateway instance.
     *
     * Initializes the internal sensor registry and prepares the Gateway
     * to receive and process incoming sensor messages.
     *
     * @param logger Reference to the shared system logger.
     */
    Gateway(ILogger& logger);

    /**
     * @brief Processes an incoming network packet.
     *
     * Determines the packet type, deserializes it into the appropriate
     * message structure, and dispatches it to the corresponding handler.
     *
     * @param packet Raw packet received from the network.
     */
    void handlePacket(const std::vector<uint8_t>& packet);

    /**
     * @brief Updates the latest telemetry information for a sensor.
     *
     * If the sensor is not already present in the registry, a new entry is
     * created. Otherwise, the previous telemetry information is replaced by
     * the newly received message.
     *
     * @param message Decoded telemetry message.
     */
    void updateSensorInfo(const TelemetryMessage& message);

    /**
     * @brief Processes a heartbeat message received from a sensor.
     *
     * Updates the timestamp of the latest heartbeat received from the sensor.
     * This information is later used to detect offline devices.
     *
     * @param message Decoded heartbeat message.
     */
    void handleHeartbeat(const HeartbeatMessage& message);

    /**
     * @brief Provides read-only access to the internal sensor registry.
     *
     * @return Copy to the internal sensor registry.
     */
    std::unordered_map<uint32_t, SensorInfo> getSensors() const;

    /**
     * @brief Checks all registered sensors for heartbeat timeout.
     *
     * Iterates over all known sensors and compares their last received heartbeat
     * time with the current system time. Sensors that did not send a heartbeat
     * within the allowed timeout period are considered offline.
     */
    void detectOfflineSensors();

    /**
     * @brief Returns the time elapsed since the last received heartbeat.
     *
     * Used by monitoring components to display sensor heartbeat age.
     *
     * @param sensorId ID of the sensor to query.
     * @return Elapsed time in seconds since the last heartbeat was received.
     */
    uint64_t getSecondsSinceLastHeartbeat(uint32_t sensorId) const;

    /**
     * @brief Returns the total number of telemetry packets processed.
     *
     * @return Number of telemetry packets received by the Gateway.
     */
    uint64_t getTelemetryPacketsReceived() const;

    /**
     * @brief Returns the total number of heartbeat messages processed.
     *
     * @return Number of heartbeat messages received by the Gateway.
     */
    uint64_t getHeartbeatPacketsReceived() const;
};