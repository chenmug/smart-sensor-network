#pragma once
#include <chrono>


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