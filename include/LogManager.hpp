#ifndef LOG_MANAGER_HPP
#define LOG_MANAGER_HPP

#include "LogEntry.hpp"
#include "LogLinkedList.hpp"
#include "LogBST.hpp"
#include "LogHashTable.hpp"
#include <vector>
#include <string>

struct BenchmarkResult {
    std::string structure;
    int n;
    double insert_ms;
    double exact_search_ms;  // Cari berdasarkan Level/Key
    double range_search_ms;  // Cari berdasarkan Rentang Waktu
    double delete_ms;
};

// CSV Loader helper
std::vector<LogEntry> loadCSV(const std::string& path, int limit = -1);

class LogManager {
    LogLinkedList ll;
    LogBST        bst;
    LogHashTable  ht_level;
    LogHashTable  ht_module;
    int total;
    std::vector<BenchmarkResult> last_benchmark_results;

public:
    LogManager();

    void loadFromCSV(const std::string& path, int limit = -1);
    void insertLog(const LogEntry& e);

    std::vector<LogEntry> searchByLevel(const std::string& level);
    std::vector<LogEntry> searchByModule(const std::string& mod);
    std::vector<LogEntry> searchByTimeRange(const std::string& t_start, const std::string& t_end);
    std::vector<LogEntry> getErrors();

    void deleteBefore(const std::string& cutoff);
    void printStatistics();

    int getTotal() const;
    std::vector<BenchmarkResult> getLastBenchmark();

    // ── BENCHMARK VALID & APPLE-TO-APPLE ───────────────────────
    void runBenchmark(const std::vector<LogEntry>& allLogs, const std::vector<int>& sizes);
};

#endif // LOG_MANAGER_HPP
