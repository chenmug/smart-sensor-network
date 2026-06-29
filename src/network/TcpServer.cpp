#include "network/TcpServer.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <thread>


// /*************** CONSTRUCTOR ***************/

TcpServer::TcpServer(Gateway& gateway, uint16_t port)
    : gateway(gateway), running(false)
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
        std::cerr << "[TCP] Server not initialized\n";
        return;
    }

    running = true;

    std::cout << "[TCP] Server started\n";

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

        std::cout << "[TCP] Client connected\n";

        handleClient(clientSock);
        close(clientSock);

         std::cout << "[TCP] Client disconnected\n";
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

    std::cout << "[TCP] Command: " << request << std::endl;

    if (req == "list")
    {
        const auto& sensors = gateway.getSensors();

        std::string response;

        for (const auto& [id, _] : sensors)
        {
            response += std::to_string(id) + " ";
        }

        response += "\n";
        return response;
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
                return "sensor not found\n";
            }

            const auto& r = it->second.lastReading;

            std::ostringstream oss;
            oss << "id=" << r.sensorId
                << " value=" << r.value
                << " state=" << static_cast<int>(r.state)
                << " timestamp=" << r.timestamp_ms
                << "\n";

            return oss.str();
        }

        catch (const std::exception&)
        {
            return "invalid sensor id\n";
        }
    }

    return "unknown command\n";
}