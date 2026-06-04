#ifndef LOG_HASH_TABLE_HPP
#define LOG_HASH_TABLE_HPP

#include "LogEntry.hpp"
#include <vector>
#include <string>
#include <unordered_map>

class LogHashTable {
    std::string key_field;   // "level" atau "module"
    std::unordered_map<std::string, std::vector<LogEntry>> buckets;
    int size_;

    std::string getKey(const LogEntry& e) const;

public:
    explicit LogHashTable(const std::string& kf);

    // INSERT — O(1) amortized
    void insert(const LogEntry& e);

    // SEARCH by key — O(1) + O(k)
    std::vector<LogEntry> search(const std::string& key) const;

    // SEARCH by time range untuk keperluan komparasi adil di benchmark — O(n)
    std::vector<LogEntry> searchByTimeRange(const std::string& t_start, const std::string& t_end) const;

    // DELETE log lama — O(n)
    int deleteBefore(const std::string& cutoff);

    // STATISTIK — O(1) per level/modul
    std::unordered_map<std::string, int> statistics() const;

    int size() const;
};

#endif // LOG_HASH_TABLE_HPP
