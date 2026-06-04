# 📊 Dashboard Visualisasi Benchmark Struktur Data

Panduan penggunaan dashboard interaktif hasil benchmark sistem log monitoring.

## 🚀 Menjalankan Dashboard

### Instalasi Dependencies
```bash
cd visualnya
npm install
```

### Jalankan Development Server
```bash
npm run dev
```

Server akan berjalan di `http://localhost:5174` (atau port lain jika ada konflik).

## 📈 Fitur Visualisasi

Dashboard menampilkan 4 chart interaktif yang dibuat dengan **Plotly.js**:

### 1. Grafik INSERT
- **Tipe**: Grouped Bar Chart
- **Menampilkan**: Waktu eksekusi operasi penyisipan (ms)
- **Struktur Data**: Hash Table, BST, Linked List
- **X-axis**: Jumlah data (N) - 1000, 3000, 5000, 10000

### 2. Grafik SEARCH
- **Tipe**: Grouped Bar Chart
- **Menampilkan**: Waktu eksekusi operasi pencarian (ms)
- **Insight**: BST menunjukkan performa terbaik untuk pencarian

### 3. Grafik DELETE
- **Tipe**: Grouped Bar Chart
- **Menampilkan**: Waktu eksekusi operasi penghapusan (ms)
- **Catatan**: Linked List menunjukkan performa lebih lambat

### 4. Grafik Skalabilitas
- **Tipe**: Line Chart dengan markers
- **Menampilkan**: Pertumbuhan kompleksitas waktu semua operasi
- **Fitur**: 
  - Interactive legend (klik untuk show/hide)
  - Hover untuk melihat detail nilai
  - Export sebagai PNG

## 🎨 Warna & Legend

| Struktur | Warna | Kompleksitas |
|----------|-------|--------------|
| Hash Table | 🔵 Biru | O(1) rata-rata |
| BST | 🟠 Oranye | O(log n) rata-rata |
| Linked List | 🟢 Hijau | O(n) untuk delete |

## 🔧 File Struktur

```
visualnya/
├── index.html          # HTML utama dengan div untuk chart
├── style.css           # Styling dashboard
├── src/
│   └── main.js         # Script Plotly & logika visualisasi
├── benchmark_results.csv  # Data hasil benchmark
└── package.json        # Dependencies
```

## 💾 Update Data Benchmark

Untuk menggunakan data benchmark terbaru:

1. **Jalankan program C++ benchmark** untuk generate `benchmark_results.csv`
2. **Ganti file** `visualnya/benchmark_results.csv` dengan file terbaru
3. **Refresh browser** - Plotly akan otomatis memuat data baru

Format CSV harus sesuai:
```csv
Struktur,n,Insert (ms),Search (ms),Delete (ms)
LinkedList,1000,0.0514,0.0107,1.1234
BST,1000,1.1398,0.0013,0.2092
...
```

## 🎯 Fitur Interaktif Plotly

- **Hover**: Lihat nilai eksak data
- **Click Legend**: Show/hide data tertentu
- **Pan**: Drag untuk geser chart
- **Zoom**: Scroll untuk zoom in/out
- **Download**: Icon kamera untuk save sebagai PNG

## 🌐 Deployment

Untuk deploy ke production:

```bash
# Build optimized production version
npm run build

# Files akan ada di folder 'dist/'
```

## 📝 Customization

### Mengubah Warna
Edit `colors` object di `src/main.js`:
```javascript
const colors = {
  HashTable: '#2196F3',    // Ubah kode hex
  BST: '#FF9800',
  LinkedList: '#4CAF50'
};
```

### Mengubah Layout/Styling
Edit `baseLayoutStyles` di `src/main.js` untuk mengubah font, background, dll.

## ⚠️ Troubleshooting

**Chart tidak muncul?**
- Pastikan `benchmark_results.csv` ada di folder `visualnya/`
- Buka console browser (F12) untuk melihat error
- Refresh halaman

**Data tidak update?**
- Clear browser cache (Ctrl+Shift+Del)
- Pastikan file CSV sudah di-save dengan benar

## 📚 Dependencies

- **Plotly.js**: Library visualisasi interaktif
- **PapaParse**: Parser CSV untuk JavaScript
- **Vite**: Build tool dan dev server

---

Dibuat dengan ❤️ untuk Proyek Akhir Struktur Data IPB
