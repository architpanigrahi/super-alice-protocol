#include "alice/logger.hpp"

std::string Logger::getCurrentTime()
{
    auto now = std::time(nullptr);
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Logger::log(LogLevel level, const std::string &message)
{
    static std::ofstream logFile("peer_log.txt", std::ios::app);

    std::string levelStr;
    switch (level)
    {
    case LogLevel::INFO:
        levelStr = "[INFO] ";
        break;
    case LogLevel::DEBUG:
        levelStr = "[DEBUG] ";
        break;
    case LogLevel::ERROR:
        levelStr = "[ERROR] ";
        break;
    }

    logFile << Logger::getCurrentTime() << " " << levelStr << message << std::endl;
}
