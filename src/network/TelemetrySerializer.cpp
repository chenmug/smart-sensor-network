#include "network/TelemetrySerializer.hpp"
#include <cstring>    // For memcpy
#include <stdexcept>  // For std::runtime_error


/**
 * @brief Internal helper namespace for binary serialization utilities.
 *
 * This namespace contains functions used by the TelemetrySerializer to convert primitive types 
 * to and from their raw byte representation using memcpy. Intended for use only within this file.
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
}

// /**************** SERIALIZE ****************/

std::vector<uint8_t> TelemetrySerializer::serialize(const TelemetryMessage& message)
{
    std::vector<uint8_t> buffer;
    buffer.reserve(
        sizeof(MessageType) +
        sizeof(uint32_t) + 
        sizeof(int) + 
        sizeof(int) +
        sizeof(double) + 
        sizeof(uint64_t)
    );

    write(buffer, static_cast<int>(message.header.type));
    write(buffer, message.header.sensorId);
    write(buffer, message.header.timestamp_ms);

    write(buffer, static_cast<int>(message.type));
    write(buffer, static_cast<int>(message.state));
    write(buffer, message.value);

    return buffer;
}


// /*************** DESERIALIZE ***************/

TelemetryMessage TelemetrySerializer::deserialize(const std::vector<uint8_t>& buffer)
{
    TelemetryMessage message;
    size_t offset = 0;
    
    message.header.type = static_cast<MessageType>(read<int>(buffer, offset));
    message.header.sensorId = read<uint32_t>(buffer, offset);
    message.header.timestamp_ms = read<uint64_t>(buffer, offset);
    
    message.type = static_cast<SensorType>(read<int>(buffer, offset));
    message.state = static_cast<SensorState>(read<int>(buffer, offset));
    message.value = read<double>(buffer, offset);

    return message;
}