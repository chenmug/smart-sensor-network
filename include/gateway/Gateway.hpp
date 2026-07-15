#pragma once
#include "network/PacketSerializer.hpp"  // Forward Declaration
#include "common/ProtocolTypes.hpp"         // Forward Declaration
#include "monitor/ILogger.hpp"              // Forward Declaration
#include <unordered_map>                    // For std::unordered_map
#include <cstdint>                          // For uint8_t, uint64_t
#include <vector>                           // for std::vector


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
     * This structure stores the most recent telemetry message received from a
     * sensor together with timestamps used to monitor communication health.
     */
    struct SensorInfo
    {
        TelemetryMessage lastTelemetry;  // Most recent telemetry message received from this sensor.
        uint64_t lastTelemetryTime;      // Timestamp when the latest telemetry message was received.
        uint64_t lastHeartbeatTime;      // Timestamp when the latest heartbeat message was received.
    };

    std::unordered_map<uint32_t, SensorInfo> sensors;  // Registry of all sensors known to the Gateway.
    PacketSerializer serializer;                       // Serializes and deserializes protocol packets.
    ILogger& logger;                                   // Shared thread-safe logger.

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
     * @return Constant reference to the internal sensor registry.
     */
    const std::unordered_map<uint32_t, SensorInfo>& getSensors() const;

    /**
     * @brief Returns the last heartbeat time received from the sensor.
     *
     * @param sensorId The sensor uniqe identifier.
     * @return The last heartbeat time.
     */
    uint64_t getLastHeartbeatTime(uint32_t sensorId) const;
};