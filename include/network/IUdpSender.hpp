#pragma once
#include <vector>   // For std::vector 


/**
 * @brief Abstract interface for UDP packet transmission.
 *
 * Defines a contract for sending serialized binary data over UDP.
 * Decouples networking implementation from application logic and enables substitution with 
 * mock or fake implementations for testing.
 */
class IUdpSender
{
public:
    /**
     * @brief Virtual destructor.
     *
     * Ensures proper cleanup of derived classes.
     */
    virtual ~IUdpSender() = default;

    /**
     * @brief Sends a binary packet over UDP.
     *
     * @param data Serialized telemetry packet.
     */
    virtual void send(const std::vector<uint8_t>& data) = 0;
};