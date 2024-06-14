#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

class Logger
{
public:
    static void log(const std::string& tag, const std::string& message)
    {
        if (!LOGGING_ENABLED)
        {
            return;
        }

        // Get the current time
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        // Format the time
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");

        // Output the log message
        std::cout << "[" << ss.str() << "] [" << tag << "] " << message << std::endl;
    }

    static bool LOGGING_ENABLED;
};

bool Logger::LOGGING_ENABLED = true;
