#pragma once
#include <cstdint>  // uint32_t, uint64_t

/**
 * @brief Defines the different types of sensors in the system.
 *
 * Each sensor type represents a different physical or simulated data source,
 * and determines how telemetry values are generated and interpreted.
 */
enum class SensorType 
{
    Motion,
    Temperature,
    Battery,
    Pressure
};

/**
 * @brief Represents the operational state of a sensor.
 *
 * The state reflects the health and validity of the sensor based on its internal logic and 
 * readings. Each sensor type defines its own rules for transitioning between states.
 */
enum class SensorState
{
    ACTIVE,   // Sensor is operating normally and producing valid readings
    WARNING,  // Sensor is operating outside optimal range 
    ERROR     // Sensor is in a critical failure state and may stop reporting data
};

/**
 * @brief Defines the different message types exchanged in the system.
 *
 * Each message type represents a different kind of communication between
 * sensor nodes and the Gateway. The message type determines how the payload
 * should be interpreted after deserialization.
 */
enum class MessageType
{
    TELEMETRY,  // Sensor measurement and operational state
    HEARTBEAT   // Periodic liveness notification
};

/**
 * @brief Common header shared by all network messages.
 *
 * Every message transmitted between sensor nodes and the Gateway begins
 * with this header. It identifies the message type, the originating sensor,
 * and the time at which the message was generated.
 */
struct PacketHeader
{
    MessageType type;       // Type of message contained in this packet
    uint32_t sensorId;      // Unique identifier of the sensor
    uint64_t timestamp_ms;  // Timestamp when the message was generated
};

/**
 * @brief Heartbeat message sent periodically by a sensor.
 *
 * Heartbeat messages contain no telemetry data. Their purpose is to inform
 * the Gateway that the sensor is still operational and reachable.
 */
struct HeartbeatMessage
{
    PacketHeader header;  // The shared header of network messages
};

/**
 * @brief Telemetry message containing the latest sensor measurement.
 *
 * Telemetry messages are periodically transmitted by sensor nodes and contain
 * the latest measured value together with the sensor's operational state.
 * The Gateway uses these messages to maintain the latest known state of each sensor.
 */
struct TelemetryMessage
{
    PacketHeader header;  // The shared header of network messages
    SensorType type;      // Type of sensor that produced the measurement
    SensorState state;    // Current operational state of the sensor
    double value;         // Measured sensor value
};