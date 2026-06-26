import Plotly from "plotly.js-dist-min";
import Papa from "papaparse";

// -- Warna & Label per Struktur Data --
const colors = {
  LinkedList: '#2563eb',   // Blue
  'AVL Tree': '#d97706',   // Amber
  HashTable:  '#059669',   // Emerald
};

const dsLabels = {
  LinkedList: 'Linked List',
  'AVL Tree': 'AVL Tree',
  HashTable:  'Hash Table',
};

const bigOLabels = {
  INSERT: {
    LinkedList: 'Linked List - O(1)',
    'AVL Tree': 'AVL Tree - O(log n)',
    HashTable:  'Hash Table - O(1) avg',
  },
  SEARCH: {
    LinkedList: 'Linked List - O(n)',
    'AVL Tree': 'AVL Tree - O(n)',
    HashTable:  'Hash Table - O(1) avg',
  },
  RANGE: {
    LinkedList: 'Linked List - O(n)',
    'AVL Tree': 'AVL Tree - O(log n + k)',
    HashTable:  'Hash Table - O(n)',
  },
  DELETE: {
    LinkedList: 'Linked List - O(n)',
    'AVL Tree': 'AVL Tree - O(n log n)',
    HashTable:  'Hash Table - O(n)',
  },
};

const DS_LIST = ['LinkedList', 'AVL Tree', 'HashTable'];

// -- Config Plotly --
const plotConfig = {
  responsive: true,
  displayModeBar: true,
  toImageButtonOptions: {
    format: 'png',
    filename: 'benchmark_chart',
    height: 700,
    width: 1200,
    scale: 2,
  },
};

const baseLayout = {
  font:         { family: "Inter, Arial, sans-serif", size: 12, color: '#0f172a' },
  paper_bgcolor: '#ffffff',
  plot_bgcolor:  '#f8fafc',
  hovermode:    'x unified',
  showlegend:   true,
  legend: {
    orientation: 'h',
    y: -0.22,
    x: 0.5,
    xanchor: 'center',
    font: { size: 11 },
    bgcolor: 'rgba(255,255,255,0.9)',
    bordercolor: '#e2e8f0',
    borderwidth: 1,
  },
  margin: { t: 40, b: 80, l: 70, r: 20 },
  xaxis: {
    title: { text: 'Jumlah Data (N)', font: { size: 12 } },
    gridcolor: '#e2e8f0',
    linecolor: '#cbd5e1',
    tickfont: { size: 11 },
  },
  yaxis: {
    gridcolor: '#e2e8f0',
    linecolor: '#cbd5e1',
    tickfont: { size: 11 },
    rangemode: 'tozero',
  },
};

// -- Fetch & Parse CSV --
fetch("/benchmark_results.csv")
  .then(r => {
    if (!r.ok) throw new Error(`HTTP ${r.status}`);
    return r.text();
  })
  .then(csvText => {
    const parsed = Papa.parse(csvText, {
      header: true,
      skipEmptyLines: true,
      dynamicTyping: true,
    });

    const rows = parsed.data;
    if (!rows.length) {
      showError("Data CSV kosong. Pastikan benchmark sudah dijalankan dari program C++.");
      return;
    }

    // Cek kolom yang tersedia
    const headers = parsed.meta.fields || [];
    console.log("Kolom CSV:", headers);
    console.log("Data rows:", rows.length);

    // Deteksi nama kolom (flexible)
    const colInsert = headers.find(h => h.includes('Insert') && !h.includes('Mem')) || '';
    const colExact  = headers.find(h => h.includes('Exact')) || headers.find(h => h.includes('Search') && !h.includes('Range')) || '';
    const colRange  = headers.find(h => h.includes('Range')) || '';
    const colDelete = headers.find(h => h.includes('Delete')) || '';
    const colMem    = headers.find(h => h.includes('Mem') || h.includes('Memory')) || '';

    console.log("Kolom mapped:", { colInsert, colExact, colRange, colDelete, colMem });

    // Struktur data penyimpanan hasil
    const data = {
      INSERT: {}, SEARCH: {}, RANGE: {}, DELETE: {}, MEMORY: {}
    };
    const nSet = new Set();

    rows.forEach(row => {
      const n  = Number(row.n);
      const ds = row.Struktur;
      if (!n || !ds) return;
      nSet.add(n);

      if (!data.INSERT[ds]) {
        data.INSERT[ds] = {};
        data.SEARCH[ds] = {};
        data.RANGE[ds]  = {};
        data.DELETE[ds] = {};
        data.MEMORY[ds] = {};
      }

      data.INSERT[ds][n] = colInsert ? Number(row[colInsert]) : null;
      data.SEARCH[ds][n] = colExact  ? Number(row[colExact])  : null;
      data.RANGE[ds][n]  = colRange  ? Number(row[colRange])  : null;
      data.DELETE[ds][n] = colDelete ? Number(row[colDelete]) : null;
      data.MEMORY[ds][n] = colMem    ? Number(row[colMem])    : null;
    });

    const nList = Array.from(nSet).sort((a, b) => a - b);
    console.log("N values:", nList);
    console.log("Structures found:", Object.keys(data.INSERT));

    // Render semua grafik
    renderBar(data.INSERT, nList, 'INSERT', 'Waktu Insert (ms)', 'grafik_insert');
    renderBar(data.SEARCH, nList, 'SEARCH', 'Waktu Search Level (ms)', 'grafik_search');
    renderBar(data.RANGE,  nList, 'RANGE',  'Waktu Range Search (ms)', 'grafik_range');
    renderBar(data.DELETE, nList, 'DELETE', 'Waktu Delete (ms)', 'grafik_delete');

    if (colMem) {
      renderMemoryBar(data.MEMORY, nList, 'grafik_memory');
    } else {
      // Tampilkan grafik memori estimasi berdasarkan rumus
      renderMemoryEstimate(nList, 'grafik_memory');
    }

    renderScalability(data, nList, 'grafik_semua_operasi');
  })
  .catch(err => {
    console.error("Gagal memuat CSV:", err);
    showError(`Gagal memuat data: ${err.message}. Pastikan file benchmark_results.csv ada di folder visualnya/.`);
  });

// -- Helper: format N sebagai label ringkas (1000 -> "1k", 100000 -> "100k") --
function nLabel(n) {
  if (n >= 1000) return (n / 1000) + 'k';
  return String(n);
}

// -- Render Grouped Bar Chart --
function renderBar(opData, nList, op, yLabel, elemId) {
  const xLabels = nList.map(n => nLabel(n));
  const structures = DS_LIST.filter(ds => opData[ds]);

  const traces = structures.map(ds => {
    const yVals = nList.map(n => {
      const v = opData[ds] ? opData[ds][n] : null;
      return (v != null && !isNaN(v) && v >= 0) ? v : 0;
    });

    return {
      x: xLabels,
      y: yVals,
      name: bigOLabels[op] ? bigOLabels[op][ds] : dsLabels[ds] || ds,
      type: 'bar',
      marker: {
        color: colors[ds] || '#999',
        opacity: 0.85,
        line: { color: colors[ds] || '#999', width: 1.5 },
      },
      text: yVals.map(v => v > 0 ? v.toFixed(3) : ''),
      textposition: 'outside',
      textfont: { size: 9, color: '#475569' },
      hovertemplate: `<b>${dsLabels[ds] || ds}</b><br>N: %{x}<br>${yLabel}: <b>%{y:.4f} ms</b><extra></extra>`,
    };
  });

  const layout = {
    ...baseLayout,
    yaxis: {
      ...baseLayout.yaxis,
      title: { text: yLabel, font: { size: 12 } },
    },
    barmode: 'group',
    bargap: 0.2,
    bargroupgap: 0.08,
  };

  Plotly.newPlot(elemId, traces, layout, plotConfig);
}

// -- Render Memory Bar Chart (dari data CSV) --
function renderMemoryBar(memData, nList, elemId) {
  const xLabels = nList.map(n => nLabel(n));
  const structures = DS_LIST.filter(ds => memData[ds]);

  const traces = structures.map(ds => {
    const yVals = nList.map(n => {
      const v = memData[ds] ? memData[ds][n] : null;
      return (v != null && !isNaN(v) && v > 0) ? v : 0;
    });

    return {
      x: xLabels,
      y: yVals,
      name: dsLabels[ds] || ds,
      type: 'bar',
      marker: {
        color: colors[ds] || '#999',
        opacity: 0.85,
        line: { color: colors[ds] || '#999', width: 1.5 },
        pattern: { shape: '/' },
      },
      text: yVals.map(v => v > 0 ? v.toFixed(0) + ' KB' : ''),
      textposition: 'outside',
      textfont: { size: 9, color: '#475569' },
      hovertemplate: `<b>${dsLabels[ds] || ds}</b><br>N: %{x}<br>Memori: <b>%{y:.1f} KB</b><extra></extra>`,
    };
  });

  const layout = {
    ...baseLayout,
    yaxis: {
      ...baseLayout.yaxis,
      title: { text: 'Estimasi Memori (KB)', font: { size: 12 } },
    },
    barmode: 'group',
    bargap: 0.2,
    bargroupgap: 0.08,
  };

  Plotly.newPlot(elemId, traces, layout, plotConfig);
}

// -- Render Memory Estimasi (jika tidak ada kolom memori di CSV) --
function renderMemoryEstimate(nList, elemId) {
  // Rumus estimasi bytes per struktur:
  // LL: n * (sizeof(LLNode) + 200) = n * ~208 bytes
  // AVL: n * (sizeof(AVLNode) + 200) = n * ~240 bytes
  // HT: n * (sizeof(LogEntry) + 200) + buckets*64 = n * ~264 bytes
  const bytesPerNode = { LinkedList: 208, 'AVL Tree': 240, HashTable: 264 };

  const xLabels = nList.map(n => nLabel(n));

  const traces = DS_LIST.map(ds => {
    const bpn = bytesPerNode[ds] || 200;
    const yVals = nList.map(n => (n * bpn) / 1024); // convert to KB

    return {
      x: xLabels,
      y: yVals,
      name: dsLabels[ds] || ds,
      type: 'bar',
      marker: {
        color: colors[ds] || '#999',
        opacity: 0.8,
        line: { color: colors[ds] || '#999', width: 1.5 },
      },
      text: yVals.map(v => v.toFixed(0) + ' KB'),
      textposition: 'outside',
      textfont: { size: 9, color: '#475569' },
      hovertemplate: `<b>${dsLabels[ds] || ds}</b><br>N: %{x}<br>Estimasi: <b>%{y:.1f} KB</b><extra></extra>`,
    };
  });

  const layout = {
    ...baseLayout,
    yaxis: {
      ...baseLayout.yaxis,
      title: { text: 'Estimasi Memori (KB)', font: { size: 12 } },
    },
    barmode: 'group',
    bargap: 0.2,
    bargroupgap: 0.08,
    annotations: [{
      x: 0.5, y: 1.05,
      xref: 'paper', yref: 'paper',
      text: 'Estimasi teoritis berdasarkan sizeof struktur (jalankan benchmark untuk data aktual)',
      showarrow: false,
      font: { size: 10, color: '#94a3b8' },
    }],
  };

  Plotly.newPlot(elemId, traces, layout, plotConfig);
}

// -- Render Line Chart Skalabilitas --
function renderScalability(data, nList, elemId) {
  const ops = [
    { key: 'INSERT', label: 'Insert', dash: 'solid' },
    { key: 'SEARCH', label: 'Exact Search', dash: 'dot' },
    { key: 'RANGE',  label: 'Range Search', dash: 'dash' },
    { key: 'DELETE', label: 'Delete', dash: 'dashdot' },
  ];
  const symbols = { LinkedList: 'circle', 'AVL Tree': 'square', HashTable: 'diamond' };
  const traces = [];

  ops.forEach(op => {
    if (!data[op.key]) return;
    DS_LIST.forEach(ds => {
      const opData = data[op.key][ds];
      if (!opData) return;

      const points = nList
        .map(n => ({ n, v: opData[n] }))
        .filter(pt => pt.v != null && !isNaN(pt.v) && pt.v >= 0);

      if (!points.length) return;

      traces.push({
        x: points.map(pt => pt.n),
        y: points.map(pt => pt.v),
        name: `${op.label} - ${dsLabels[ds] || ds}`,
        legendgroup: ds,
        type: 'scatter',
        mode: 'lines+markers',
        line: { color: colors[ds] || '#999', width: 2.5, dash: op.dash },
        marker: {
          symbol: symbols[ds] || 'circle',
          size: 7,
          color: colors[ds] || '#999',
          line: { color: '#fff', width: 1.5 },
        },
        hovertemplate: `<b>${op.label} (${dsLabels[ds] || ds})</b><br>N: %{x:,}<br>Waktu: <b>%{y:.4f} ms</b><extra></extra>`,
      });
    });
  });

  const layout = {
    ...baseLayout,
    hovermode: 'closest',
    legend: {
      ...baseLayout.legend,
      y: -0.3,
      font: { size: 10 },
    },
    margin: { ...baseLayout.margin, b: 120, t: 60 },
    xaxis: {
      ...baseLayout.xaxis,
      title: { text: 'Jumlah Data (N)', font: { size: 12 } },
    },
    yaxis: {
      ...baseLayout.yaxis,
      title: { text: 'Waktu Eksekusi (ms)', font: { size: 12 } },
    },
  };

  Plotly.newPlot(elemId, traces, layout, plotConfig);
}

// -- Error Display --
function showError(msg) {
  const ids = ['grafik_insert','grafik_search','grafik_range','grafik_delete','grafik_memory','grafik_semua_operasi'];
  ids.forEach(id => {
    const el = document.getElementById(id);
    if (el) {
      el.innerHTML = `
        <div style="display:flex;align-items:center;justify-content:center;height:300px;flex-direction:column;gap:12px;color:#dc2626;">
          <div style="font-size:1.5rem;">[!]</div>
          <div style="font-size:0.9rem;text-align:center;max-width:400px;line-height:1.6;">${msg}</div>
        </div>`;
    }
  });
}
