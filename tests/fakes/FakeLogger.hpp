#pragma once
#include "monitor/ILogger.hpp"
#include <string>  // For std::string


/**
 * @brief Lightweight fake logger used for unit testing.
 *
 * This class allowing production components that depend on a Logger to be tested
 * without producing console output or requiring a background logging thread.
 *
 * It is used in unit tests to:
 * - Avoid side effects from std::cout
 * - Eliminate concurrency dependencies (no worker thread)
 * - Keep tests deterministic and fast
 *
 * The interface matches Logger::log() but intentionally ignores all input.
 */
class FakeLogger : public ILogger
{
public:
    /**
     * @brief Mock logging function used in unit tests.
     *
     * This function intentionally performs no operation and exists only to
     * satisfy the Logger interface in testing environments.
     *
     * @param msg Log message (ignored).
     */
    void log(const std::string& msg) override {}
};