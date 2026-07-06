#include "network/UdpReceiver.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>


// /*************** CONSTRUCTOR ***************/

UdpReceiver::UdpReceiver(Gateway& gateway, ILogger& logger, uint16_t port)
    : gateway(gateway), logger(logger), port(port), running(false)
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        throw std::runtime_error("Failed to create UDP socket");
    }

    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind failed");
        close(sockfd);
        sockfd = -1;
    }
}


// /************* RECEIVE PACKET **************/

void UdpReceiver::receivePacket()
{
    std::vector<uint8_t> buffer(MAX_PACKET_SIZE);

    sockaddr_in senderAddr{};
    socklen_t senderLen = sizeof(senderAddr);

    ssize_t bytesReceived = recvfrom(
        sockfd,
        buffer.data(),
        buffer.size(),
        0,
        reinterpret_cast<sockaddr*>(&senderAddr),
        &senderLen
    );

    if (bytesReceived < 0)
    {
        if (!running)
        {
            return; 
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return; 
        }

        perror("recvfrom failed");
        return;
    }

    buffer.resize(bytesReceived);

    gateway.handlePacket(buffer);
}


// /**************** RUN LOOP ****************/

void UdpReceiver::run()
{
    if (sockfd < 0)
    {
        logger.log("Invalid socket. Receiver not started.");
        return;
    }

    running = true;

    logger.log("[UDP] Receiver started");

    while (running) 
    {
        receivePacket();
    }
}


// /******************* STOP *******************/

void UdpReceiver::stop()
{
    running = false;
}


// /**************** DESTRUCTOR ****************/

UdpReceiver::~UdpReceiver()
{
    stop();

    if (sockfd >= 0)
    {
        close(sockfd);
        sockfd = -1;
    }
}