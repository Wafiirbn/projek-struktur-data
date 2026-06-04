#include "LogHashTable.hpp"
#include <algorithm>

std::string LogHashTable::getKey(const LogEntry& e) const {
    if (key_field == "level")  return e.level;
    if (key_field == "module") return e.module;
    return e.log_id;
}

LogHashTable::LogHashTable(const std::string& kf) : key_field(kf), size_(0) {}

void LogHashTable::insert(const LogEntry& e) {
    buckets[getKey(e)].push_back(e);
    size_++;
}

std::vector<LogEntry> LogHashTable::search(const std::string& key) const {
    auto it = buckets.find(key);
    return (it != buckets.end()) ? it->second : std::vector<LogEntry>{};
}

std::vector<LogEntry> LogHashTable::searchByTimeRange(const std::string& t_start, const std::string& t_end) const {
    LogEntry tmp;
    tmp.timestamp = t_start;
    long long ts = tmp.toEpoch();
    tmp.timestamp = t_end;
    long long te = tmp.toEpoch();
    std::vector<LogEntry> res;
    for (auto& [key, chain] : buckets) {
        for (auto& e : chain) {
            long long t = e.toEpoch();
            if (t >= ts && t <= te) {
                res.push_back(e);
            }
        }
    }
    return res;
}

int LogHashTable::deleteBefore(const std::string& cutoff) {
    LogEntry tmp;
    tmp.timestamp = cutoff;
    long long cut = tmp.toEpoch();
    int deleted = 0;
    for (auto& [key, chain] : buckets) {
        int before = chain.size();
        chain.erase(std::remove_if(chain.begin(), chain.end(),
            [&](const LogEntry& e){ return e.toEpoch() < cut; }),
            chain.end());
        deleted += before - (int)chain.size();
    }
    size_ -= deleted;
    return deleted;
}

std::unordered_map<std::string, int> LogHashTable::statistics() const {
    std::unordered_map<std::string, int> stats;
    for (auto& [key, chain] : buckets) {
        stats[key] = (int)chain.size();
    }
    return stats;
}

int LogHashTable::size() const {
    return size_;
}
