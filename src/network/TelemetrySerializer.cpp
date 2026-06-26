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

std::vector<uint8_t> TelemetrySerializer::serialize(const SensorReading& reading)
{
    std::vector<uint8_t> buffer;
    buffer.reserve(
        sizeof(uint32_t) + 
        sizeof(int) + 
        sizeof(int) +
        sizeof(double) + 
        sizeof(uint64_t)
    );

    write(buffer, reading.sensorId);
    write(buffer, static_cast<int>(reading.type));
    write(buffer, static_cast<int>(reading.state));
    write(buffer, reading.value);
    write(buffer, reading.timestamp_ms);

    return buffer;
}


// /*************** DESERIALIZE ***************/

SensorReading TelemetrySerializer::deserialize(const std::vector<uint8_t>& buffer)
{
    SensorReading reading;
    size_t offset = 0;
    
    reading.sensorId = read<uint32_t>(buffer, offset);
    reading.type = static_cast<SensorType>(read<int>(buffer, offset));
    reading.state = static_cast<SensorState>(read<int>(buffer, offset));
    reading.value = read<double>(buffer, offset);
    reading.timestamp_ms = read<uint64_t>(buffer, offset);

    return reading;
}