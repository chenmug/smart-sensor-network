#include "gateway/Gateway.hpp"
#include "network/UdpReceiver.hpp"
#include "network/TcpServer.hpp"
#include "sensor/MotionSensor.hpp"
#include "sensor/TemperatureSensor.hpp"
#include "sensor/BatterySensor.hpp"
#include "sensor/PressureSensor.hpp"
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


    // ----------- Gateway & Services -----------

    Gateway gateway(logger);
    UdpReceiver receiver(gateway, logger, 9000);
    TcpServer tcpServer(gateway, logger, 8080);


    // ---------------- Sensors -----------------

    MotionSensor sensor1(1);
    MotionSensor sensor2(2);

    TemperatureSensor sensor3(3);  // Sensor with heartbeat failure simulation
    TemperatureSensor sensor4(4);

    BatterySensor sensor5(5);

    PressureSensor sensor6(6);


    // --------------- UDP Senders --------------

    UdpSender sender1("127.0.0.1", 9000);
    UdpSender sender2("127.0.0.1", 9000);
    UdpSender sender3("127.0.0.1", 9000);
    UdpSender sender4("127.0.0.1", 9000);
    UdpSender sender5("127.0.0.1", 9000);
    UdpSender sender6("127.0.0.1", 9000);


    // -------------- Sensor Nodes --------------

    SensorNode node1(sensor1, sender1, logger);
    SensorNode node2(sensor2, sender2, logger);
    SensorNode node3(sensor3, sender3, logger);
    SensorNode node4(sensor4, sender4, logger);
    SensorNode node5(sensor5, sender5, logger);
    SensorNode node6(sensor6, sender6, logger);

    std::atomic<bool> udpReady{false};
    std::atomic<bool> tcpReady{false};
    std::atomic<bool> watchdogRunning{true};
    std::atomic<bool> heartbeatSimulationRunning{true};


    // --------- UDP Receiver Thread -------------

    std::thread udpThread([&]()
    {
        logger.log("[SYSTEM] Starting UDP service");
        udpReady = true;
        receiver.run();
    });


    // ------------ TCP Server Thread -------------

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

    
    // ------------- Watchdog Thread ---------------

    std::thread watchdogThread([&]()
    {
        logger.log("[SYSTEM] Watchdog started");

        while (watchdogRunning)
        {
            gateway.detectOfflineSensors();

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });


    // ------------- Initial Telemetry --------------

    logger.log("[SYSTEM] Sensors initialized");
    logger.log("[SYSTEM] Sending initial telemetry packets");

    node1.tick();
    node2.tick();
    node3.tick();
    node4.tick();
    node5.tick();
    node6.tick();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    logger.log("\n");


    // ------------ Heartbeat Simulation --------------

    logger.log("[SYSTEM] Sending initial heartbeats");

    // All sensors send heartbeat one time
    node1.sendHeartbeat();
    node2.sendHeartbeat();
    node3.sendHeartbeat();   
    node4.sendHeartbeat();
    node5.sendHeartbeat();
    node6.sendHeartbeat();

    std::thread heartbeatThread([&]()
    {
        logger.log("[SYSTEM] Simulating heartbeat failure for sensor 3");

        while (heartbeatSimulationRunning)
        {
            node1.sendHeartbeat();
            node2.sendHeartbeat();

            // node3 intentionally does not send heartbeat

            node4.sendHeartbeat();
            node5.sendHeartbeat();
            node6.sendHeartbeat();

            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    });


    // -------------- Watchdog Detection ----------------

    logger.log("\n");
    logger.log("[SYSTEM] Waiting for watchdog detection");

    std::this_thread::sleep_for(std::chrono::milliseconds(SENSOR_HEARTBEAT_TIMEOUT_MS + 2000));

    logger.log("[SYSTEM] Watchdog check completed\n");

    logger.log("[SYSTEM] Waiting for TCP client...");
    logger.log("[SYSTEM] Press ENTER to shutdown...\n");

    std::this_thread::sleep_for(std::chrono::milliseconds(SENSOR_HEARTBEAT_TIMEOUT_MS + 4000));


    // ----------- Simulate Sensor Recovery -----------

    logger.log("[SYSTEM] Recovering sensor 3");

    node3.sendHeartbeat();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    logger.log("[SYSTEM] Sensor 3 recovery completed\n");


    std::cin.get();


    // ------------------- Shutdown ---------------------

    logger.log("[SYSTEM] Shutting down...");

    watchdogRunning = false;
    heartbeatSimulationRunning = false;

    receiver.stop();
    tcpServer.stop();

    heartbeatThread.join();
    watchdogThread.join();
    udpThread.join();
    tcpThread.join();

    logger.log("[SYSTEM] Shutdown complete");

    logger.stop();

    std::cout << "\n=== SYSTEM SHUTDOWN COMPLETE ===\n";

    return 0;
}