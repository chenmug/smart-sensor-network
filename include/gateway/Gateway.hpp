#pragma once
#include "network/TelemetrySerializer.hpp"  // Forward Declaration
#include "common/ProtocolTypes.hpp"         // Forward Declaration
#include <unordered_map>                    // For std::unordered_map
#include <cstdint>                          // For uint8_t, uint64_t
#include <vector>                           // for std::vector

/**
 * @brief Central system that receives telemetry from sensors and stores their state.
 *
 * Gateway acts as the entry point for all sensor data in the system.
 * It maintains the latest known state of each sensor.
 *
 * Responsibilities:
 * - Receiving incoming telemetry packets
 * - Decoding binary data into SensorReading objects
 * - Maintaining the latest state per sensor
 */
class Gateway
{
private:
    /**
     * @brief Internal representation of a single sensor's latest known state.
     *
     * This structure stores the most recent telemetry information received
     * from a given sensor, along with metadata about when it was last updated.
     */
    struct SensorInfo
    {
        SensorReading lastReading;  // The last telemetry reading received from this sensor.
        uint64_t lastUpdateTime;    // Timestamp (milliseconds) when the gateway last received telemetry from this sensor.
    };

    std::unordered_map<uint32_t, SensorInfo> sensors;  // Registry of all sensors known to the gateway.
    TelemetrySerializer serializer;                    // Converts bytes into telemetry data into 
    
public:
    /**
     * @brief Constructs an empty Gateway instance.
     *
     * Initializes the internal sensor registry.
     */
    Gateway() = default;

    /**
     * @brief Handles an incoming raw UDP packet.
     *
     * This function is responsible for receiving binary telemetry data, decoding it into a 
     * SensorReading object, and forwarding it to updateSensor().
     *
     * @param packet Raw binary telemetry data received over UDP.
     */
    void handlePacket(const std::vector<uint8_t>& packet);

    /**
     * @brief Updates the sensor info.
     *
     * Stores the latest telemetry reading for a given sensor and updates its last update timestamp.
     *
     * - If sensor does not exist - it is created
     * - If sensor exists - its state is overwritten with latest reading
     *
     * @param reading Decoded telemetry data from a sensor.
     */
    void updateSensorInfo(const SensorReading& reading);

    /**
     * @brief Provides read-only access to the internal sensor registry.
     *
     * This function is intended for testing and debugging purposes only,
     * and exposes the current state of all sensors stored in the Gateway.
     *
     * @return Const reference to the map of sensor IDs and their latest state.
     */
    const std::unordered_map<uint32_t, SensorInfo>& getSensors() const;
};