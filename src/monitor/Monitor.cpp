#include "monitor/Monitor.hpp"
#include "common/SensorTypesString.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>


// /***************** CONSTRUCTOR *****************/

Monitor::Monitor(Gateway& gateway)
    : gateway(gateway)
{}


// /***************** SENSOR LIST *****************/

std::string Monitor::sensorList() const
{
    const auto& sensors = gateway.getSensors();

    std::vector<uint32_t> ids;

    for (const auto& [id, _] : sensors)
    {
        ids.push_back(id);
    }

    std::sort(ids.begin(), ids.end());

    std::ostringstream oss;

    oss << "=== SENSOR LIST ===\n\n";

    oss << std::left
        << std::setw(6)  << "ID"
        << std::setw(15) << "TYPE"
        << std::setw(12) << "STATE"
        << std::setw(12) << "HEALTH"
        << "LAST HB\n";

    oss << "-----------------------------------------------------\n";

    for (auto id : ids)
    {
        const auto& info = sensors.at(id);
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


// /*************** SENSOR DETAILS ****************/

std::string Monitor::sensorDetails(uint32_t id) const
{
    const auto& sensors = gateway.getSensors();

    auto it = sensors.find(id);
    if (it == sensors.end())
    {
        return "sensor not found\n";
    }

    const auto& r = it->second.lastTelemetry;
    const auto& info = it->second;

    std::ostringstream oss;

    oss << "=== SENSOR INFORMATION ===\n\n"
        << "Packet Header\n"
        << "--------------\n"
        << "messageType : " << to_string(r.header.type) << "\n"
        << "sensorId    : " << id << "\n"
        << "timestamp   : " << r.header.timestamp_ms << "\n\n"

        << "Telemetry Payload\n"
        << "-----------------\n"
        << "sensorType  : " << to_string(r.type) << "\n"
        << "state       : " << to_string(r.state) << "\n"
        << "value       : " << r.value << "\n\n"

        << "Heartbeat Status\n"
        << "----------------\n"
        << "health         : " << to_string(info.health) << "\n"
        << "last heartbeat : " << gateway.getSecondsSinceLastHeartbeat(id) << " sec ago\n\n";

    return oss.str();
}