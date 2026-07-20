#pragma once
#include "gateway/Gateway.hpp"       // Forward Declaration
#include "common/ProtocolTypes.hpp"  // Forward Declaration
#include <string>                    // For std::string
#include <cstdint>                   // For uint32_t


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

    /**
     * @brief Generates a summary of the current health status of all sensors.
     *
     * Produces a formatted report containing the number of online, offline,
     * and unknown sensors, followed by a list of all sensors currently
     * marked as offline.
     *
     * @return Formatted health summary for the monitoring interface.
     */
    std::string healthSummary() const;

    /**
     * @brief Generates a summary of the current system statistics.
     *
     * Produces a formatted report containing general runtime statistics such as
     * the total number of registered sensors and the number of telemetry and
     * heartbeat messages processed by the Gateway.
     *
     * @return Formatted system statistics report.
     */
    std::string stats() const;

private:
    /**
     * @brief Counts sensors by their current health status.
     *
     * Iterates over all registered sensors in the Gateway and calculates the number of sensors that 
     * are currently online, offline, and in an unknown health state.
     *
     * @return A HealthCounts structure containing the number of online, offline, and unknown sensors.
     */
    HealthCounts countHealthSensors() const;
};