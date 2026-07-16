#pragma once
#include "network/IUdpSender.hpp"
#include <vector>   // For std::vector
#include <cstdint>  // For uint8_t

/**
 * @brief Fake UDP sender used for unit testing.
 *
 * Instead of sending real UDP packets over the network, this class stores all sent packets in 
 * memory so they can be inspected and verified in tests.
 */
class FakeUdpSender : public IUdpSender
{
private:

    std::vector<std::vector<uint8_t>> packets;  // Stores all packets that were "sent" via the fake sender for test verification
    
public:
    /**
     * @brief Sends a "fake" UDP packet.
     *
     * This function stores the packet in an internal container for later inspection.
     *
     * @param @param data Binary packet that would have been sent over UDP.
     */
    void send(const std::vector<uint8_t>& data) override
    {
        packets.push_back(data);
    }

    /**
     * @brief Retrieves all packets that were "sent".
     *
     * Used in unit tests to verify that the correct data was passed to the sender.
     *
     * @return Const reference to the internal list of sent packets.
     */
    const std::vector<std::vector<uint8_t>>& getPackets() const
    {
        return packets;
    }
};