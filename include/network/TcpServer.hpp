#pragma once
#include "gateway/Gateway.hpp"  // Forward Declaration
#include <atomic>               // For std::atomic
#include <string>               // For std::string


/**
 * @brief TCP server providing a read-only monitoring interface for the sensor network.
 *
 * The TcpServer allows external clients to query the current state of all sensors
 * stored inside the Gateway via simple text-based commands.
 *
 * This component is part of the system's observability layer and is used for:
 * - Debugging
 * - System inspection
 * - Real-time monitoring of sensor state
 *
 * Supported commands:
 * - "list"     -> returns all registered sensor IDs.
 * - "get <id>" -> returns the latest reading of a specific sensor.
 */
class TcpServer
{
private:
    
    static constexpr std::size_t MAX_PACKET_SIZE = 1024;  // Maximum supported TCP request size (bytes).
    Gateway& gateway;                                     // Reference to the central Gateway storing the latest sensor state.
    int serverSocket = -1;                                // File descriptor of the listening TCP socket (-1 indicates an invalid socket).
    std::atomic<bool> running;                            // Controls whether the server's main loop continues accepting clients.

public:

    /**
     * @brief Constructs a TCP server bound to the specified port.
     *
     * Creates the listening socket, binds it to the requested port,
     * and prepares it to accept incoming client connections.
     *
     * @param gateway Reference to the central Gateway storing sensor state.
     * @param port TCP port on which the server will listen.
     */
    TcpServer(Gateway& gateway, uint16_t port);

    /**
     * @brief Processes a single client command and generates a response.
     *
     * Parses the incoming text command, queries the Gateway when necessary,
     * and returns a formatted response that will be sent back to the client.
     *
     * Supported commands:
     * - "list"     -> returns all registered sensor IDs.
     * - "get <id>" -> returns the latest reading for the specified sensor.
     *
     * Invalid or unsupported commands return an appropriate error message.
     *
     * @param request Raw command received from the TCP client.
     * @return Formatted response string to send back to the client.
     */
    std::string processRequest(const std::string& request);

    /**
     * @brief Starts the TCP server main loop.
     *
     * Continuously waits for incoming client connections and handles
     * one client at a time (single-client model).
     *
     * The server continues accepting connections until stop() is called.
     */
    void run();

    /**
     * @brief Requests the server to stop accepting new connections.
     *
     * Signals the main server loop to terminate gracefully.
     */
    void stop();

    /**
     * @brief Stops the server and releases socket resources.
     */
    ~TcpServer();

private:

    /**
     * @brief Handles a single connected TCP client session.
     *
     * Responsibilities:
     * - Receives commands from the connected client.
     * - Parses supported commands.
     * - Queries the Gateway for sensor information.
     * - Sends formatted responses back to the client.
     *
     * The client session remains active until the client disconnects or a communication error occurs.
     *
     * @param clientSock Connected client socket file descriptor.
     */
    void handleClient(int clientSock);
};