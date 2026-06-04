/*
 * ============================================================
 * SISTEM MONITORING & LOG AKTIVITAS APLIKASI
 * Topik 9 — Analisis Struktur Data (Optimized)
 * Bahasa: C++17
 * Struktur Data: Linked List | BST | Hash Table
 * ============================================================
 */

#include "LogManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <limits>

using namespace std;
using namespace std::chrono;

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
