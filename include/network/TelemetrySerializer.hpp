#pragma once
#include "common/ProtocolTypes.hpp"  // Forward Declaration
#include <vector>                    // For std::vector
#include <cstdint>                   // For uint8_t

/**
 * @brief Handles serialization and deserialization of telemetry data.
 *
 * The TelemetrySerializer is responsible for converting TelemetryMessage objects into a binary 
 * format suitable for network transmission, and reconstructing TelemetryMessage objects from 
 * received binary data.
 *
 * This class is stateless and provides only static utility functions.
 */
class TelemetrySerializer
{
public:
    /**
     * @brief Serializes a TelemetryMessage into a binary buffer.
     *
     * Converts the structured telemetry message into a contiguous sequence of bytes
     * that can be transmitted over the network (e.g., via UDP/TCP).
     *
     * @param message The telemetry message to serialize.
     * @return A vector of bytes representing the serialized message.
     */
    static std::vector<uint8_t> serialize(const TelemetryMessage& message);

    /**
     * @brief Deserializes a binary buffer into a TelemetryMessage object.
     *
     * Reconstructs the original TelemetryMessage from a raw byte buffer received
     * over the network.
     *
     * @param buffer The binary data buffer.
     * @return The reconstructed TelemetryMessage object.
     */
    static TelemetryMessage deserialize(const std::vector<uint8_t>& buffer);
};