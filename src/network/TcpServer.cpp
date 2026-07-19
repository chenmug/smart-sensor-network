#include "network/TcpServer.hpp"
#include "common/SensorTypesString.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>


// /*************** CONSTRUCTOR ***************/

TcpServer::TcpServer(Gateway& gateway,ILogger& logger, uint16_t port)
    : gateway(gateway), logger(logger), running(false)
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0)
    {
        throw std::runtime_error("Failed to create TCP socket");
    }

    fcntl(serverSocket, F_SETFL, O_NONBLOCK);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind failed");
        close(serverSocket);
        serverSocket = -1;
        return;
    }

    if (listen(serverSocket, 1) < 0)
    {
        perror("listen failed");
        close(serverSocket);
        serverSocket = -1;
        return;
    }
}


// /**************** RUN LOOP ****************/

void TcpServer::run()
{
    if (serverSocket < 0)
    {
        logger.log("[TCP] Server not initialized");
        return;
    }

    running = true;

    logger.log("[TCP] Server started");

    while (running)
    {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);

        int clientSock = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

        if (clientSock < 0)
        {
            if (!running)
            {
                 break;
            }
               
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            perror("accept failed");
            continue;
        }

        logger.log("[TCP] Client connected");

        handleClient(clientSock);
        close(clientSock);

        logger.log("[TCP] Client disconnected");
    }
}


// /******************* STOP *******************/

void TcpServer::stop()
{
    running = false;
}


// /**************** DESTRUCTOR ****************/

TcpServer::~TcpServer()
{
    stop();

    if (serverSocket >= 0)
    {
        close(serverSocket);
        serverSocket = -1;
    }
}


// /************** HANDLE CLIENT ***************/

void TcpServer::handleClient(int clientSock)
{
    char buffer[MAX_PACKET_SIZE];

    while (true)
    {
        ssize_t bytes = read(clientSock, buffer, sizeof(buffer));

        // client disconnected or error
        if (bytes <= 0)
        {
            break;
        }

        // construct request from exact bytes (no null-termination dependency)
        std::string request(buffer, bytes);

        std::string response = processRequest(request);

        size_t total = 0;

        while (total < response.size())
        {
            ssize_t sent = write(
                clientSock,
                response.data() + total,
                response.size() - total);

            if (sent <= 0) // error or disconnect
            {
                logger.log("[TCP] Failed to send response");
                return;
            }

            total += static_cast<size_t>(sent);
        }
    }
}


// /************* PROCESS REQUEST **************/

std::string TcpServer::processRequest(const std::string& request)
{
    std::string req = request;

    // normalize
    while (!req.empty() && (req.back() == '\n' || req.back() == '\r'))
    {
        req.pop_back();
    }

    logger.log("[TCP] Command: " + req);

    if (req == "list")
    {
        const auto& sensors = gateway.getSensors();

        std::ostringstream oss;

        oss << "=== SENSOR LIST ===\n\n";

        oss << std::left
            << std::setw(6)  << "ID"
            << std::setw(15) << "TYPE"
            << std::setw(12) << "STATE"
            << std::setw(12) << "HEALTH"
            << "LAST HB\n";

        oss << "-----------------------------------------------------\n";

        for (const auto& [id, info] : sensors)
        {
            const auto& telemetry = info.lastTelemetry;

            oss << std::left
                << std::setw(6)  << id
                << std::setw(15) << to_string(telemetry.type)
                << std::setw(12) << to_string(telemetry.state)
                << std::setw(12) << to_string(info.health)
                << gateway.getSecondsSinceLastHeartbeat(id)
                << " sec\n";
        }

        oss << "\n\n";

        return oss.str();
    }

    if (req.rfind("get ", 0) == 0)
    {
        const auto& sensors = gateway.getSensors();

        try
        {
            uint32_t id = static_cast<uint32_t>(std::stoi(req.substr(4)));

            auto it = sensors.find(id);

            if (it == sensors.end())
            {
                logger.log("[TCP] Sensor " + std::to_string(id) + " not found");
                return "sensor not found\n";
            }

            const auto& r = it->second.lastTelemetry;
            const auto& info = it->second;

            std::ostringstream oss;
            oss << "=== SENSOR INFORMATION ===" << "\n\n"
                << "Packet Header" << "\n"
                << "--------------" << "\n"
                << "messageType : " << to_string(r.header.type) << "\n"
                << "sensorId    : " << r.header.sensorId << "\n"
                << "timestamp   : " << r.header.timestamp_ms << "\n\n"

                << "Telemetry Payload" << "\n"
                << "-----------------" << "\n"
                << "sensorType  : " << to_string(r.type) << "\n"
                << "state       : " << to_string(r.state) << "\n"
                << "value       : " << r.value << "\n\n"

                << "Heartbeat Status" << "\n"
                << "----------------" << "\n"
                << "health         : " << to_string(info.health) << "\n"
                << "last heartbeat : " << gateway.getSecondsSinceLastHeartbeat(r.header.sensorId) << " sec ago" << "\n\n\n";

            return oss.str();
        }

        catch (const std::exception&)
        {
            logger.log("[TCP] Invalid sensor id");
            return "invalid sensor id\n";
        }
    }

    logger.log("[TCP] Unknown command: " + req);
    return "unknown command\n";
}