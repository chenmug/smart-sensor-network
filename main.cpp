#include "gateway/Gateway.hpp"
#include "network/UdpReceiver.hpp"
#include "network/TcpServer.hpp"
#include "sensor/MotionSensor.hpp"
#include "sensor/TemperatureSensor.hpp"
#include "sensor/BatterySensor.hpp"
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

    TemperatureSensor sensor3(3);
    TemperatureSensor sensor4(4);

    BatterySensor sensor5(5);

    UdpSender sender1("127.0.0.1", 9000);
    UdpSender sender2("127.0.0.1", 9000);
    UdpSender sender3("127.0.0.1", 9000);
    UdpSender sender4("127.0.0.1", 9000);
    UdpSender sender5("127.0.0.1", 9000);

    SensorNode node1(sensor1, sender1, logger);
    SensorNode node2(sensor2, sender2, logger);
    SensorNode node3(sensor3, sender3, logger);
    SensorNode node4(sensor4, sender4, logger);
    SensorNode node5(sensor5, sender5, logger);

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

    logger.log("[SYSTEM] Sensors created and initialized");

    node1.tick();
    node2.tick();
    node3.tick();
    node4.tick();
    node5.tick();

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