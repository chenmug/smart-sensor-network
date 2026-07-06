#pragma once
#include <string>  // For std::string


/**
 * @brief Abstract interface for system logging.
 *
 * Provides a unified logging contract used across the system to decouple
 * components from a concrete logging implementation.
 *
 * This allows:
 * - Swapping implementations (console logger, test fake logger)
 * - Unit testing without side effects
 * - Centralized control over logging behavior
 *
 * The interface is intentionally minimal and thread-agnostic; concurrency
 * handling is the responsibility of the concrete implementation.
 */
class ILogger
{
public:
    /**
     * @brief Virtual destructor.
     *
     * Ensures proper cleanup of derived classes.
     */
    virtual ~ILogger() = default;

    /**
     * @brief Writes a log message to the configured output.
     *
     * @param msg The message to be logged.
     */
    virtual void log(const std::string& msg) = 0;
};