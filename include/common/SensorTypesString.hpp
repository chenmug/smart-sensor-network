#pragma once
#include "common/ProtocolTypes.hpp"  // Forward Declaration
#include <string>                    // For std::string

/**
 * @brief Converts a SensorType enum value to its string representation.
 *
 * This function maps each SensorType enum value to a human-readable string.
 * It is typically used for logging, debugging, and telemetry visualization.
 *
 * @param type The sensor type to convert.
 * @return A string representing the given SensorType value.
 */
std::string to_string(SensorType type);

/**
 * @brief Converts a SensorState enum value to its string representation.
 *
 * This function maps each SensorState enum value to a human-readable string that describes the
 * operational state of a sensor. It is mainly used for logging, monitoring, and system diagnostics.
 *
 * @param state The sensor state to convert.
 * @return A string representing the given SensorState value.
 */
std::string to_string(SensorState state);

/**
 * @brief Converts a MessageType enum value to its string representation.
 *
 * This function maps each MessageType enum value to a human-readable string.
 * It is typically used for logging, debugging, and telemetry visualization.
 *
 * @param type The message type to convert.
 * @return A string representing the given MessageType value.
 */
std::string to_string(MessageType type);

/**
 * @brief Converts a SensorHealth enum value to its string representation.
 *
 * This function maps each SensorHealth enum value to a human-readable string.
 * It is typically used for logging, debugging, and telemetry visualization.
 *
 * @param health The sensor health type to convert.
 * @return A string representing the given SensorHealth value.
 */
std::string to_string(SensorHealth health);