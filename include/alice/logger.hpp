// Not thread safe!! Created By Rokas Paulauskas on 14/11/2024
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
    static void setLogFile(const std::string &log_file_path);

private:
    static std::string getCurrentTime();
    static std::string log_file_path_;
    Logger() = default;
};

#endif
