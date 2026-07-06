#include "monitor/Logger.hpp"
#include "common/TimeUtils.hpp"
#include <chrono>
#include <iostream>


// /***************** START *****************/

void Logger::start()
{
    running = true;
    worker = std::thread(&Logger::run, this);
}


// /***************** STOP ******************/

void Logger::stop()
{
    running = false;
    cv.notify_all();

    if (worker.joinable())
    {
        worker.join();
    }
}


// /****************** LOG ******************/

void Logger::log(const std::string& msg)
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(msg);
    }

    cv.notify_one();
}


// /****************** RUN ******************/

void Logger::run()
{
    while (running)
    {
        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, [&] {
            return !queue.empty() || !running;
        });

        while (!queue.empty())
        {
            std::string msg = queue.front();
            queue.pop();

            lock.unlock();

            std::cout << "[" << currentTimeString() << "] " << msg << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1200));

            lock.lock();
        }
    }

    // flush remaining
    while (!queue.empty())
    {
        std::cout << queue.front() << std::endl;
        queue.pop();
    }
}