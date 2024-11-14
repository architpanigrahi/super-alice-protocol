#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

enum class LogLevel
{
    INFO,
    DEBUG,
    ERROR
};

class Logger
{
public:
    static void log(LogLevel level, const std::string &message);

private:
    static std::string getCurrentTime();

    Logger() = default;
};

#endif
