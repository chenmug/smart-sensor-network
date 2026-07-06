#pragma once
#include "gateway/Gateway.hpp"
#include <cstdint>  // For uint16_t
#include <atomic>   // for std::atomic

/**
 * @brief UDP receiver responsible for receiving telemetry packets and forwarding them to the Gateway.
 *
 * The UdpReceiver listens on a specific UDP port, receives raw binary packets,
 * and dispatches them to the Gateway for decoding and processing.
 *
 * Responsibilities:
 * - Binding to a UDP port
 * - Receiving packets using recvfrom()
 * - Forwarding received data to Gateway::handlePacket()
 */
class UdpReceiver
{
private:
    static constexpr std::size_t MAX_PACKET_SIZE = 1024;  // The maximum packet size
    Gateway& gateway;                                     // Reference to the central system that processes telemetry data
    uint16_t port;                                        // UDP port on which the receiver listens
    int sockfd = -1;                                      // Socket file descriptor (-1 means not initialized)
    std::atomic<bool> running;                            // Controls main loop execution
    ILogger& logger;                                      // Reference to the shared system logger used for thread-safe system logging.

public:
    /**
     * @brief Constructs a UDP receiver bound to a specific port.
     *
     * @param gateway Reference to the Gateway that processes incoming packets
     * @param logger Reference to the shared system Logger.
     * @param port UDP port to listen on
     */
    UdpReceiver(Gateway& gateway, ILogger& logger, uint16_t port);

    /**
     * @brief Receives a single UDP packet and forwards it to the Gateway.
     *
     * Blocks until a packet arrives or an error occurs.
     */
    void receivePacket();

    /**
     * @brief Starts listening for incoming UDP packets.
     *
     * This function runs a receive loop, blocking on recvfrom(),
     * and forwards each received packet to the Gateway.
     */
    void run();

    /**
     * @brief Stops the receiver loop safely.
     */
    void stop();

    /**
     * @brief Destroys the UDP receiver and closes the socket.
     */
    ~UdpReceiver();
};