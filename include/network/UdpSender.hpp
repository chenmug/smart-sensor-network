#pragma once
#include "network/IUdpSender.hpp"  // Forward Declaration
#include <string>                  // For std::string
#include <vector>                  // For std::vector
#include <cstdint>                 // For uint16_t  


/**
 * @brief Sends binary telemetry data to a remote UDP endpoint.
 *
 * UdpSender is responsible for transmitting serialized telemetry packets over the network.
 * It manages the lifetime of a UDP socket and sends raw byte buffers to a configured
 * gateway IP address and port.
 */
class UdpSender : public IUdpSender
{
private:
    std::string gatewayIp;  // Destination IPv4 address of the gateway
    uint16_t gatewayPort;   // Destination UDP port
    int sockfd = -1;        // UDP socket file descriptor (-1 means invalid/uninitialized)

public:
    /**
     * @brief Constructs a UDP sender for a specific destination.
     *
     * Creates a UDP socket and stores the destination address used for
     * subsequent transmissions.
     *
     * @param ip Destination IPv4 address.
     * @param port Destination UDP port.
     */
    UdpSender(const std::string& ip, uint16_t port);

    /**
     * @brief Closes the UDP socket.
     *
     * Releases the underlying operating system socket resource.
     */
    ~UdpSender() override;

    /**
     * @brief Sends a serialized telemetry packet over UDP.
     *
     * @param data Serialized telemetry data to transmit.
     */
    void send(const std::vector<uint8_t>& data) override;
};