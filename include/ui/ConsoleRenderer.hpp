#pragma once
#include "monitor/Logger.hpp"
#include "gateway/Gateway.hpp"
#include "network/TcpServer.hpp"
#include "monitor/Monitor.hpp"
#include <string>


/**
 *
 * @brief Provides a real-time console dashboard for the sensor network.
 *
 * Displays system logs, TCP monitoring status, sensor statistics,
 * health information, and architecture overview.
 *
 * The renderer acts as a presentation layer and only reads data from
 * system components without modifying their state.
 */
class ConsoleRenderer
{
private:

    Logger& logger;         // Logger providing recent runtime events.
    TcpServer& tcpServer;   // TCP server providing monitoring connection status.
    Monitor& monitor;       // Monitor providing formatted system statistics.

public:
    /**
     * @brief Constructs a console renderer.
     *
     * Stores references to the system components required for displaying
     * the monitoring dashboard.
     *
     * @param logger Reference to the Logger providing system events.
     * @param tcpServer Reference to the TCP server monitoring client activity.
     * @param monitor Reference to the Monitor providing system reports.
     */
    ConsoleRenderer(Logger& logger, TcpServer& tcpServer, Monitor& monitor);

    /**
     * @brief Renders the complete monitoring dashboard.
     *
     * Clears the console and displays the current system status including:
     *
     * - Recent system logs
     * - TCP monitoring information
     * - Sensor statistics
     * - System architecture overview
     *
     * This function only reads data from system components and does not
     * modify their internal state.
     */
    void render();

private:
    /**
     * @brief Renders the application title section.
     */
    void renderHeader() const;

    /**
     * @brief Renders the log and monitoring panels.
     *
     * Displays recent Logger messages alongside TCP client information and system statistics provided by Monitor.
     */
    void renderLogs() const;

    /**
     * @brief Renders the system architecture overview.
     */
    void renderArchitecture() const;

    /**
     * @brief Renders the dashboard footer section.
     */
    void renderFooter() const;
};