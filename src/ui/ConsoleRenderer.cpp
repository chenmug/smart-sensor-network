#include "ui/ConsoleRenderer.hpp"
#include "common/ProtocolTypes.hpp"
#include <iostream>
#include <iomanip>


/**
 * @brief Internal utilities for formatting the console dashboard layout.
 *
 * Defines column widths and helper functions used to align and truncate
 * displayed text inside the dashboard table.
 */
namespace
{
    constexpr int SYSTEM_WIDTH = 75;
    constexpr int TCP_WIDTH = 24;

    /**
     * @brief Formats text to fit a fixed console column width.
     *
     * Pads shorter strings with spaces and truncates longer strings
     * with an ellipsis to preserve table alignment.
     *
     * @param text Input text to format.
     * @param width Target column width.
     *
     * @return Formatted string with fixed width.
     */
    std::string fitText(const std::string& text, int width)
    {
        if (static_cast<int>(text.size()) <= width)
        {
            return text + std::string(width - text.size(), ' ');
        }

        return text.substr(0, width - 3) + "...";
    }
}


// /*************** CONSTRUCTOR ***************/

ConsoleRenderer::ConsoleRenderer(Logger& logger, TcpServer& tcpServer, Monitor& monitor)
    : logger(logger), tcpServer(tcpServer), monitor(monitor)
{}


// /***************** RENDER ******************/

void ConsoleRenderer::render()
{
    std::system("clear");

    renderHeader();

    std::cout << "\n";

    renderLogs();

    std::cout << "\n";

    renderArchitecture();

    std::cout << "\n";

    renderFooter();
}


// /************** RENDER HEADER **************/

void ConsoleRenderer::renderHeader() const
{
    std::cout
    << "+--------------------------------------------------------------------------------------------------------+\n"
    << "|                                         SMART SENSOR NETWORK                                           |\n"
    << "|                                  Distributed Embedded Telemetry System                                 |\n"
    << "+--------------------------------------------------------------------------------------------------------+\n";
}


// /*************** RENDER LOGS ***************/

void ConsoleRenderer::renderLogs() const
{
    auto logs = logger.getRecentLogs();
    SystemStats stats = monitor.getStats();

    std::vector<std::string> tcpPanel =
    {
        "Client: " + std::string(
            tcpServer.isClientConnected()
            ? "CONNECTED"
            : "DISCONNECTED"),

        "Port: 8080",

        "Last command: " + tcpServer.getLastCommand(),

        "",

        "=== STATISTICS ===",

        "",

        "Total sensors : " + std::to_string(stats.totalSensors),

        "Telemetry     : " + std::to_string(stats.telemetryPackets),

        "Heartbeat     : " + std::to_string(stats.heartbeatPackets),

        "Online        : " + std::to_string(stats.health.online),

        "Offline       : " + std::to_string(stats.health.offline)
    };


    std::cout
    << "+-----------------------------------------------------------------------------+--------------------------+\n"
    << "| SYSTEM LOG                                                                  | TCP MONITOR              |\n"
    << "|-----------------------------------------------------------------------------|--------------------------|\n";


    size_t maxLines = std::max(logs.size(), tcpPanel.size());

    for(size_t i = 0; i < maxLines; i++)
    {
        std::string logLine;
        std::string tcpLine;

        if(i < logs.size())
        {
            logLine = logs[i];
        }

        if(i < tcpPanel.size())
        {
            tcpLine = tcpPanel[i];
        }

        std::cout
        << "| " << fitText(logLine, SYSTEM_WIDTH)
        << " | " << fitText(tcpLine, TCP_WIDTH) << " |\n";
    }

    std::cout
    << "+-----------------------------------------------------------------------------+--------------------------+\n";
}


// /*********** RENDER ARCHITECTURE ***********/

void ConsoleRenderer::renderArchitecture() const
{
    std::cout
    << "+--------------------------------------------------------------------------------------------------------+\n"
    << "| Architecture: Sensor Nodes --> UDP --> Gateway --> TCP --> Monitor                                     |\n"
    << "+--------------------------------------------------------------------------------------------------------+\n\n"

    << "[SYSTEM] Type 'help' to view available commands\n"
    << "[SYSTEM] Press ENTER to shutdown...";
}


// /************* RENDER FOOTER **************/

void ConsoleRenderer::renderFooter() const
{
    std::cout << "\n";
}