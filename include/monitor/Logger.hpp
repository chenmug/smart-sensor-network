#pragma once
#include "monitor/ILogger.hpp"  // Forward Declaration
#include <queue>                // For std::queue
#include <string>               // For std::string
#include <mutex>                // For std::mutex
#include <condition_variable>   // For std::condition_variable
#include <atomic>               // For std::atomic
#include <thread>               // For std::thread


/**
 * @brief Thread-safe asynchronous logger.
 *
 * The Logger provides a centralized logging service that serializes output from
 * multiple threads. Instead of writing directly to std::cout, threads enqueue
 * log messages while a dedicated background thread prints them in order.
 *
 * This prevents interleaved console output and reduces contention between
 * worker threads performing I/O.
 */
class Logger : public ILogger
{
private:

    std::queue<std::string> queue;       // Queue of pending log messages.
    std::mutex mtx;                      // Protects access to the message queue.
    std::condition_variable cv;          // Notifies the worker when new messages arrive.
    std::atomic<bool> running{false};    // Indicates whether the logger is running.
    std::thread worker;                  // Background thread responsible for printing messages.

public:
    /**
     * @brief Enqueues a message for asynchronous printing.
     *
     * The message is added to the internal queue and later printed by the
     * logger thread.
     *
     * @param msg Message to be printed.
     */
    void log(const std::string& msg) override;

    /**
     * @brief Starts the logger worker thread.
     *
     * Launches the background thread responsible for consuming queued
     * log messages and writing them to the console.
     */
    void start();

    /**
     * @brief Stops the logger and waits for the worker thread to exit.
     *
     * Ensures that all queued messages are printed before shutdown.
     */
    void stop();

private:
    /**
     * @brief Main worker loop executed by the logger thread.
     *
     * Waits for incoming log messages and prints them sequentially until
     * the logger is stopped.
     */
    void run();
};