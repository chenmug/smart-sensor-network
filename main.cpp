#include "gateway/Gateway.hpp"
#include "network/UdpReceiver.hpp"
#include "network/TcpServer.hpp"
#include "sensor/MotionSensor.h"
#include "sensor/SensorNode.hpp"
#include "network/UdpSender.hpp"
#include "monitor/Logger.hpp"
#include <iostream>
#include <thread>
#include <atomic>


int main()
{
    std::cout << "=== SMART SENSOR NETWORK DEMO START ===\n\n";

    Logger logger;
    logger.start();

    logger.log("[SYSTEM] Logger started");

    Gateway gateway(logger);
    UdpReceiver receiver(gateway, logger, 9000);
    TcpServer tcpServer(gateway, logger, 8080);

    MotionSensor sensor1(1);
    MotionSensor sensor2(2);

    UdpSender sender1("127.0.0.1", 9000);
    UdpSender sender2("127.0.0.1", 9000);

    SensorNode node1(sensor1, sender1, logger);
    SensorNode node2(sensor2, sender2, logger);

    std::atomic<bool> udpReady{false};
    std::atomic<bool> tcpReady{false};

    std::thread udpThread([&]()
    {
        logger.log("[SYSTEM] Starting UDP service");
        udpReady = true;
        receiver.run();
    });

    std::thread tcpThread([&]()
    {
        logger.log("[SYSTEM] Starting TCP service");
        tcpReady = true;
        tcpServer.run();
    });

    // Wait until ready
    while (!udpReady || !tcpReady)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    logger.log("[SYSTEM] All services ready\n");

    logger.log("[SYSTEM] Sensors initialized");

    node1.tick();
    node2.tick();

    logger.log("[SYSTEM] Waiting for TCP client...");

    // Wait 
    logger.log("[SYSTEM] Press ENTER to shutdown...\n");
    std::cin.get();

    // Shutdown
    logger.log("[SYSTEM] Shutting down...");

    receiver.stop();
    tcpServer.stop();

    udpThread.join();
    tcpThread.join();

    logger.log("[SYSTEM] Shutdown complete");

    logger.stop();

    std::cout << "\n=== SYSTEM SHUTDOWN COMPLETE ===\n";

    return 0;
}