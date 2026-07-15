#pragma once
#include "common/ProtocolTypes.hpp"
#include <cstdint>
#include <vector>

/**
 * @brief Serializes and deserializes network messages.
 *
 * This utility class converts structured protocol messages to their binary representation for 
 * network transmission, and reconstructs them from raw byte buffers received over the network.
 *
 * The serializer currently supports:
 * - PacketMessage
 * - HeartbeatMessage
 *
 * All functions are stateless and provided as static utilities.
 */
class PacketSerializer
{
public:

    /**
     * @brief Returns the type of message stored in a binary packet.
     *
     * Reads only the packet header without fully deserializing the message.
     * This allows the receiver to determine which deserialization routine
     * should be used.
     *
     * @param buffer Raw binary packet.
     * @return The message type contained in the packet.
     *
     * @throws std::runtime_error if the packet is too small to contain a header.
     */
    static MessageType peekMessageType(const std::vector<uint8_t>& buffer);

    /**
     * @brief Serializes a telemetry message into a binary buffer.
     *
     * Converts a TelemetryMessage into a contiguous sequence of bytes suitable
     * for transmission over the network.
     *
     * @param message The telemetry message to serialize.
     * @return Serialized binary representation of the message.
     */
    static std::vector<uint8_t> serialize(const TelemetryMessage& message);

    /**
     * @brief Serializes a heartbeat message into a binary buffer.
     *
     * Converts a HeartbeatMessage into a compact binary representation for
     * transmission to the Gateway.
     *
     * @param message The heartbeat message to serialize.
     * @return Serialized binary representation of the message.
     */
    static std::vector<uint8_t> serialize(const HeartbeatMessage& message);

    /**
     * @brief Deserializes a binary buffer into a TelemetryMessage.
     *
     * Reconstructs a telemetry message from its serialized binary form.
     *
     * @param buffer Raw binary packet.
     * @return The reconstructed TelemetryMessage.
     */
    static TelemetryMessage deserializeTelemetry(const std::vector<uint8_t>& buffer);

    /**
     * @brief Deserializes a binary buffer into a HeartbeatMessage.
     *
     * Reconstructs a heartbeat message from its serialized binary form.
     *
     * @param buffer Raw binary packet.
     * @return The reconstructed HeartbeatMessage.
     */
    static HeartbeatMessage deserializeHeartbeat(const std::vector<uint8_t>& buffer);
};