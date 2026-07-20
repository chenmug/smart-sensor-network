#include "monitor/Logger.hpp"
#include "common/TimeUtils.hpp"
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

        history.push_back("[" + currentTimeString() + "] " + msg);

        if (history.size() > MAX_HISTORY)
        {
            history.erase(history.begin());
        }
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
            queue.pop();
        }
    }
}


// /************ GET RECENT LOGS ************/

std::vector<std::string> Logger::getRecentLogs() const
{
    std::lock_guard<std::mutex> lock(mtx);

    return history;
}