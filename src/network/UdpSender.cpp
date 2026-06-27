#include "network/UdpSender.hpp"  // Forward Declaration
#include <sys/socket.h>           // For socket
#include <stdexcept>              // For std::runtime_error
#include <unistd.h>               // For close
#include <netinet/in.h>           // For sockaddr_in
#include <arpa/inet.h>            // For inet_pton


// /**************** CONSTRUCTOR ****************/

UdpSender::UdpSender(const std::string& ip, uint16_t port)
    : gatewayIp(ip), gatewayPort(port)
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sockfd < 0)
    {
        throw std::runtime_error("Failed to create UDP socket");
    }
}


// /*************** DESTRUCTOR ****************/

UdpSender::~UdpSender()
{
    if (sockfd >= 0)
    {
        close(sockfd);
    }
}


// /****************** SEND *******************/

void UdpSender::send(const std::vector<uint8_t>& data)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(gatewayPort);

    if (inet_pton(AF_INET, gatewayIp.c_str(), &addr.sin_addr) <= 0)
    {
        throw std::runtime_error("Invalid IP address");
    }

    ssize_t sent = sendto(
        sockfd,
        data.data(),
        data.size(),
        0,
        (sockaddr*)&addr,
        sizeof(addr)
    );

    if (sent < 0)
    {
        throw std::runtime_error("UDP send failed");
    }
}
