#pragma once
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>


/**
 * @brief Returns the current system timestamp in milliseconds.
 *
 * This utility function provides a monotonic time reference (in milliseconds)
 * based on the system clock. It is used for timestamping sensor readings, heartbeat tracking,
 * and telemetry packet generation.
 *
 * @return uint64_t Current time in milliseconds since epoch.
 */
inline uint64_t now()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

/**
 * @brief Returns the current local time formatted as a human-readable string.
 *
 * Generates a timestamp in the format "HH:MM:SS.mmm", where the fractional
 * part represents milliseconds. This function is intended for logging and
 * debugging purposes to improve the readability and ordering of system events.
 *
 * @return std::string The current local time formatted as "HH:MM:SS.mmm".
 */
inline std::string currentTimeString()
{
    using namespace std::chrono;

    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::time_t t = system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;

    oss << std::put_time(&tm, "%H:%M:%S")
        << '.'
        << std::setw(3)
        << std::setfill('0')
        << ms.count();

    return oss.str();
}