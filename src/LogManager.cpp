#include "LogManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <random>
#include <iomanip>

using namespace std::chrono;

// ═══════════════════════════════════════════════════════════════
//  CSV LOADER IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════
std::vector<LogEntry> loadCSV(const std::string& path, int limit) {
    std::vector<LogEntry> logs;
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "❌ Gagal membuka file atau file belum ada: " << path << "\n";
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

// ═══════════════════════════════════════════════════════════════
//  BENCHMARK HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════
template<typename Func>
static double measureMs(Func&& f) {
    auto t0 = high_resolution_clock::now();
    f();
    auto t1 = high_resolution_clock::now();
    return duration<double, std::milli>(t1 - t0).count();
}

static void printBenchmarkTable(const std::vector<BenchmarkResult>& results) {
    std::cout << "\n" << std::string(90, '=') << "\n";
    std::cout << std::left
              << std::setw(15) << "Struktur"
              << std::setw(8)  << "n"
              << std::setw(16) << "Insert (ms)"
              << std::setw(18) << "Exact Srch (ms)"
              << std::setw(18) << "Range Srch (ms)"
              << std::setw(15) << "Delete (ms)"
              << "\n" << std::string(90, '-') << "\n";
    for (auto& r : results) {
        std::cout << std::setw(15) << r.structure
                  << std::setw(8)  << r.n
                  << std::setw(16) << std::fixed << std::setprecision(4) << r.insert_ms
                  << std::setw(18) << r.exact_search_ms
                  << std::setw(18) << r.range_search_ms
                  << std::setw(15) << r.delete_ms
                  << "\n";
    }
    std::cout << std::string(90, '=') << "\n";
}

// ═══════════════════════════════════════════════════════════════
//  LOG MANAGER IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════
LogManager::LogManager() : ht_level("level"), ht_module("module"), total(0) {}

void LogManager::loadFromCSV(const std::string& path, int limit) {
    auto logs = loadCSV(path, limit);
    if (logs.empty()) return;
    std::cout << "📂 Memuat " << logs.size() << " entri log ke sistem...\n";
    for (auto& e : logs) insertLog(e);
    std::cout << "✅ Berhasil sinkronisasi: " << total << " log\n";
}

void LogManager::insertLog(const LogEntry& e) {
    ll.insert(e);
    bst.insert(e);
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
    return bst.searchByTimeRange(t_start, t_end);
}

std::vector<LogEntry> LogManager::getErrors() {
    return ht_level.search("ERROR");
}

void LogManager::deleteBefore(const std::string& cutoff) {
    int deleted = ll.deleteBefore(cutoff);
    bst.deleteBefore(cutoff);
    ht_level.deleteBefore(cutoff);
    ht_module.deleteBefore(cutoff);
    total -= deleted;
    std::cout << "🗑️ Berhasil menghapus: " << deleted << " log lawas (sebelum " << cutoff << ")\n";
}

void LogManager::printStatistics() {
    auto stats = ht_level.statistics();
    std::cout << "\n📊 STATISTIK STRUKTUR DATA (HASH TABLE MAP)\n" << std::string(45, '-') << "\n";
    for (auto& [lvl, cnt] : stats) {
        std::cout << "  " << std::setw(12) << std::left << lvl << ": " << cnt << " log\n";
    }
    std::cout << "  " << std::setw(12) << std::left << "TOTAL DATA" << ": " << total << " log\n";
}

int LogManager::getTotal() const {
    return total;
}

std::vector<BenchmarkResult> LogManager::getLastBenchmark() {
    return last_benchmark_results;
}

void LogManager::runBenchmark(const std::vector<LogEntry>& allLogs, const std::vector<int>& sizes) {
    if (allLogs.empty()) {
        std::cout << "⚠️ Tidak ada data untuk melakukan pengujian benchmark.\n";
        return;
    }
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "  MULAI PENGUJIAN BENCHMARK APPLE-TO-APPLE\n";
    std::cout << std::string(70, '=') << "\n";

    last_benchmark_results.clear();
    std::string search_level = "ERROR";

    for (int n : sizes) {
        if (n > (int)allLogs.size()) continue;
        std::vector<LogEntry> sample(allLogs.begin(), allLogs.begin() + n);

        // Shuffling khusus agar BST optimal (mencegah degradasi worst-case)
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

        std::cout << "▶ Menjalankan pengujian untuk n = " << n << " entri...\n";

        // ── Skenario 1: Linked List ──
        {
            LogLinkedList ll_b;
            double t_ins = measureMs([&]{ for (auto& e : sample) ll_b.insert(e); });
            double t_ex  = measureMs([&]{ ll_b.searchByLevel(search_level); });
            double t_rng = measureMs([&]{ ll_b.searchByTimeRange(t_start, t_end); });
            
            LogLinkedList ll_del;
            for (auto& e : sample) ll_del.insert(e);
            double t_del = measureMs([&]{ ll_del.deleteBefore(cutoff); });
            
            last_benchmark_results.push_back({"LinkedList", n, t_ins, t_ex, t_rng, t_del});
        }

        // ── Skenario 2: BST ──
        {
            LogBST bst_b;
            double t_ins = measureMs([&]{ for (auto& e : shuffled) bst_b.insert(e); });
            double t_ex  = measureMs([&]{ bst_b.searchByLevel(search_level); });
            double t_rng = measureMs([&]{ bst_b.searchByTimeRange(t_start, t_end); });
            
            LogBST bst_del;
            for (auto& e : shuffled) bst_del.insert(e);
            double t_del = measureMs([&]{ bst_del.deleteBefore(cutoff); });
            
            last_benchmark_results.push_back({"BST", n, t_ins, t_ex, t_rng, t_del});
        }

        // ── Skenario 3: Hash Table ──
        {
            LogHashTable ht_b("level");
            double t_ins = measureMs([&]{ for (auto& e : sample) ht_b.insert(e); });
            double t_ex  = measureMs([&]{ ht_b.search(search_level); });
            double t_rng = measureMs([&]{ ht_b.searchByTimeRange(t_start, t_end); });
            
            LogHashTable ht_del("level");
            for (auto& e : sample) ht_del.insert(e);
            double t_del = measureMs([&]{ ht_del.deleteBefore(cutoff); });
            
            last_benchmark_results.push_back({"HashTable", n, t_ins, t_ex, t_rng, t_del});
        }
    }
    printBenchmarkTable(last_benchmark_results);
}
