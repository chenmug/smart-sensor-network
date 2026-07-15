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
    template<typename T>
    void write(std::vector<uint8_t>& buffer, const T& value)
    {
        const size_t offset = buffer.size();
        buffer.resize(offset + sizeof(T));

        std::memcpy(buffer.data() + offset, &value, sizeof(T));
    }


    template<typename T>
    T read(const std::vector<uint8_t>& buffer, size_t& offset)
    {
        if (offset + sizeof(T) > buffer.size())
        {
            throw std::runtime_error("Buffer underflow during deserialization");
        }

        T value;
        std::memcpy(&value, buffer.data() + offset, sizeof(T));
        offset += sizeof(T);

        return value;
    }


    PacketHeader readHeader(const std::vector<uint8_t>& buffer, size_t& offset)
    {
        PacketHeader header;

        header.type = static_cast<MessageType>(read<int>(buffer, offset));
        header.sensorId = read<uint32_t>(buffer, offset);
        header.timestamp_ms = read<uint64_t>(buffer, offset);

        return header;
    }


    void writeHeader(std::vector<uint8_t>& buffer, const PacketHeader& header)
    {
        write(buffer, static_cast<int>(header.type));
        write(buffer, header.sensorId);
        write(buffer, header.timestamp_ms);
    }
}


// /**************** TELEMETRY SERIALIZE ****************/

std::vector<uint8_t> PacketSerializer::serialize(const TelemetryMessage& message)
{
    std::vector<uint8_t> buffer;
    buffer.reserve(
        sizeof(int) +
        sizeof(uint32_t) + 
        sizeof(uint64_t) +
        sizeof(int) + 
        sizeof(int) +
        sizeof(double) 
    );

    writeHeader(buffer, message.header);

    write(buffer, static_cast<int>(message.type));
    write(buffer, static_cast<int>(message.state));
    write(buffer, message.value);

    return buffer;
}


// /**************** HEARTBEAT SERIALIZE ****************/

std::vector<uint8_t> PacketSerializer::serialize(const HeartbeatMessage& message)
{
    std::vector<uint8_t> buffer;
    buffer.reserve(
        sizeof(int) +
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
    message.type = static_cast<SensorType>(read<int>(buffer, offset));
    message.state = static_cast<SensorState>(read<int>(buffer, offset));
    message.value = read<double>(buffer, offset);

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