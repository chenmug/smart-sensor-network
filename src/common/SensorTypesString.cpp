#include "common/SensorTypesString.hpp"  //Forward Declaration


// /*************** STRING TYPE ***************/

std::string to_string(SensorType type)
{
    switch(type)
    {
        case SensorType::Motion:
            return "Motion";
            
        case SensorType::Temperature:
            return "Temperature";

        case SensorType::Battery:
            return "Battery";
        
        case SensorType::Pressure:
            return "Pressure";

        default:
            return "Unknown";
    }
}


// /************** STRING STATE ***************/

std::string to_string(SensorState state)
{
    switch(state)
    {
        case SensorState::ACTIVE: 
            return "ACTIVE";

        case SensorState::WARNING: 
            return "WARNING";

        case SensorState::ERROR: 
            return "ERROR";

        default: 
            return "UNKNOWN";
    }
}


// /*********** STRING MASSEGE TYPE ***********/

std::string to_string(MessageType type)
{
    return (type == MessageType::TELEMETRY) ? "TELEMETRY" : "HEARTBEAT";
}