#pragma once
#include "gateway/Gateway.hpp"  // Forward Declaration
#include <string>               // For std::string
#include <cstdint>              // For uint32_t


/**
 * @brief Provides monitoring and reporting functionality for the sensor network.
 *
 * The Monitor acts as a presentation layer between the Gateway and external
 * monitoring clients. It retrieves sensor information from the Gateway and
 * formats it into human-readable responses for display through the TCP interface.
 *
 * Responsibilities:
 * - Generating sensor list reports
 * - Displaying detailed sensor information
 * - Formatting Gateway state for monitoring clients
 */
class Monitor
{
private:

    Gateway& gateway;  // Gateway providing access to sensor state information.

public:
    /**
     * @brief Constructs a Monitor instance.
     *
     * @param gateway Reference to the Gateway containing sensor information.
     */
    Monitor(Gateway& gateway);

    /**
     * @brief Generates a summary list of all registered sensors.
     *
     * Creates a formatted table containing basic information about each sensor,
     * including its identifier, type, state, health status, and heartbeat age.
     *
     * @return Formatted sensor list as a string.
     */
    std::string sensorList() const;

    /**
     * @brief Generates detailed information for a specific sensor.
     *
     * Retrieves the latest telemetry data and heartbeat status of the requested
     * sensor and formats it for display.
     *
     * @param id Unique identifier of the sensor.
     *
     * @return Formatted sensor information as a string.
     */
    std::string sensorDetails(uint32_t id) const;

    /**
     * @brief Generates the help text for the monitoring interface.
     *
     * Used by TCP clients to discover available commands and their usage.
     *
     * @return Formatted help message containing supported commands.
     */
    std::string help() const;
};