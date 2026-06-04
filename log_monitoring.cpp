/*
 * ============================================================
 * SISTEM MONITORING & LOG AKTIVITAS APLIKASI
 * Topik 9 — Analisis Struktur Data (Optimized)
 * Bahasa: C++17
 * Struktur Data: Linked List | BST | Hash Table
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <random>
#include <ctime>
#include <stdexcept>
#include <limits>

using namespace std;
using namespace std::chrono;

// ═══════════════════════════════════════════════════════════════
//  STRUKTUR DATA: LogEntry
// ═══════════════════════════════════════════════════════════════
struct LogEntry {
    string log_id;
    string timestamp;
    string level;
    string module;
    string message;
    mutable long long _epoch = -1;

    // Konversi timestamp string ke Unix epoch (long long), di-cache
    long long toEpoch() const {
        if (_epoch >= 0) return _epoch;
        struct tm t = {};
        istringstream ss(timestamp);
        ss >> get_time(&t, "%Y-%m-%d %H:%M:%S");
        t.tm_isdst = -1;
        _epoch = (long long)mktime(&t);
        return _epoch;
    }

    void print() const {
        cout << "[" << timestamp << "] "
             << setw(8) << left << level << " | "
             << setw(12) << left << module << " | "
             << log_id << " | "
             << message << "\n";
    }
};

// ═══════════════════════════════════════════════════════════════
//  STRUKTUR 1: SINGLY LINKED LIST
// ═══════════════════════════════════════════════════════════════
struct LLNode {
    LogEntry entry;
    LLNode* next;
    explicit LLNode(const LogEntry& e) : entry(e), next(nullptr) {}
};

class LogLinkedList {
    LLNode* head;
    LLNode* tail;
    int size_;

public:
    LogLinkedList() : head(nullptr), tail(nullptr), size_(0) {}

    ~LogLinkedList() {
        LLNode* cur = head;
        while (cur) { LLNode* tmp = cur->next; delete cur; cur = tmp; }
    }

    // INSERT — O(1)
    void insert(const LogEntry& e) {
        LLNode* node = new LLNode(e);
        if (!tail) head = tail = node;
        else { tail->next = node; tail = node; }
        size_++;
    }

    // SEARCH by level — O(n)
    vector<LogEntry> searchByLevel(const string& level) const {
        vector<LogEntry> res;
        for (LLNode* cur = head; cur; cur = cur->next)
            if (cur->entry.level == level) res.push_back(cur->entry);
        return res;
    }

    // SEARCH by module — O(n)
    vector<LogEntry> searchByModule(const string& mod) const {
        vector<LogEntry> res;
        for (LLNode* cur = head; cur; cur = cur->next)
            if (cur->entry.module == mod) res.push_back(cur->entry);
        return res;
    }

    // SEARCH by time range — O(n)
    vector<LogEntry> searchByTimeRange(const string& t_start, const string& t_end) const {
        LogEntry tmp; tmp.timestamp = t_start; long long ts = tmp.toEpoch();
        tmp.timestamp = t_end;                 long long te = tmp.toEpoch();
        vector<LogEntry> res;
        for (LLNode* cur = head; cur; cur = cur->next) {
            long long t = cur->entry.toEpoch();
            if (t >= ts && t <= te) res.push_back(cur->entry);
        }
        return res;
    }

    // DELETE log lama (sebelum cutoff) — O(n)
    int deleteBefore(const string& cutoff) {
        LogEntry tmp; tmp.timestamp = cutoff;
        long long cut = tmp.toEpoch();
        int deleted = 0;
        LLNode* prev = nullptr;
        LLNode* cur  = head;
        while (cur) {
            if (cur->entry.toEpoch() < cut) {
                LLNode* del = cur;
                if (!prev) head = cur->next;
                else        prev->next = cur->next;
                if (del == tail) tail = prev;
                cur = cur->next;
                delete del;
                deleted++;
                size_--;
            } else {
                prev = cur;
                cur  = cur->next;
            }
        }
        return deleted;
    }

    // STATISTIK per level — O(n)
    unordered_map<string, int> statistics() const {
        unordered_map<string, int> stats;
        for (LLNode* cur = head; cur; cur = cur->next)
            stats[cur->entry.level]++;
        return stats;
    }

    int size() const { return size_; }

    void printAll(int limit = 10) const {
        int cnt = 0;
        for (LLNode* cur = head; cur && cnt < limit; cur = cur->next, cnt++)
            cur->entry.print();
        if (size_ > limit)
            cout << "  ... (" << size_ - limit << " entri lainnya)\n";
    }
};

// ═══════════════════════════════════════════════════════════════
//  STRUKTUR 2: BINARY SEARCH TREE (by timestamp)
// ═══════════════════════════════════════════════════════════════
struct BSTNode {
    long long key;   // Unix epoch
    LogEntry entry;
    BSTNode* left;
    BSTNode* right;
    explicit BSTNode(const LogEntry& e)
        : entry(e), key(e.toEpoch()), left(nullptr), right(nullptr) {}
};

class LogBST {
    BSTNode* root;
    int size_;

    // Insert iteratif — mencegah stack overflow pada data besar terurut
    void insertIter(const LogEntry& e) {
        BSTNode* node = new BSTNode(e);
        if (!root) { root = node; size_++; return; }
        BSTNode* cur = root;
        while (true) {
            if (node->key <= cur->key) {
                if (!cur->left) { cur->left = node; break; }
                else cur = cur->left;
            } else {
                if (!cur->right) { cur->right = node; break; }
                else cur = cur->right;
            }
        }
        size_++;
    }

    void rangeQuery(BSTNode* node, long long ts, long long te,
                    vector<LogEntry>& res) const {
        if (!node) return;
        if (node->key >= ts) rangeQuery(node->left,  ts, te, res);
        if (node->key >= ts && node->key <= te) res.push_back(node->entry);
        if (node->key <= te) rangeQuery(node->right, ts, te, res);
    }

    // Mengumpulkan node yang lolos cutoff secara efisien (Inorder)
    void collectBefore(BSTNode* node, long long cut,
                       vector<LogEntry>& keep) const {
        if (!node) return;
        collectBefore(node->left,  cut, keep);
        if (node->key >= cut) keep.push_back(node->entry);
        collectBefore(node->right, cut, keep);
    }

    void destroyTree(BSTNode* node) {
        if (!node) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    void inorder(BSTNode* node, vector<LogEntry>& res) const {
        if (!node) return;
        inorder(node->left, res);
        res.push_back(node->entry);
        inorder(node->right, res);
    }

    // Membangun balanced BST dari array terurut untuk mencegah skewing setelah rekonstruksi delete
    BSTNode* buildBalancedFromSorted(const vector<LogEntry>& sorted_logs, int start, int end) {
        if (start > end) return nullptr;
        int mid = start + (end - start) / 2;
        BSTNode* node = new BSTNode(sorted_logs[mid]);
        node->left = buildBalancedFromSorted(sorted_logs, start, mid - 1);
        node->right = buildBalancedFromSorted(sorted_logs, mid + 1, end);
        return node;
    }

public:
    LogBST() : root(nullptr), size_(0) {}
    ~LogBST() { destroyTree(root); }

    // INSERT — O(log n) rata-rata jika teracak
    void insert(const LogEntry& e) {
        insertIter(e);
    }

    // SEARCH by time range — O(log n + k)
    vector<LogEntry> searchByTimeRange(const string& t_start, const string& t_end) const {
        LogEntry tmp; tmp.timestamp = t_start; long long ts = tmp.toEpoch();
        tmp.timestamp = t_end;                 long long te = tmp.toEpoch();
        vector<LogEntry> res;
        rangeQuery(root, ts, te, res);
        return res;
    }

    // SEARCH by level — O(n) traversal
    vector<LogEntry> searchByLevel(const string& level) const {
        vector<LogEntry> all;
        inorder(root, all);
        vector<LogEntry> res;
        for (auto& e : all)
            if (e.level == level) res.push_back(e);
        return res;
    }

    // DELETE log lama — Rebuild tree seimbang O(n) agar BST tidak pincang/skewed
    int deleteBefore(const string& cutoff) {
        LogEntry tmp; tmp.timestamp = cutoff;
        long long cut = tmp.toEpoch();
        vector<LogEntry> keep;
        collectBefore(root, cut, keep);
        int deleted = size_ - (int)keep.size();
        destroyTree(root);
        
        // Membangun ulang secara seimbang (Balanced Tree Reconstruction)
        root = buildBalancedFromSorted(keep, 0, (int)keep.size() - 1);
        size_ = (int)keep.size();
        return deleted;
    }

    vector<LogEntry> getAllSorted() const {
        vector<LogEntry> res;
        inorder(root, res);
        return res;
    }

    int size() const { return size_; }
};

// ═══════════════════════════════════════════════════════════════
//  STRUKTUR 3: HASH TABLE (separate chaining)
// ═══════════════════════════════════════════════════════════════
class LogHashTable {
    string key_field;   // "level" atau "module"
    unordered_map<string, vector<LogEntry>> buckets;
    int size_;

    string getKey(const LogEntry& e) const {
        if (key_field == "level")  return e.level;
        if (key_field == "module") return e.module;
        return e.log_id;
    }

public:
    explicit LogHashTable(const string& kf) : key_field(kf), size_(0) {}

    // INSERT — O(1) amortized
    void insert(const LogEntry& e) {
        buckets[getKey(e)].push_back(e);
        size_++;
    }

    // SEARCH by key — O(1) + O(k)
    vector<LogEntry> search(const string& key) const {
        auto it = buckets.find(key);
        return (it != buckets.end()) ? it->second : vector<LogEntry>{};
    }

    // SEARCH by time range untuk keperluan komparasi adil di benchmark — O(n)
    vector<LogEntry> searchByTimeRange(const string& t_start, const string& t_end) const {
        LogEntry tmp; tmp.timestamp = t_start; long long ts = tmp.toEpoch();
        tmp.timestamp = t_end;                 long long te = tmp.toEpoch();
        vector<LogEntry> res;
        for (auto& [key, chain] : buckets) {
            for (auto& e : chain) {
                long long t = e.toEpoch();
                if (t >= ts && t <= te) res.push_back(e);
            }
        }
        return res;
    }

    // DELETE log lama — O(n)
    int deleteBefore(const string& cutoff) {
        LogEntry tmp; tmp.timestamp = cutoff;
        long long cut = tmp.toEpoch();
        int deleted = 0;
        for (auto& [key, chain] : buckets) {
            int before = chain.size();
            chain.erase(remove_if(chain.begin(), chain.end(),
                [&](const LogEntry& e){ return e.toEpoch() < cut; }),
                chain.end());
            deleted += before - (int)chain.size();
        }
        size_ -= deleted;
        return deleted;
    }

    // STATISTIK — O(1) per level/modul
    unordered_map<string, int> statistics() const {
        unordered_map<string, int> stats;
        for (auto& [key, chain] : buckets)
            stats[key] = (int)chain.size();
        return stats;
    }

    int size() const { return size_; }
};

// ═══════════════════════════════════════════════════════════════
//  BENCHMARK HELPER & DATA WRAPPER
// ═══════════════════════════════════════════════════════════════
struct BenchmarkResult {
    string structure;
    int n;
    double insert_ms;
    double exact_search_ms;  // Cari berdasarkan Level/Key
    double range_search_ms;  // Cari berdasarkan Rentang Waktu
    double delete_ms;
};

template<typename Func>
double measureMs(Func&& f) {
    auto t0 = high_resolution_clock::now();
    f();
    auto t1 = high_resolution_clock::now();
    return duration<double, milli>(t1 - t0).count();
}

void exportBenchmarkToCSV(const string& filename, const vector<BenchmarkResult>& results) {
    ofstream f(filename);
    f << "Struktur,n,Insert (ms),Exact Search Level (ms),Range Search Time (ms),Delete (ms)\n";
    for (auto& r : results) {
        f << r.structure << ","
          << r.n << ","
          << fixed << setprecision(4) << r.insert_ms << ","
          << r.exact_search_ms << ","
          << r.range_search_ms << ","
          << r.delete_ms << "\n";
    }
    f.close();
    cout << "📊 Hasil benchmark berhasil disimpan ke '" << filename << "'\n";
}

void printBenchmarkTable(const vector<BenchmarkResult>& results) {
    cout << "\n" << string(90, '=') << "\n";
    cout << left
         << setw(15) << "Struktur"
         << setw(8)  << "n"
         << setw(16) << "Insert (ms)"
         << setw(18) << "Exact Srch (ms)"
         << setw(18) << "Range Srch (ms)"
         << setw(15) << "Delete (ms)"
         << "\n" << string(90, '-') << "\n";
    for (auto& r : results) {
        cout << setw(15) << r.structure
             << setw(8)  << r.n
             << setw(16) << fixed << setprecision(4) << r.insert_ms
             << setw(18) << r.exact_search_ms
             << setw(18) << r.range_search_ms
             << setw(15) << r.delete_ms
             << "\n";
    }
    cout << string(90, '=') << "\n";
}

// ═══════════════════════════════════════════════════════════════
//  CSV LOADER
// ═══════════════════════════════════════════════════════════════
vector<LogEntry> loadCSV(const string& path, int limit = -1) {
    vector<LogEntry> logs;
    ifstream f(path);
    if (!f.is_open()) {
        cerr << "❌ Gagal membuka file atau file belum ada: " << path << "\n";
        return logs;
    }
    string line;
    getline(f, line); // skip header
    while (getline(f, line)) {
        if (limit >= 0 && (int)logs.size() >= limit) break;
        istringstream ss(line);
        LogEntry e;
        getline(ss, e.log_id,   ',');
        getline(ss, e.timestamp,',');
        getline(ss, e.level,    ',');
        getline(ss, e.module,   ',');
        getline(ss, e.message);
        
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
//  LOG MANAGER — Orkestrator Utama
// ═══════════════════════════════════════════════════════════════
class LogManager {
    LogLinkedList ll;
    LogBST        bst;
    LogHashTable  ht_level;
    LogHashTable  ht_module;
    int total;
    vector<BenchmarkResult> last_benchmark_results;

public:
    LogManager() : ht_level("level"), ht_module("module"), total(0) {}

    void loadFromCSV(const string& path, int limit = -1) {
        auto logs = loadCSV(path, limit);
        if (logs.empty()) return;
        cout << "📂 Memuat " << logs.size() << " entri log ke sistem...\n";
        for (auto& e : logs) insertLog(e);
        cout << "✅ Berhasil sinkronisasi: " << total << " log\n";
    }

    void insertLog(const LogEntry& e) {
        ll.insert(e);
        bst.insert(e);
        ht_level.insert(e);
        ht_module.insert(e);
        total++;
    }

    vector<LogEntry> searchByLevel(const string& level) { return ht_level.search(level); }
    vector<LogEntry> searchByModule(const string& mod) { return ht_module.search(mod); }
    vector<LogEntry> searchByTimeRange(const string& t_start, const string& t_end) { return bst.searchByTimeRange(t_start, t_end); }
    vector<LogEntry> getErrors() { return ht_level.search("ERROR"); }

    void deleteBefore(const string& cutoff) {
        int d1 = ll.deleteBefore(cutoff);
        int d2 = bst.deleteBefore(cutoff);
        int d3 = ht_level.deleteBefore(cutoff);
        int d4 = ht_module.deleteBefore(cutoff);
        total -= d1;
        cout << "🗑️ Berhasil menghapus: " << d1 << " log lawas (sebelum " << cutoff << ")\n";
    }

    void printStatistics() {
        auto stats = ht_level.statistics();
        cout << "\n📊 STATISTIK STRUKTUR DATA (HASH TABLE MAP)\n" << string(45, '-') << "\n";
        for (auto& [lvl, cnt] : stats)
            cout << "  " << setw(12) << left << lvl << ": " << cnt << " log\n";
        cout << "  " << setw(12) << left << "TOTAL DATA" << ": " << total << " log\n";
    }

    int getTotal() const { return total; }
    vector<BenchmarkResult> getLastBenchmark() { return last_benchmark_results; }

    // ── BENCHMARK VALID & APPLE-TO-APPLE ───────────────────────
    void runBenchmark(const vector<LogEntry>& allLogs, const vector<int>& sizes) {
        if(allLogs.empty()){
            cout << "⚠️ Tidak ada data untuk melakukan pengujian benchmark.\n";
            return;
        }
        cout << "\n" << string(70, '=') << "\n";
        cout << "  MULAIPENGUJIAN BENCHMARK APPLE-TO-APPLE\n";
        cout << string(70, '=') << "\n";

        last_benchmark_results.clear();
        string search_level = "ERROR";

        for (int n : sizes) {
            if (n > (int)allLogs.size()) continue;
            vector<LogEntry> sample(allLogs.begin(), allLogs.begin() + n);

            // Shuffling khusus agar BST optimal (mencegah degradasi worst-case)
            vector<LogEntry> shuffled = sample;
            mt19937 rng(42);
            shuffle(shuffled.begin(), shuffled.end(), rng);

            string cutoff = sample[n/2].timestamp;
            string t_start = sample[0].timestamp;
            
            // Set Rentang Waktu Pencarian (Simulasi 7 Hari dari Log Pertama)
            LogEntry tmp; tmp.timestamp = t_start;
            long long te_epoch = tmp.toEpoch() + 7LL * 24 * 3600;
            time_t te_t = (time_t)te_epoch;
            struct tm* tminfo = localtime(&te_t);
            char buf[20];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tminfo);
            string t_end = string(buf);

            cout << "▶ Menjalankan pengujian untuk n = " << n << " entri...\n";

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
};

// ═══════════════════════════════════════════════════════════════
//  CLI INTERFACE
// ═══════════════════════════════════════════════════════════════
void printMenu() {
    cout << "\n╔══════════════════════════════════════╗\n";
    cout << "║   SISTEM MONITORING LOG AKTIVITAS    ║\n";
    cout << "╠══════════════════════════════════════╣\n";
    cout << "║  1. Insert log baru                  ║\n";
    cout << "║  2. Search by level                  ║\n";
    cout << "║  3. Search by modul                  ║\n";
    cout << "║  4. Search by rentang waktu          ║\n";
    cout << "║  5. Tampilkan log ERROR              ║\n";
    cout << "║  6. Delete log lama (by cutoff)      ║\n";
    cout << "║  7. Statistik log                    ║\n";
    cout << "║  8. Jalankan benchmark               ║\n";
    cout << "║  9. Export hasil benchmark ke CSV    ║\n";
    cout << "║  0. Keluar                           ║\n";
    cout << "╚══════════════════════════════════════╝\n";
    cout << "Pilihan: ";
}

void printResults(const vector<LogEntry>& results, int limit = 10) {
    cout << "\n📋 Ditemukan " << results.size() << " log";
    if ((int)results.size() > limit) cout << " (menampilkan " << limit << " pertama)";
    cout << ":\n" << string(70, '-') << "\n";
    int cnt = 0;
    for (auto& e : results) {
        if (cnt++ >= limit) break;
        e.print();
    }
}

// ═══════════════════════════════════════════════════════════════
//  MAIN PROGRAM
// ═══════════════════════════════════════════════════════════════
int main(int argc, char* argv[]) {
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║   Sistem Monitoring & Log Aktivitas Aplikasi             ║\n";
    cout << "║   Topik 9 — Analisis Struktur Data (Fixed & Valid)       ║\n";
    cout << "╚══════════════════════════════════════════════════════════╝\n\n";

    string csv_path = "logs_dummy.csv";
    if (argc > 1) csv_path = argv[1];

    LogManager manager;
    manager.loadFromCSV(csv_path);
    vector<LogEntry> allLogs = loadCSV(csv_path);

    int choice;
    while (true) {
        printMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore();

        if (choice == 0) {
            cout << "👋 Keluar dari sistem. Sampai jumpa!\n";
            break;
        } 
        else if (choice == 1) {
            LogEntry e;
            cout << "Log ID    : "; getline(cin, e.log_id);
            cout << "Timestamp (YYYY-MM-DD HH:MM:SS): "; getline(cin, e.timestamp);
            cout << "Level (INFO/WARNING/ERROR): "; getline(cin, e.level);
            cout << "Module    : "; getline(cin, e.module);
            cout << "Message   : "; getline(cin, e.message);
            manager.insertLog(e);
            cout << "✅ Sukses memasukkan data baru.\n";
        } 
        else if (choice == 2) {
            string level;
            cout << "Level (INFO/WARNING/ERROR): "; getline(cin, level);
            auto t0 = high_resolution_clock::now();
            auto res = manager.searchByLevel(level);
            double ms = duration<double,milli>(high_resolution_clock::now()-t0).count();
            printResults(res);
            cout << "⏱  Waktu Eksplorasi (Hash): " << fixed << setprecision(4) << ms << " ms\n";
        } 
        else if (choice == 3) {
            string mod;
            cout << "Modul (auth/api_gateway/database/payment): "; getline(cin, mod);
            auto t0 = high_resolution_clock::now();
            auto res = manager.searchByModule(mod);
            double ms = duration<double,milli>(high_resolution_clock::now()-t0).count();
            printResults(res);
            cout << "⏱  Waktu Eksplorasi (Hash): " << fixed << setprecision(4) << ms << " ms\n";
        } 
        else if (choice == 4) {
            string t_start, t_end;
            cout << "Waktu mulai (YYYY-MM-DD HH:MM:SS): "; getline(cin, t_start);
            cout << "Waktu akhir (YYYY-MM-DD HH:MM:SS): "; getline(cin, t_end);
            auto t0 = high_resolution_clock::now();
            auto res = manager.searchByTimeRange(t_start, t_end);
            double ms = duration<double,milli>(high_resolution_clock::now()-t0).count();
            printResults(res);
            cout << "⏱  Waktu Eksplorasi (BST): " << fixed << setprecision(4) << ms << " ms\n";
        } 
        else if (choice == 5) {
            auto t0 = high_resolution_clock::now();
            auto res = manager.getErrors();
            double ms = duration<double,milli>(high_resolution_clock::now()-t0).count();
            printResults(res);
            cout << "⏱  Waktu Eksplorasi: " << fixed << setprecision(4) << ms << " ms\n";
        } 
        else if (choice == 6) {
            string cutoff;
            cout << "Cutoff timestamp (YYYY-MM-DD HH:MM:SS): "; getline(cin, cutoff);
            auto t0 = high_resolution_clock::now();
            manager.deleteBefore(cutoff);
            double ms = duration<double,milli>(high_resolution_clock::now()-t0).count();
            cout << "⏱  Total Durasi Eksekusi: " << fixed << setprecision(4) << ms << " ms\n";
        } 
        else if (choice == 7) {
            manager.printStatistics();
        } 
        else if (choice == 8) {
            vector<int> sizes = {1000, 3000, 5000, 10000};
            manager.runBenchmark(allLogs, sizes);
        } 
        else if (choice == 9) {
            auto res = manager.getLastBenchmark();
            if (res.empty()) {
                cout << "⚠️ Silakan jalankan fitur benchmark (Menu 8) terlebih dahulu untuk mengambil data!\n";
            } else {
                exportBenchmarkToCSV("benchmark_results.csv", res);
            }
        } 
        else {
            cout << "⚠️ Opsi menu tidak tersedia.\n";
        }
    }
    return 0;
}