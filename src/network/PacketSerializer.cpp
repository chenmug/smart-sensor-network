#include "network/PacketSerializer.hpp"
#include <cstring>    // For memcpy
#include <stdexcept>  // For std::runtime_error


/**
 * @brief Internal helper namespace for binary serialization utilities.
 *
 * This namespace contains helper functions used by PacketSerializer to convert primitive data 
 * types and common packet structures to and from their binary representation.
 *
 * These functions are intended for internal use only within this translation unit.
 */
namespace
{
    constexpr size_t BITS_PER_BYTE = 8;  // Number of bits in 1 byte, used when shifting between byte positions
    constexpr uint8_t BYTE_MASK = 0xFF;  // Mask used to extract the lowest 8 bits (1 byte) from an integer

    template<typename T>
    void write(std::vector<uint8_t>& buffer, const T& value)
    {
        for (size_t i = sizeof(T); i > 0; --i)
        {
            buffer.push_back(static_cast<uint8_t>(value >> ((i - 1) * BITS_PER_BYTE) & BYTE_MASK));
        }
    }


    template<typename T>
    T read(const std::vector<uint8_t>& buffer, size_t& offset)
    {
        if (offset + sizeof(T) > buffer.size())
        {
            throw std::runtime_error("Buffer underflow during deserialization");
        }

        T value = 0;

        for (size_t i = 0; i < sizeof(T); ++i)
        {
            value = static_cast<T>((value << 8) | static_cast<T>(buffer[offset++]));
        }

        return value;
    }

    
    void writeDouble(std::vector<uint8_t>& buffer, double value)
    {
        uint64_t bits;
        std::memcpy(&bits, &value, sizeof(bits));

        write(buffer, bits);
    }


    double readDouble(const std::vector<uint8_t>& buffer, size_t& offset)
    {
        uint64_t bits = read<uint64_t>(buffer, offset);

        double value;
        std::memcpy(&value, &bits, sizeof(value));

        return value;
    }


    PacketHeader readHeader(const std::vector<uint8_t>& buffer, size_t& offset)
    {
        PacketHeader header;

        header.type = static_cast<MessageType>(read<uint8_t>(buffer, offset));
        header.sensorId = read<uint32_t>(buffer, offset);
        header.timestamp_ms = read<uint64_t>(buffer, offset);

        return header;
    }


    void writeHeader(std::vector<uint8_t>& buffer, const PacketHeader& header)
    {
        write(buffer, static_cast<uint8_t>(header.type));
        write(buffer, header.sensorId);
        write(buffer, header.timestamp_ms);
    }
}


// /**************** TELEMETRY SERIALIZE ****************/

std::vector<uint8_t> PacketSerializer::serialize(const TelemetryMessage& message)
{
    std::vector<uint8_t> buffer;
    buffer.reserve(
        sizeof(uint8_t) +
        sizeof(uint32_t) + 
        sizeof(uint64_t) +
        sizeof(uint8_t) + 
        sizeof(uint8_t) +
        sizeof(double) 
    );

    writeHeader(buffer, message.header);

    write(buffer, static_cast<uint8_t>(message.type));
    write(buffer, static_cast<uint8_t>(message.state));

    writeDouble(buffer, message.value);

    return buffer;
}


// /**************** HEARTBEAT SERIALIZE ****************/

std::vector<uint8_t> PacketSerializer::serialize(const HeartbeatMessage& message)
{
    std::vector<uint8_t> buffer;
    buffer.reserve(
        sizeof(uint8_t) +
        sizeof(uint32_t) +
        sizeof(uint64_t)
    );

    writeHeader(buffer, message.header);

    return buffer;
}


// /*************** TELEMETRY DESERIALIZE ***************/

TelemetryMessage PacketSerializer::deserializeTelemetry(const std::vector<uint8_t>& buffer)
{
    TelemetryMessage message;
    size_t offset = 0;

    message.header = readHeader(buffer, offset);
    message.type = static_cast<SensorType>(read<uint8_t>(buffer, offset));
    message.state = static_cast<SensorState>(read<uint8_t>(buffer, offset));
    message.value = readDouble(buffer, offset);

    return message;
}


// /*************** HEARTBEAT DESERIALIZE ***************/

HeartbeatMessage PacketSerializer::deserializeHeartbeat(const std::vector<uint8_t>& buffer)
{
    HeartbeatMessage message;
    size_t offset = 0;

    message.header = readHeader(buffer, offset);

    return message;
}


// /**************** PEEK MESSAGE TYPE *****************/

MessageType PacketSerializer::peekMessageType(const std::vector<uint8_t>& buffer)
{
    size_t offset = 0;

    return readHeader(buffer, offset).type;
}