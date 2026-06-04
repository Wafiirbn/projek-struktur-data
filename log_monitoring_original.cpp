/*
 * ============================================================
 * SISTEM MONITORING & LOG AKTIVITAS APLIKASI
 * Topik 9 ΓÇö Analisis Struktur Data (Final ΓÇö Minggu ke-14)
 * Bahasa: C++17
 * Struktur Data: Linked List | AVL Tree | Hash Table
 * Fitur Tambahan: Pengukuran Memori, Benchmark 5x Repeat
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
#include <cmath>

using namespace std;
using namespace std::chrono;

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  STRUKTUR DATA: LogEntry
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
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
             << setw(14) << left << module << " | "
             << log_id << " | "
             << message << "\n";
    }
};

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  STRUKTUR 1: SINGLY LINKED LIST
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
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

    // INSERT ΓÇö O(1)
    void insert(const LogEntry& e) {
        LLNode* node = new LLNode(e);
        if (!tail) head = tail = node;
        else { tail->next = node; tail = node; }
        size_++;
    }

    // SEARCH by level ΓÇö O(n)
    vector<LogEntry> searchByLevel(const string& level) const {
        vector<LogEntry> res;
        for (LLNode* cur = head; cur; cur = cur->next)
            if (cur->entry.level == level) res.push_back(cur->entry);
        return res;
    }

    // SEARCH by module ΓÇö O(n)
    vector<LogEntry> searchByModule(const string& mod) const {
        vector<LogEntry> res;
        for (LLNode* cur = head; cur; cur = cur->next)
            if (cur->entry.module == mod) res.push_back(cur->entry);
        return res;
    }

    // SEARCH by time range ΓÇö O(n)
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

    // DELETE log lama (sebelum cutoff) ΓÇö O(n)
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

    // STATISTIK per level ΓÇö O(n)
    unordered_map<string, int> statistics() const {
        unordered_map<string, int> stats;
        for (LLNode* cur = head; cur; cur = cur->next)
            stats[cur->entry.level]++;
        return stats;
    }

    // Estimasi memori (bytes): overhead node + string storage
    size_t estimateMemoryBytes() const {
        // Per node: pointer (8) + LogEntry (~200 bytes rata-rata)
        return (size_t)size_ * (sizeof(LLNode) + 200);
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

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  STRUKTUR 2: AVL TREE (Self-Balancing BST by timestamp)
//  Keunggulan vs BST biasa: Selalu O(log n) worst-case
//  karena menjaga |balance factor| <= 1 dengan rotasi otomatis
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
struct AVLNode {
    long long key;   // Unix epoch
    LogEntry entry;
    AVLNode* left;
    AVLNode* right;
    int height;
    explicit AVLNode(const LogEntry& e)
        : entry(e), key(e.toEpoch()), left(nullptr), right(nullptr), height(1) {}
};

class LogAVL {
    AVLNode* root;
    int size_;

    int getHeight(AVLNode* n) const { return n ? n->height : 0; }

    int getBalance(AVLNode* n) const {
        return n ? getHeight(n->left) - getHeight(n->right) : 0;
    }

    void updateHeight(AVLNode* n) {
        if (n) n->height = 1 + max(getHeight(n->left), getHeight(n->right));
    }

    // Rotasi kanan (untuk kasus Left-Left)
    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left  = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    // Rotasi kiri (untuk kasus Right-Right)
    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y  = x->right;
        AVLNode* T2 = y->left;
        y->left  = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    // Balance node setelah insert/delete
    AVLNode* balance(AVLNode* node) {
        updateHeight(node);
        int bf = getBalance(node);

        // Left-Left Case
        if (bf > 1 && getBalance(node->left) >= 0)
            return rotateRight(node);
        // Left-Right Case
        if (bf > 1 && getBalance(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // Right-Right Case
        if (bf < -1 && getBalance(node->right) <= 0)
            return rotateLeft(node);
        // Right-Left Case
        if (bf < -1 && getBalance(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        return node;
    }

    AVLNode* insertNode(AVLNode* node, const LogEntry& e) {
        if (!node) return new AVLNode(e);
        if (e.toEpoch() <= node->key)
            node->left  = insertNode(node->left,  e);
        else
            node->right = insertNode(node->right, e);
        return balance(node);
    }

    void rangeQuery(AVLNode* node, long long ts, long long te,
                    vector<LogEntry>& res) const {
        if (!node) return;
        if (node->key >= ts) rangeQuery(node->left,  ts, te, res);
        if (node->key >= ts && node->key <= te) res.push_back(node->entry);
        if (node->key <= te) rangeQuery(node->right, ts, te, res);
    }

    void collectGeq(AVLNode* node, long long cut,
                    vector<LogEntry>& keep) const {
        if (!node) return;
        collectGeq(node->left,  cut, keep);
        if (node->key >= cut) keep.push_back(node->entry);
        collectGeq(node->right, cut, keep);
    }

    void destroyTree(AVLNode* node) {
        if (!node) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    void inorder(AVLNode* node, vector<LogEntry>& res) const {
        if (!node) return;
        inorder(node->left, res);
        res.push_back(node->entry);
        inorder(node->right, res);
    }

    // Build balanced AVL dari array terurut ΓÇö digunakan setelah delete rekonstruksi
    AVLNode* buildFromSorted(const vector<LogEntry>& logs, int start, int end) {
        if (start > end) return nullptr;
        int mid = start + (end - start) / 2;
        AVLNode* node = new AVLNode(logs[mid]);
        node->left  = buildFromSorted(logs, start, mid - 1);
        node->right = buildFromSorted(logs, mid + 1, end);
        updateHeight(node);
        return node;
    }

public:
    LogAVL() : root(nullptr), size_(0) {}
    ~LogAVL() { destroyTree(root); }

    // INSERT ΓÇö O(log n) WORST-CASE (AVL selalu seimbang)
    void insert(const LogEntry& e) {
        root = insertNode(root, e);
        size_++;
    }

    // SEARCH by time range ΓÇö O(log n + k)
    vector<LogEntry> searchByTimeRange(const string& t_start, const string& t_end) const {
        LogEntry tmp; tmp.timestamp = t_start; long long ts = tmp.toEpoch();
        tmp.timestamp = t_end;                 long long te = tmp.toEpoch();
        vector<LogEntry> res;
        rangeQuery(root, ts, te, res);
        return res;
    }

    // SEARCH by level ΓÇö O(n) traversal (AVL tidak diindeks by level)
    vector<LogEntry> searchByLevel(const string& level) const {
        vector<LogEntry> all;
        inorder(root, all);
        vector<LogEntry> res;
        for (auto& e : all)
            if (e.level == level) res.push_back(e);
        return res;
    }

    // DELETE log lama ΓÇö Rebuild dari array terurut O(n)
    int deleteBefore(const string& cutoff) {
        LogEntry tmp; tmp.timestamp = cutoff;
        long long cut = tmp.toEpoch();
        vector<LogEntry> keep;
        collectGeq(root, cut, keep);
        int deleted = size_ - (int)keep.size();
        destroyTree(root);
        root  = buildFromSorted(keep, 0, (int)keep.size() - 1);
        size_ = (int)keep.size();
        return deleted;
    }

    vector<LogEntry> getAllSorted() const {
        vector<LogEntry> res;
        inorder(root, res);
        return res;
    }

    // Estimasi memori: per node (pointer kiri+kanan+parent) + LogEntry + height int
    size_t estimateMemoryBytes() const {
        return (size_t)size_ * (sizeof(AVLNode) + 200);
    }

    int getTreeHeight() const { return getHeight(root); }

    int size() const { return size_; }
};

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  STRUKTUR 3: HASH TABLE (separate chaining)
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
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

    // INSERT ΓÇö O(1) amortized
    void insert(const LogEntry& e) {
        buckets[getKey(e)].push_back(e);
        size_++;
    }

    // SEARCH by key ΓÇö O(1) + O(k)
    vector<LogEntry> search(const string& key) const {
        auto it = buckets.find(key);
        return (it != buckets.end()) ? it->second : vector<LogEntry>{};
    }

    // SEARCH by time range ΓÇö O(n) untuk komparasi benchmark
    vector<LogEntry> searchByTimeRange(const string& t_start, const string& t_end) const {
        LogEntry tmp; tmp.timestamp = t_start; long long ts = tmp.toEpoch();
        tmp.timestamp = t_end;                 long long te = tmp.toEpoch();
        vector<LogEntry> res;
        for (auto it = buckets.begin(); it != buckets.end(); ++it) {
            for (auto& e : it->second) {
                long long t = e.toEpoch();
                if (t >= ts && t <= te) res.push_back(e);
            }
        }
        return res;
    }

    // DELETE log lama ΓÇö O(n)
    int deleteBefore(const string& cutoff) {
        LogEntry tmp; tmp.timestamp = cutoff;
        long long cut = tmp.toEpoch();
        int deleted = 0;
        for (auto it = buckets.begin(); it != buckets.end(); ++it) {
            auto& chain = it->second;
            int before = chain.size();
            chain.erase(remove_if(chain.begin(), chain.end(),
                [&](const LogEntry& e){ return e.toEpoch() < cut; }),
                chain.end());
            deleted += before - (int)chain.size();
        }
        size_ -= deleted;
        return deleted;
    }

    // STATISTIK ΓÇö O(1) per level/modul
    unordered_map<string, int> statistics() const {
        unordered_map<string, int> stats;
        for (auto it = buckets.begin(); it != buckets.end(); ++it)
            stats[it->first] = (int)it->second.size();
        return stats;
    }

    // Estimasi memori: unordered_map overhead + per entry
    size_t estimateMemoryBytes() const {
        return (size_t)size_ * (sizeof(LogEntry) + 200) + buckets.size() * 64;
    }

    int size() const { return size_; }
};

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  BENCHMARK HELPER
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
struct BenchmarkResult {
    string structure;
    int n;
    double insert_ms;
    double exact_search_ms;    // Cari berdasarkan Level/Key
    double range_search_ms;    // Cari berdasarkan Rentang Waktu
    double delete_ms;
    double insert_mem_kb;      // Estimasi memori setelah insert (KB)
};

template<typename Func>
double measureMs(Func&& f) {
    auto t0 = high_resolution_clock::now();
    f();
    auto t1 = high_resolution_clock::now();
    return duration<double, milli>(t1 - t0).count();
}

// Rata-rata dari multiple run untuk stabilitas hasil
template<typename Func>
double measureMsAvg(Func&& f, int repeat = 5) {
    double total = 0.0;
    for (int i = 0; i < repeat; i++) total += measureMs(f);
    return total / repeat;
}

void exportBenchmarkToCSV(const string& filename, const vector<BenchmarkResult>& results) {
    ofstream f(filename);
    f << "Struktur,n,Insert (ms),Exact Search Level (ms),Range Search Time (ms),Delete (ms),Insert Memory (KB)\n";
    for (auto& r : results) {
        f << r.structure << ","
          << r.n << ","
          << fixed << setprecision(4) << r.insert_ms << ","
          << r.exact_search_ms << ","
          << r.range_search_ms << ","
          << r.delete_ms << ","
          << r.insert_mem_kb << "\n";
    }
    f.close();
    cout << "≡ƒôè Hasil benchmark berhasil disimpan ke '" << filename << "'\n";
}

void printBenchmarkTable(const vector<BenchmarkResult>& results) {
    cout << "\n" << string(105, '=') << "\n";
    cout << left
         << setw(12) << "Struktur"
         << setw(8)  << "n"
         << setw(14) << "Insert (ms)"
         << setw(18) << "Exact Srch (ms)"
         << setw(18) << "Range Srch (ms)"
         << setw(14) << "Delete (ms)"
         << setw(16) << "Memori (KB)"
         << "\n" << string(105, '-') << "\n";
    for (auto& r : results) {
        cout << setw(12) << r.structure
             << setw(8)  << r.n
             << setw(14) << fixed << setprecision(4) << r.insert_ms
             << setw(18) << r.exact_search_ms
             << setw(18) << r.range_search_ms
             << setw(14) << r.delete_ms
             << setw(16) << setprecision(1) << r.insert_mem_kb
             << "\n";
    }
    cout << string(105, '=') << "\n";
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  CSV LOADER
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
vector<LogEntry> loadCSV(const string& path, int limit = -1) {
    vector<LogEntry> logs;
    ifstream f(path);
    if (!f.is_open()) {
        cerr << "Γ¥î Gagal membuka file: " << path << "\n";
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

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  LOG MANAGER ΓÇö Orkestrator Utama
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
class LogManager {
    LogLinkedList ll;
    LogAVL        avl;
    LogHashTable  ht_level;
    LogHashTable  ht_module;
    int total;
    vector<BenchmarkResult> last_benchmark_results;

public:
    LogManager() : ht_level("level"), ht_module("module"), total(0) {}

    void loadFromCSV(const string& path, int limit = -1) {
        auto logs = loadCSV(path, limit);
        if (logs.empty()) return;
        cout << "≡ƒôé Memuat " << logs.size() << " entri log ke sistem...\n";
        for (auto& e : logs) insertLog(e);
        cout << "Γ£à Sinkronisasi selesai: " << total << " log dimuat\n";
    }

    void insertLog(const LogEntry& e) {
        ll.insert(e);
        avl.insert(e);
        ht_level.insert(e);
        ht_module.insert(e);
        total++;
    }

    vector<LogEntry> searchByLevel(const string& level)  { return ht_level.search(level); }
    vector<LogEntry> searchByModule(const string& mod)   { return ht_module.search(mod); }
    vector<LogEntry> searchByTimeRange(const string& ts, const string& te) { return avl.searchByTimeRange(ts, te); }
    vector<LogEntry> getErrors()                         { return ht_level.search("ERROR"); }

    void deleteBefore(const string& cutoff) {
        int d1 = ll.deleteBefore(cutoff);
        int d2 = avl.deleteBefore(cutoff);
        int d3 = ht_level.deleteBefore(cutoff);
        int d4 = ht_module.deleteBefore(cutoff);
        total -= d1;
        cout << "≡ƒùæ∩╕Å Berhasil menghapus: " << d1 << " log lawas (sebelum " << cutoff << ")\n";
    }

    void printStatistics() {
        auto stats = ht_level.statistics();
        cout << "\n≡ƒôè STATISTIK SISTEM LOG\n" << string(50, '-') << "\n";
        int grand_total = 0;
        for (auto it = stats.begin(); it != stats.end(); ++it) {
            cout << "  " << setw(12) << left << it->first << ": " << setw(8) << it->second << " log\n";
            grand_total += it->second;
        }
        cout << string(50, '-') << "\n";
        cout << "  " << setw(12) << left << "TOTAL" << ": " << grand_total << " log\n";
        cout << "\n  AVL Tree tinggi saat ini: " << avl.getTreeHeight() << "\n";
    }

    int getTotal() const { return total; }
    vector<BenchmarkResult> getLastBenchmark() { return last_benchmark_results; }

    // ΓöÇΓöÇ BENCHMARK APPLE-TO-APPLE dengan 5x Repeat ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
    void runBenchmark(const vector<LogEntry>& allLogs, const vector<int>& sizes,
                      int repeat = 5) {
        if (allLogs.empty()) {
            cout << "ΓÜá∩╕Å Tidak ada data untuk benchmark.\n";
            return;
        }
        cout << "\n" << string(75, '=') << "\n";
        cout << "  PENGUJIAN BENCHMARK (Repeat=" << repeat << "x, Rata-rata)\n";
        cout << "  Struktur Data: Linked List | AVL Tree | Hash Table\n";
        cout << string(75, '=') << "\n";

        last_benchmark_results.clear();
        const string search_level = "ERROR";

        for (int n : sizes) {
            if (n > (int)allLogs.size()) {
                cout << "ΓÜá∩╕Å Lewati n=" << n << " (data tidak cukup, hanya " << allLogs.size() << " entri)\n";
                continue;
            }
            vector<LogEntry> sample(allLogs.begin(), allLogs.begin() + n);

            // Shuffle untuk AVL agar insert lebih natural (bukan sorted)
            vector<LogEntry> shuffled = sample;
            mt19937 rng(42);
            shuffle(shuffled.begin(), shuffled.end(), rng);

            string cutoff = sample[n / 2].timestamp;
            string t_start = sample[0].timestamp;

            // Hitung t_end = t_start + 30 hari
            LogEntry tmp; tmp.timestamp = t_start;
            long long te_epoch = tmp.toEpoch() + 30LL * 24 * 3600;
            time_t te_t = (time_t)te_epoch;
            struct tm* tminfo = localtime(&te_t);
            char buf[20];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tminfo);
            string t_end = string(buf);

            cout << "\nΓû╢ n = " << n << " entri (repeat=" << repeat << "x)...\n";

            // ΓöÇΓöÇ Skenario 1: Linked List ΓöÇΓöÇ
            {
                // INSERT (repeat kali, rata-rata)
                double t_ins = measureMsAvg([&]{
                    LogLinkedList ll_b;
                    for (auto& e : sample) ll_b.insert(e);
                }, repeat);

                // EXACT SEARCH (repeat kali, menggunakan list yang sudah terisi)
                LogLinkedList ll_filled;
                for (auto& e : sample) ll_filled.insert(e);
                double t_ex = measureMsAvg([&]{ ll_filled.searchByLevel(search_level); }, repeat);
                double t_rng = measureMsAvg([&]{ ll_filled.searchByTimeRange(t_start, t_end); }, repeat);

                // DELETE (repeat kali, setiap kali rebuild dulu)
                double t_del_total = 0.0;
                for (int r = 0; r < repeat; r++) {
                    LogLinkedList ll_del;
                    for (auto& e : sample) ll_del.insert(e);
                    t_del_total += measureMs([&]{ ll_del.deleteBefore(cutoff); });
                }
                double t_del = t_del_total / repeat;

                // Memori
                LogLinkedList ll_mem;
                for (auto& e : sample) ll_mem.insert(e);
                double mem_kb = ll_mem.estimateMemoryBytes() / 1024.0;

                last_benchmark_results.push_back({"LinkedList", n, t_ins, t_ex, t_rng, t_del, mem_kb});
                cout << "  Γ£ô LinkedList: ins=" << fixed << setprecision(3) << t_ins
                     << "ms, search=" << t_ex << "ms, range=" << t_rng
                     << "ms, del=" << t_del << "ms, mem=" << setprecision(1) << mem_kb << " KB\n";
            }

            // ΓöÇΓöÇ Skenario 2: AVL Tree ΓöÇΓöÇ
            {
                double t_ins = measureMsAvg([&]{
                    LogAVL avl_b;
                    for (auto& e : shuffled) avl_b.insert(e);
                }, repeat);

                LogAVL avl_filled;
                for (auto& e : shuffled) avl_filled.insert(e);
                double t_ex  = measureMsAvg([&]{ avl_filled.searchByLevel(search_level); }, repeat);
                double t_rng = measureMsAvg([&]{ avl_filled.searchByTimeRange(t_start, t_end); }, repeat);

                double t_del_total = 0.0;
                for (int r = 0; r < repeat; r++) {
                    LogAVL avl_del;
                    for (auto& e : shuffled) avl_del.insert(e);
                    t_del_total += measureMs([&]{ avl_del.deleteBefore(cutoff); });
                }
                double t_del = t_del_total / repeat;

                LogAVL avl_mem;
                for (auto& e : shuffled) avl_mem.insert(e);
                double mem_kb = avl_mem.estimateMemoryBytes() / 1024.0;

                last_benchmark_results.push_back({"AVL Tree", n, t_ins, t_ex, t_rng, t_del, mem_kb});
                cout << "  Γ£ô AVL Tree:  ins=" << fixed << setprecision(3) << t_ins
                     << "ms, search=" << t_ex << "ms, range=" << t_rng
                     << "ms, del=" << t_del << "ms, mem=" << setprecision(1) << mem_kb << " KB\n";
            }

            // ΓöÇΓöÇ Skenario 3: Hash Table ΓöÇΓöÇ
            {
                double t_ins = measureMsAvg([&]{
                    LogHashTable ht_b("level");
                    for (auto& e : sample) ht_b.insert(e);
                }, repeat);

                LogHashTable ht_filled("level");
                for (auto& e : sample) ht_filled.insert(e);
                double t_ex  = measureMsAvg([&]{ ht_filled.search(search_level); }, repeat);
                double t_rng = measureMsAvg([&]{ ht_filled.searchByTimeRange(t_start, t_end); }, repeat);

                double t_del_total = 0.0;
                for (int r = 0; r < repeat; r++) {
                    LogHashTable ht_del("level");
                    for (auto& e : sample) ht_del.insert(e);
                    t_del_total += measureMs([&]{ ht_del.deleteBefore(cutoff); });
                }
                double t_del = t_del_total / repeat;

                LogHashTable ht_mem("level");
                for (auto& e : sample) ht_mem.insert(e);
                double mem_kb = ht_mem.estimateMemoryBytes() / 1024.0;

                last_benchmark_results.push_back({"HashTable", n, t_ins, t_ex, t_rng, t_del, mem_kb});
                cout << "  Γ£ô HashTable: ins=" << fixed << setprecision(3) << t_ins
                     << "ms, search=" << t_ex << "ms, range=" << t_rng
                     << "ms, del=" << t_del << "ms, mem=" << setprecision(1) << mem_kb << " KB\n";
            }
        }
        printBenchmarkTable(last_benchmark_results);
    }
};

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  CLI INTERFACE
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void printMenu() {
    cout << "\nΓòöΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòù\n";
    cout << "Γòæ   SISTEM MONITORING LOG AKTIVITAS          Γòæ\n";
    cout << "Γòæ   Struktur: Linked List | AVL | HashTable  Γòæ\n";
    cout << "ΓòáΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòú\n";
    cout << "Γòæ  1. Insert log baru                        Γòæ\n";
    cout << "Γòæ  2. Search by level                        Γòæ\n";
    cout << "Γòæ  3. Search by modul                        Γòæ\n";
    cout << "Γòæ  4. Search by rentang waktu                Γòæ\n";
    cout << "Γòæ  5. Tampilkan log ERROR                    Γòæ\n";
    cout << "Γòæ  6. Delete log lama (by cutoff)            Γòæ\n";
    cout << "Γòæ  7. Statistik log                          Γòæ\n";
    cout << "Γòæ  8. Jalankan benchmark                     Γòæ\n";
    cout << "Γòæ  9. Export hasil benchmark ke CSV          Γòæ\n";
    cout << "Γòæ  0. Keluar                                 Γòæ\n";
    cout << "ΓòÜΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓò¥\n";
    cout << "Pilihan: ";
}

void printResults(const vector<LogEntry>& results, int limit = 10) {
    cout << "\n≡ƒôï Ditemukan " << results.size() << " log";
    if ((int)results.size() > limit) cout << " (menampilkan " << limit << " pertama)";
    cout << ":\n" << string(75, '-') << "\n";
    int cnt = 0;
    for (auto& e : results) {
        if (cnt++ >= limit) break;
        e.print();
    }
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  MAIN PROGRAM
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
int main(int argc, char* argv[]) {
    cout << "ΓòöΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòù\n";
    cout << "Γòæ   Sistem Monitoring & Log Aktivitas Aplikasi                 Γòæ\n";
    cout << "Γòæ   Topik 9 ΓÇö Analisis Struktur Data (Final ΓÇö Minggu ke-14)    Γòæ\n";
    cout << "Γòæ   Struktur: Linked List | AVL Tree | Hash Table              Γòæ\n";
    cout << "ΓòÜΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓò¥\n\n";

    // Default: system_logs.csv (100k entries)
    string csv_path = "system_logs.csv";
    if (argc > 1) csv_path = argv[1];

    cout << "≡ƒôü Menggunakan dataset: " << csv_path << "\n";

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
            cout << "≡ƒæï Keluar dari sistem. Sampai jumpa!\n";
            break;
        }
        else if (choice == 1) {
            LogEntry e;
            cout << "Log ID    : "; getline(cin, e.log_id);
            cout << "Timestamp (YYYY-MM-DD HH:MM:SS): "; getline(cin, e.timestamp);
            cout << "Level (INFO/WARNING/ERROR/DEBUG/CRITICAL): "; getline(cin, e.level);
            cout << "Module    : "; getline(cin, e.module);
            cout << "Message   : "; getline(cin, e.message);
            manager.insertLog(e);
            cout << "Γ£à Log baru berhasil ditambahkan ke sistem.\n";
        }
        else if (choice == 2) {
            string level;
            cout << "Level (INFO/WARNING/ERROR/DEBUG/CRITICAL): "; getline(cin, level);
            auto t0 = high_resolution_clock::now();
            auto res = manager.searchByLevel(level);
            double ms = duration<double,milli>(high_resolution_clock::now()-t0).count();
            printResults(res);
            cout << "ΓÅ▒  Waktu Pencarian (Hash Table, O(1)): " << fixed << setprecision(4) << ms << " ms\n";
        }
        else if (choice == 3) {
            string mod;
            cout << "Modul: "; getline(cin, mod);
            auto t0 = high_resolution_clock::now();
            auto res = manager.searchByModule(mod);
            double ms = duration<double,milli>(high_resolution_clock::now()-t0).count();
            printResults(res);
            cout << "ΓÅ▒  Waktu Pencarian (Hash Table, O(1)): " << fixed << setprecision(4) << ms << " ms\n";
        }
        else if (choice == 4) {
            string t_start, t_end;
            cout << "Waktu mulai (YYYY-MM-DD HH:MM:SS): "; getline(cin, t_start);
            cout << "Waktu akhir (YYYY-MM-DD HH:MM:SS): "; getline(cin, t_end);
            auto t0 = high_resolution_clock::now();
            auto res = manager.searchByTimeRange(t_start, t_end);
            double ms = duration<double,milli>(high_resolution_clock::now()-t0).count();
            printResults(res);
            cout << "ΓÅ▒  Waktu Pencarian (AVL Tree, O(log n + k)): " << fixed << setprecision(4) << ms << " ms\n";
        }
        else if (choice == 5) {
            auto t0 = high_resolution_clock::now();
            auto res = manager.getErrors();
            double ms = duration<double,milli>(high_resolution_clock::now()-t0).count();
            printResults(res);
            cout << "ΓÅ▒  Waktu Pencarian ERROR (Hash Table): " << fixed << setprecision(4) << ms << " ms\n";
        }
        else if (choice == 6) {
            string cutoff;
            cout << "Cutoff timestamp (YYYY-MM-DD HH:MM:SS): "; getline(cin, cutoff);
            auto t0 = high_resolution_clock::now();
            manager.deleteBefore(cutoff);
            double ms = duration<double,milli>(high_resolution_clock::now()-t0).count();
            cout << "ΓÅ▒  Total Durasi Delete: " << fixed << setprecision(4) << ms << " ms\n";
        }
        else if (choice == 7) {
            manager.printStatistics();
        }
        else if (choice == 8) {
            // Ukuran benchmark: 1k, 5k, 10k, 25k, 50k, 100k
            vector<int> sizes = {1000, 5000, 10000, 25000, 50000, 100000};
            cout << "\nΓÜÖ∩╕Å Benchmark sizes: 1k, 5k, 10k, 25k, 50k, 100k (5x repeat masing-masing)\n";
            cout << "ΓÅ│ Proses ini membutuhkan beberapa menit untuk dataset besar...\n";
            manager.runBenchmark(allLogs, sizes, 5);
        }
        else if (choice == 9) {
            auto res = manager.getLastBenchmark();
            if (res.empty()) {
                cout << "ΓÜá∩╕Å Silakan jalankan benchmark (Menu 8) terlebih dahulu!\n";
            } else {
                exportBenchmarkToCSV("benchmark_results.csv", res);
            }
        }
        else {
            cout << "ΓÜá∩╕Å Opsi menu tidak tersedia.\n";
        }
    }
    return 0;
}
