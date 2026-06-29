#include "sensor/MotionSensor.h"
#include "sensor/SensorNode.hpp"
#include "network/UdpSender.hpp"
#include "network/UdpReceiver.hpp"
#include "network/TcpServer.hpp"
#include "gateway/Gateway.hpp"
#include <iostream>
#include <thread>


int main()
{
    std::cout << "\n=== SMART SENSOR NETWORK DEMO START ===\n\n";

    Gateway gateway;
    UdpReceiver receiver(gateway, 9000);
    TcpServer tcpServer(gateway, 8080);

    MotionSensor sensor1(1);
    MotionSensor sensor2(2);

    UdpSender sender1("127.0.0.1", 9000);
    UdpSender sender2("127.0.0.1", 9000);

    SensorNode node1(sensor1, sender1);
    SensorNode node2(sensor2, sender2);

    node1.tick();
    node2.tick();

    std::cout << "\n[SYSTEM] Sensors initialized.\n";
    std::cout << "[SYSTEM] Waiting for TCP client...\n";

    std::thread udpThread([&]()
    {
        std::cout << "[SYSTEM] UDP running\n";
        receiver.run();
    });

    std::thread tcpThread([&]()
    {
        std::cout << "[SYSTEM] TCP running\n";
        tcpServer.run();
    });


    std::cout << "\nPress ENTER to shutdown...\n";
    std::cin.get();

    std::cout << "\n[SYSTEM] Shutting down...\n";

    receiver.stop();
    tcpServer.stop();

    // join all threads 
    udpThread.join();
    tcpThread.join();

    std::cout << "\n=== SYSTEM SHUTDOWN COMPLETE ===\n";

    return 0;
}