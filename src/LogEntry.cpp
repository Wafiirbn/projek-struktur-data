#include "LogEntry.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

long long LogEntry::toEpoch() const {
    if (_epoch >= 0) return _epoch;
    struct tm t = {};
    std::istringstream ss(timestamp);
    ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
    t.tm_isdst = -1;
    _epoch = (long long)mktime(&t);
    return _epoch;
}

void LogEntry::print() const {
    std::cout << "[" << timestamp << "] "
              << std::setw(8) << std::left << level << " | "
              << std::setw(12) << std::left << module << " | "
              << log_id << " | "
              << message << "\n";
}
