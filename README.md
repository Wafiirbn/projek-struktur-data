# Sistem Monitoring & Log Aktivitas Aplikasi

Proyek ini adalah implementasi sistem pemantauan dan log aktivitas (*log monitoring*) untuk sebuah aplikasi skala besar menggunakan bahasa pemrograman **C++17**. Sistem ini didesain untuk menganalisis performa berbagai struktur data dalam menangani operasi *insert, search, range query*, dan *delete* dengan efisiensi tinggi secara paralel.

## 🚀 Fitur Utama
- **Multi-Structure Storage**: Setiap log yang masuk direkam secara sinkron ke dalam tiga struktur data yang berbeda (Linked List, AVL Tree, Hash Table) untuk tujuan *benchmarking*.
- **Self-Balancing AVL Tree**: Struktur data otomatis melakukan rotasi *Left/Right* untuk memastikan kompleksitas waktu *worst-case O(log n)*.
- **Hash Table Indexing**: Pengindeksan O(1) berbasis *Separate Chaining* untuk memetakan kategori level dan modul sistem.
- **Sistem Benchmark Apple-to-Apple**: Mengukur waktu respons dari masing-masing struktur data dengan dataset bervolume tinggi (*repetisi 5x per skenario*).
- **Dashboard Visualisasi Web**: Laporan *benchmark* diekspor sebagai CSV lalu divisualisasikan dalam *dashboard web* interaktif.

## 📂 Struktur Repositori

Proyek ini mengadopsi arsitektur modular C++ yang memisahkan definisi dan implementasi.

```text
strukdat/
├── include/                     # File Header (.hpp)
│   ├── LogEntry.hpp             # Definisi skema dan objek Log
│   ├── LogLinkedList.hpp        # Struktur Singly Linked List
│   ├── LogAVL.hpp               # Struktur Self-Balancing AVL Tree
│   ├── LogHashTable.hpp         # Struktur Separate Chaining Hash Table
│   └── LogManager.hpp           # Orkestrator yang menggabungkan seluruh struktur
│
├── src/                         # File Implementasi (.cpp)
│   ├── LogEntry.cpp             
│   ├── LogLinkedList.cpp
│   ├── LogAVL.cpp
│   ├── LogHashTable.cpp
│   ├── LogManager.cpp           # Proses CSV Loading & Benchmark System
│   └── main.cpp                 # CLI User Interface
│
├── visualnya/                   # Submodul Web Dashboard (Node.js + Vite)
│   ├── index.html
│   ├── src/
│   │   ├── main.js
│   │   └── style.css
│   └── package.json
│
├── Makefile                     # Skrip otomatisasi build program
└── system_logs.csv              # Dataset sintetis berisi 100k log
```

## 🧠 Struktur Data & Kompleksitas

| Operasi           | Linked List   | AVL Tree         | Hash Table      | Keterangan |
|-------------------|---------------|------------------|-----------------|------------|
| **Insert**        | `O(1)`        | `O(log n)`       | `O(1)` amortized| Node diletakkan pada akhir LL / bucket HT atau melalui penyeimbangan pohon. |
| **Search by Key** | `O(n)`        | `O(n)`           | `O(1) + O(k)`   | Hash Table unggul melalui *Direct Addressing Lookup*. |
| **Range Query**   | `O(n)`        | `O(log n + k)`   | `O(n)`          | AVL Tree memotong *(pruning)* iterasi di luar range batas tanggal pencarian. |
| **Delete Log**    | `O(n)`        | `O(n)`           | `O(n)`          | AVL memanfatkan *rebuild divide-and-conquer* setelah penghapusan terstruktur. |

> *Catatan: n merepresentasikan jumlah seluruh entri, dan k mewakili jumlah entri yang cocok.*

## ⚙️ Cara Menjalankan Program (Kompilasi C++)

Untuk menjalankan CLI aplikasi berbasis C++, Anda membutuhkan **Compiler GCC/G++** (MinGW pada Windows) yang mendukung C++17.

**Langkah 1: Kompilasi menggunakan Makefile**
```bash
# Untuk pengguna Windows
mingw32-make

# Untuk pengguna Linux/macOS
make
```

**Langkah 2: Menjalankan Program**
```bash
./log_monitoring system_logs.csv
```

**Langkah 3: Eksekusi Benchmark**
1. Pilih menu `8` untuk menjalankan *Apple-to-Apple Benchmark* (Dibutuhkan beberapa menit untuk ribuan entri data).
2. Setelah selesai, pilih menu `9` untuk mengekspor ke file `benchmark_results.csv`.

## 📊 Cara Menjalankan Visualisasi Web

Aplikasi web ini menggunakan bundler **Vite** yang memungkinkan penampilan *dashboard* grafik interaktif untuk hasil eksperimen.

1. Buka terminal baru dan masuk ke folder *visualnya*.
   ```bash
   cd visualnya
   ```
2. Lakukan instalasi seluruh *dependencies* Node.js.
   ```bash
   npm install
   ```
3. Jalankan server di *localhost*.
   ```bash
   npm run dev
   ```
4. Buka *browser* di URL `http://localhost:5173`. Pastikan file `benchmark_results.csv` Anda sudah terbentuk sebelum menggunakan visualisasi.

## 📝 Lisensi
Proyek ini dibuat untuk keperluan Topik 9 — Analisis Struktur Data (Final).
