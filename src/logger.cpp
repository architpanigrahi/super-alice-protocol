// Created By Rokas Paulauskas on 14/11/2024
#include "alice/logger.hpp"
std::string Logger::log_file_path_ = "alice.log";
std::string Logger::getCurrentTime()
{
    auto now = std::time(nullptr);
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
void Logger::setLogFile(const std::string &log_file_path)
{
    log_file_path_ = log_file_path;
}
void Logger::log(LogLevel level, const std::string &message)
{
    static std::ofstream logFile(log_file_path_, std::ios::app);

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
    std::cout << Logger::getCurrentTime() << " " << levelStr << message << std::endl;
}
