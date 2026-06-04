#ifndef LOG_ENTRY_HPP
#define LOG_ENTRY_HPP

#include <string>

struct LogEntry {
    std::string log_id;
    std::string timestamp;
    std::string level;
    std::string module;
    std::string message;
    mutable long long _epoch = -1;

    // Konversi timestamp string ke Unix epoch (long long), di-cache
    long long toEpoch() const;
    void print() const;
};

#endif // LOG_ENTRY_HPP
