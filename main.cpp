#include "sensor/MotionSensor.h"
#include "sensor/SensorNode.hpp"
#include "network/UdpSender.hpp"
#include "network/UdpReceiver.hpp"
#include "gateway/Gateway.hpp"
#include <thread>


int main()
{
    Gateway gateway;
    UdpReceiver receiver(gateway, 9000);

    MotionSensor sensor(1);
    UdpSender sender("127.0.0.1", 9000);
    SensorNode node(sensor, sender);

    std::thread receiverThread([&]()
    {
        receiver.run();
    });

    std::thread sensorThread([&]()
    {
        for (int i = 0; i < 20; ++i)
        {
            node.tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    sensorThread.join();
    receiver.stop();
    receiverThread.join();

    return 0;
}