#include "network/TcpServer.hpp"
#include "common/SensorTypesString.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <thread>


// /*************** CONSTRUCTOR ***************/

TcpServer::TcpServer(Gateway& gateway,ILogger& logger, uint16_t port)
    : gateway(gateway), monitor(gateway), logger(logger), running(false)
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

    if (req == "help")
    {
        return monitor.help();
    }

    if (req == "list")
    {
        return monitor.sensorList();
    }

    if (req == "health")
    {
        return monitor.healthSummary();
    }

    if (req.rfind("get ", 0) == 0)
    {
        try
        {
            uint32_t id = parseId(req);
            if (id >= gateway.getSensors().size())
            {
                logger.log("[TCP] Sensor " + std::to_string(id) + " not found");
            }

            return monitor.sensorDetails(id);
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


// /**************** PARSE ID ******************/

uint32_t TcpServer::parseId(const std::string& request) const
{
    constexpr std::size_t prefixLength = 4; 

    int id = std::stoi(request.substr(prefixLength));

    if (id <= 0)
    {
        throw std::invalid_argument("Invalid sensor id");
    }

    return static_cast<uint32_t>(id);
}