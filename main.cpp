#include "sensor/MotionSensor.h"
#include "sensor/SensorNode.hpp"
#include "network/UdpSender.hpp"
#include <thread>

int main()
{
    MotionSensor sensor(1);
    UdpSender sender("127.0.0.1", 9000);
    SensorNode node(sensor, sender);

    for (int i = 0; i < 5; ++i)
    {
        node.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}