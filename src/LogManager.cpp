#include "LogManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <random>
#include <iomanip>

using namespace std::chrono;

// ===============================================================
//  CSV LOADER IMPLEMENTATION
// ===============================================================
std::vector<LogEntry> loadCSV(const std::string& path, int limit) {
    std::vector<LogEntry> logs;
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "[ERROR] Gagal membuka file atau file belum ada: " << path << "\n";
        return logs;
    }
    std::string line;
    std::getline(f, line); // skip header
    while (std::getline(f, line)) {
        if (limit >= 0 && (int)logs.size() >= limit) break;
        std::istringstream ss(line);
        LogEntry e;
        std::getline(ss, e.log_id,   ',');
        std::getline(ss, e.timestamp,',');
        std::getline(ss, e.level,    ',');
        std::getline(ss, e.module,   ',');
        std::getline(ss, e.message);
        
        if (!e.message.empty() && e.message.back() == '\r') e.message.pop_back();
        if (!e.message.empty() && e.message.front() == '"') {
            e.message = e.message.substr(1);
            if (!e.message.empty() && e.message.back() == '"') e.message.pop_back();
        }
        if (!e.log_id.empty()) logs.push_back(e);
    }
    return logs;
}

// ===============================================================
//  BENCHMARK HELPER FUNCTIONS
// ===============================================================
template<typename Func>
static double measureMsAvg(Func&& f, int repeat = 5) {
    double total_ms = 0;
    for (int i = 0; i < repeat; ++i) {
        auto t0 = high_resolution_clock::now();
        f();
        auto t1 = high_resolution_clock::now();
        total_ms += duration<double, std::milli>(t1 - t0).count();
    }
    return total_ms / repeat;
}

static void printBenchmarkTable(const std::vector<BenchmarkResult>& results) {
    std::cout << "\n" << std::string(105, '=') << "\n";
    std::cout << std::left
              << std::setw(15) << "Struktur"
              << std::setw(8)  << "n"
              << std::setw(16) << "Insert (ms)"
              << std::setw(18) << "Exact Srch (ms)"
              << std::setw(18) << "Range Srch (ms)"
              << std::setw(15) << "Delete (ms)"
              << std::setw(15) << "Memory (KB)"
              << "\n" << std::string(105, '-') << "\n";
    for (auto& r : results) {
        std::cout << std::left
                  << std::setw(15) << r.structure
                  << std::setw(8)  << r.n
                  << std::setw(16) << std::fixed << std::setprecision(4) << r.insert_ms
                  << std::setw(18) << r.exact_search_ms
                  << std::setw(18) << r.range_search_ms
                  << std::setw(15) << r.delete_ms
                  << std::setw(15) << std::setprecision(1) << r.mem_kb
                  << "\n";
    }
    std::cout << std::string(105, '=') << "\n";
}

// ===============================================================
//  LOG MANAGER IMPLEMENTATION
// ===============================================================
LogManager::LogManager() : ht_level("level"), ht_module("module"), total(0) {}

void LogManager::loadFromCSV(const std::string& path, int limit) {
    auto logs = loadCSV(path, limit);
    if (logs.empty()) return;
    std::cout << "[LOAD] Memuat " << logs.size() << " entri log ke sistem...\n";
    for (auto& e : logs) insertLog(e);
    std::cout << "[OK] Berhasil sinkronisasi: " << total << " log\n";
}

void LogManager::insertLog(const LogEntry& e) {
    ll.insert(e);
    avl.insert(e);
    ht_level.insert(e);
    ht_module.insert(e);
    total++;
}

std::vector<LogEntry> LogManager::searchByLevel(const std::string& level) {
    return ht_level.search(level);
}

std::vector<LogEntry> LogManager::searchByModule(const std::string& mod) {
    return ht_module.search(mod);
}

std::vector<LogEntry> LogManager::searchByTimeRange(const std::string& t_start, const std::string& t_end) {
    return avl.searchByTimeRange(t_start, t_end);
}

std::vector<LogEntry> LogManager::getErrors() {
    return ht_level.search("ERROR");
}

void LogManager::deleteBefore(const std::string& cutoff) {
    int deleted = ll.deleteBefore(cutoff);
    avl.deleteBefore(cutoff);
    ht_level.deleteBefore(cutoff);
    ht_module.deleteBefore(cutoff);
    total -= deleted;
    std::cout << "[DELETE] Berhasil menghapus: " << deleted << " log lawas (sebelum " << cutoff << ")\n";
}

void LogManager::printStatistics() {
    auto stats = ht_level.statistics();
    std::cout << "\n[STATS] STATISTIK STRUKTUR DATA (HASH TABLE MAP)\n" << std::string(45, '-') << "\n";
    for (auto& p : stats) {
        std::cout << "  " << std::setw(12) << std::left << p.first << ": " << p.second << " log\n";
    }
    std::cout << "  " << std::setw(12) << std::left << "TOTAL DATA" << ": " << total << " log\n";
}

int LogManager::getTotal() const {
    return total;
}

std::vector<BenchmarkResult> LogManager::getLastBenchmark() {
    return last_benchmark_results;
}

void LogManager::runBenchmark(const std::vector<LogEntry>& allLogs, const std::vector<int>& sizes, int repeat) {
    if (allLogs.empty()) {
        std::cout << "[WARNING] Tidak ada data untuk melakukan pengujian benchmark.\n";
        return;
    }
    std::cout << "\n" << std::string(75, '=') << "\n";
    std::cout << "  MULAI PENGUJIAN BENCHMARK APPLE-TO-APPLE (REPEAT " << repeat << "x)\n";
    std::cout << std::string(75, '=') << "\n";

    last_benchmark_results.clear();
    std::string search_level = "ERROR";

    for (int n : sizes) {
        if (n > (int)allLogs.size()) {
            std::cout << "[SKIP] Lewati n=" << n << " (data tidak cukup, hanya " << allLogs.size() << " entri)\n";
            continue;
        }
        std::vector<LogEntry> sample(allLogs.begin(), allLogs.begin() + n);

        // Shuffling khusus agar AVL optimal (mencegah degradasi worst-case)
        std::vector<LogEntry> shuffled = sample;
        std::mt19937 rng(42);
        std::shuffle(shuffled.begin(), shuffled.end(), rng);

        std::string cutoff = sample[n/2].timestamp;
        std::string t_start = sample[0].timestamp;
        
        // Set Rentang Waktu Pencarian (Simulasi 7 Hari dari Log Pertama)
        LogEntry tmp; tmp.timestamp = t_start;
        long long te_epoch = tmp.toEpoch() + 7LL * 24 * 3600;
        std::time_t te_t = (std::time_t)te_epoch;
        struct std::tm* tminfo = std::localtime(&te_t);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tminfo);
        std::string t_end = std::string(buf);

        std::cout << "\n>> n = " << n << " entri (repeat=" << repeat << "x)...\n";

        // ── Skenario 1: Linked List ──
        {
            LogLinkedList ll_mem;
            for (auto& e : sample) ll_mem.insert(e);
            
            double t_ins = measureMsAvg([&]{ 
                LogLinkedList ll_b;
                for (auto& e : sample) ll_b.insert(e); 
            }, repeat);
            
            double t_ex  = measureMsAvg([&]{ ll_mem.searchByLevel(search_level); }, repeat);
            double t_rng = measureMsAvg([&]{ ll_mem.searchByTimeRange(t_start, t_end); }, repeat);
            
            double t_del = measureMsAvg([&]{ 
                LogLinkedList ll_del;
                for (auto& e : sample) ll_del.insert(e);
                ll_del.deleteBefore(cutoff); 
            }, repeat);
            
            double mem_kb = ll_mem.estimateMemoryBytes() / 1024.0;
            last_benchmark_results.push_back({"LinkedList", n, t_ins, t_ex, t_rng, t_del, mem_kb});
            std::cout << "  [+] LinkedList: ins=" << std::fixed << std::setprecision(3) << t_ins
                      << "ms, search=" << t_ex << "ms, range=" << t_rng
                      << "ms, del=" << t_del << "ms, mem=" << std::setprecision(1) << mem_kb << " KB\n";
        }

        // ── Skenario 2: AVL Tree ──
        {
            LogAVL avl_mem;
            for (auto& e : shuffled) avl_mem.insert(e);
            
            double t_ins = measureMsAvg([&]{ 
                LogAVL avl_b;
                for (auto& e : shuffled) avl_b.insert(e); 
            }, repeat);
            
            double t_ex  = measureMsAvg([&]{ avl_mem.searchByLevel(search_level); }, repeat);
            double t_rng = measureMsAvg([&]{ avl_mem.searchByTimeRange(t_start, t_end); }, repeat);
            
            double t_del = measureMsAvg([&]{ 
                LogAVL avl_del;
                for (auto& e : shuffled) avl_del.insert(e);
                avl_del.deleteBefore(cutoff); 
            }, repeat);
            
            double mem_kb = avl_mem.estimateMemoryBytes() / 1024.0;
            last_benchmark_results.push_back({"AVL Tree", n, t_ins, t_ex, t_rng, t_del, mem_kb});
            std::cout << "  [+] AVL Tree:   ins=" << std::fixed << std::setprecision(3) << t_ins
                      << "ms, search=" << t_ex << "ms, range=" << t_rng
                      << "ms, del=" << t_del << "ms, mem=" << std::setprecision(1) << mem_kb << " KB\n";
        }

        // ── Skenario 3: Hash Table ──
        {
            LogHashTable ht_mem("level");
            for (auto& e : sample) ht_mem.insert(e);
            
            double t_ins = measureMsAvg([&]{ 
                LogHashTable ht_b("level");
                for (auto& e : sample) ht_b.insert(e); 
            }, repeat);
            
            double t_ex  = measureMsAvg([&]{ ht_mem.search(search_level); }, repeat);
            double t_rng = measureMsAvg([&]{ ht_mem.searchByTimeRange(t_start, t_end); }, repeat);
            
            double t_del = measureMsAvg([&]{ 
                LogHashTable ht_del("level");
                for (auto& e : sample) ht_del.insert(e);
                ht_del.deleteBefore(cutoff); 
            }, repeat);
            
            double mem_kb = ht_mem.estimateMemoryBytes() / 1024.0;
            last_benchmark_results.push_back({"HashTable", n, t_ins, t_ex, t_rng, t_del, mem_kb});
            std::cout << "  [+] HashTable:  ins=" << std::fixed << std::setprecision(3) << t_ins
                      << "ms, search=" << t_ex << "ms, range=" << t_rng
                      << "ms, del=" << t_del << "ms, mem=" << std::setprecision(1) << mem_kb << " KB\n";
        }
    }
    printBenchmarkTable(last_benchmark_results);
}
