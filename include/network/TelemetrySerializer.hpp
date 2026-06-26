#pragma once
#include "common/ProtocolTypes.hpp"  // Forward Declaration
#include <vector>                    // For std::vector
#include <cstdint>                   // For uint8_t

/**
 * @brief Handles serialization and deserialization of telemetry data.
 *
 * The TelemetrySerializer is responsible for converting SensorReading objects into a binary 
 * format suitable for network transmission, and reconstructing SensorReading objects from 
 * received binary data.
 *
 * This class is stateless and provides only static utility functions.
 */
class TelemetrySerializer
{
public:
    /**
     * @brief Serializes a SensorReading into a binary buffer.
     *
     * Converts the structured sensor reading into a contiguous sequence of bytes
     * that can be transmitted over the network (e.g., via UDP/TCP).
     *
     * @param reading The sensor reading to serialize.
     * @return A vector of bytes representing the serialized reading.
     */
    static std::vector<uint8_t> serialize(const SensorReading& reading);

    /**
     * @brief Deserializes a binary buffer into a SensorReading object.
     *
     * Reconstructs the original SensorReading from a raw byte buffer received
     * over the network.
     *
     * @param buffer The binary data buffer.
     * @return The reconstructed SensorReading object.
     */
    static SensorReading deserialize(const std::vector<uint8_t>& buffer);
};