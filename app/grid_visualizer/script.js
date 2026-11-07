// グリッド考察補助アプリ (Grid Visualizer) - script.js

class GridVisualizer {
  constructor() {
    this.h = 5;
    this.w = 5;

    // カラーサイクル (デフォルト色リスト)
    this.palette = [
      "#ffffff", // 1. 白 (道)
      "#1e293b", // 2. 黒 (壁)
      "#ef4444", // 3. 赤 (注目/訪問)
      "#3b82f6", // 4. 青
      "#10b981", // 5. 緑 (Start)
      "#eab308", // 6. 黄
      "#a855f7", // 7. 紫
      "#06b6d4", // 8. 水色
    ];
    this.cycleCount = 2; // デフォルト 2 色 (白 ⇄ 黒)

    this.currentMode = "cycle"; // 'cycle' (色トグル), 'bucket' (塗りつぶし), 'eraser' (消しゴム)
    this.readerMode = "string"; // 'string' (文字列モード) | 'number' (数字モード)
    this.isMouseDown = false;
    this.dragColor = null; // ドラッグ塗りで適用する色
    this.is1Indexed = false;
    this.selectedCell = null; // {r, c}
    this.history = [];
    this.historyIndex = -1;
    this.maxHistory = 50;

    // グリッドデータ: gridData[r][c] = { color: string, text: string }
    this.gridData = [];

    this.initDOM();
    this.initEvents();
    this.renderCyclePreview();
    this.resetGrid(this.h, this.w);

    // 保存されていたサイドバー状態の復元
    try {
      const savedCollapsed = localStorage.getItem("grid_visualizer_sidebar_collapsed") === "true";
      if (savedCollapsed) {
        this.setSidebarCollapsed(true);
      }
    } catch (err) {}
  }

  get activeCycle() {
    return this.palette.slice(0, this.cycleCount);
  }

  initDOM() {
    // サイドバー開閉関連
    this.appContainer = document.getElementById("app-container");
    this.btnHeaderToggleSidebar = document.getElementById("btn-header-toggle-sidebar");
    this.btnToggleSidebar = document.getElementById("btn-toggle-sidebar");
    this.btnCloseSidebar = document.getElementById("btn-close-sidebar");
    this.btnFloatingExpand = document.getElementById("btn-floating-expand");
    this.sidebarToggleIcon = document.getElementById("sidebar-toggle-icon");
    this.sidebarToggleText = document.getElementById("sidebar-toggle-text");

    // グリッドサイズ入力
    this.inputH = document.getElementById("input-h");
    this.inputW = document.getElementById("input-w");
    this.btnResize = document.getElementById("btn-resize");
    this.btnAddRow = document.getElementById("btn-add-row");
    this.btnDelRow = document.getElementById("btn-del-row");
    this.btnAddCol = document.getElementById("btn-add-col");
    this.btnDelCol = document.getElementById("btn-del-col");

    // グリッド表示
    this.gridTable = document.getElementById("grid-table");
    this.cellSizeSlider = document.getElementById("cell-size-slider");
    this.toggleIndex0 = document.getElementById("toggle-index-0");
    this.toggleIndex1 = document.getElementById("toggle-index-1");
    this.toastEl = document.getElementById("toast");

    // カラーサイクル関連
    this.cycleCountDisplay = document.getElementById("cycle-count-display");
    this.btnCycleMinus = document.getElementById("btn-cycle-minus");
    this.btnCyclePlus = document.getElementById("btn-cycle-plus");
    this.cyclePreviewList = document.getElementById("cycle-preview-list");

    // ヘッダーアクション
    this.btnOpenReader = document.getElementById("btn-open-reader");
    this.btnExportText = document.getElementById("btn-export-text");
    this.btnCopyCpp = document.getElementById("btn-copy-cpp");

    // 読み込みモーダル
    this.modalOverlay = document.getElementById("modal-overlay");
    this.modalCloseBtn = document.getElementById("modal-close-btn");
    this.tabModeString = document.getElementById("tab-mode-string");
    this.tabModeNumber = document.getElementById("tab-mode-number");
    this.modeDescText = document.getElementById("mode-desc-text");
    this.fileDropZone = document.getElementById("file-drop-zone");
    this.fileInput = document.getElementById("file-input");
    this.modalTextarea = document.getElementById("modal-textarea");
    this.btnModalCancel = document.getElementById("btn-modal-cancel");
    this.modalConfirmBtn = document.getElementById("modal-confirm-btn");
  }

  initEvents() {
    // リサイズ
    this.btnResize.addEventListener("click", () => {
      const h = parseInt(this.inputH.value, 10);
      const w = parseInt(this.inputW.value, 10);
      if (h > 0 && w > 0 && h <= 50 && w <= 50) {
        this.resetGrid(h, w);
      } else {
        alert("行数 H と列数 W は 1 〜 50 の範囲で指定してください。");
      }
    });

    // 行・列の追加/削除
    this.btnAddRow.addEventListener("click", () => this.addRow());
    this.btnDelRow.addEventListener("click", () => this.delRow());
    this.btnAddCol.addEventListener("click", () => this.addCol());
    this.btnDelCol.addEventListener("click", () => this.delCol());

    // 色数 (+ / -)
    this.btnCycleMinus.addEventListener("click", () => {
      if (this.cycleCount > 2) {
        this.cycleCount--;
        this.cycleCountDisplay.textContent = this.cycleCount;
        this.renderCyclePreview();
      }
    });

    this.btnCyclePlus.addEventListener("click", () => {
      if (this.cycleCount < this.palette.length) {
        this.cycleCount++;
        this.cycleCountDisplay.textContent = this.cycleCount;
        this.renderCyclePreview();
      }
    });

    // ツールモード切り替え
    document.querySelectorAll(".tool-btn").forEach((btn) => {
      btn.addEventListener("click", (e) => {
        document.querySelectorAll(".tool-btn").forEach((b) => b.classList.remove("active"));
        const targetBtn = e.currentTarget;
        targetBtn.classList.add("active");
        this.currentMode = targetBtn.dataset.mode;
      });
    });

    // クイックスタンプ
    document.querySelectorAll(".stamp-btn").forEach((btn) => {
      btn.addEventListener("click", () => {
        const text = btn.dataset.text;
        const color = btn.dataset.color;
        if (this.selectedCell) {
          const { r, c } = this.selectedCell;
          this.saveHistory();
          if (color) this.gridData[r][c].color = color;
          if (text !== undefined) this.gridData[r][c].text = text;
          this.updateCellDOM(r, c);
        } else {
          this.showToast("スタンプを押すマスをクリックして選択してください");
        }
      });
    });

    // セルサイズスライダー
    this.cellSizeSlider.addEventListener("input", (e) => {
      const size = e.target.value;
      document.documentElement.style.setProperty("--cell-size", `${size}px`);
    });

    // インデックス切り替え
    this.toggleIndex0.addEventListener("click", () => {
      this.is1Indexed = false;
      this.toggleIndex0.classList.add("active");
      this.toggleIndex1.classList.remove("active");
      this.renderHeaders();
    });
    this.toggleIndex1.addEventListener("click", () => {
      this.is1Indexed = true;
      this.toggleIndex1.classList.add("active");
      this.toggleIndex0.classList.remove("active");
      this.renderHeaders();
    });

    // クリア & リセット
    document.getElementById("btn-clear-colors").addEventListener("click", () => {
      this.saveHistory();
      for (let r = 0; r < this.h; r++) {
        for (let c = 0; c < this.w; c++) {
          this.gridData[r][c].color = "#ffffff";
        }
      }
      this.renderGrid();
      this.showToast("色をクリアしました");
    });

    document.getElementById("btn-clear-texts").addEventListener("click", () => {
      this.saveHistory();
      for (let r = 0; r < this.h; r++) {
        for (let c = 0; c < this.w; c++) {
          this.gridData[r][c].text = "";
        }
      }
      this.renderGrid();
      this.showToast("数字・テキストをクリアしました");
    });

    // Undo / Redo
    document.getElementById("btn-undo").addEventListener("click", () => this.undo());
    document.getElementById("btn-redo").addEventListener("click", () => this.redo());

    // サイドバー開閉
    if (this.btnHeaderToggleSidebar) {
      this.btnHeaderToggleSidebar.addEventListener("click", () => this.toggleSidebar());
    }
    if (this.btnToggleSidebar) {
      this.btnToggleSidebar.addEventListener("click", () => this.toggleSidebar());
    }
    if (this.btnCloseSidebar) {
      this.btnCloseSidebar.addEventListener("click", () => this.setSidebarCollapsed(true));
    }
    if (this.btnFloatingExpand) {
      this.btnFloatingExpand.addEventListener("click", () => this.setSidebarCollapsed(false));
    }

    // キーボードショートカット
    window.addEventListener("keydown", (e) => this.handleKeyDown(e));

    // マウスアップでドラッグ解除
    window.addEventListener("mouseup", () => {
      this.isMouseDown = false;
      this.dragColor = null;
    });

    // エクスポートボタン
    this.btnExportText.addEventListener("click", () => this.exportText());
    this.btnCopyCpp.addEventListener("click", () => this.exportCppArray());

    // モーダル関連イベント
    this.setupReaderModalEvents();
  }

  // テキスト / ファイル読み込みモーダルのイベント設定
  setupReaderModalEvents() {
    // 開く
    this.btnOpenReader.addEventListener("click", () => this.openReaderModal());

    // 閉じる
    this.modalCloseBtn.addEventListener("click", () => this.closeModal());
    this.btnModalCancel.addEventListener("click", () => this.closeModal());
    this.modalOverlay.addEventListener("click", (e) => {
      if (e.target === this.modalOverlay) this.closeModal();
    });

    // タブ切り替え: 文字列モード / 数字モード
    this.tabModeString.addEventListener("click", () => this.setReaderMode("string"));
    this.tabModeNumber.addEventListener("click", () => this.setReaderMode("number"));

    // 反映ボタン
    this.modalConfirmBtn.addEventListener("click", () => this.confirmModalImport());

    // ファイル選択 (クリック)
    this.fileDropZone.addEventListener("click", () => this.fileInput.click());
    this.fileInput.addEventListener("change", (e) => {
      const file = e.target.files && e.target.files[0];
      if (file) this.loadFile(file);
    });

    // ドラッグ＆ドロップ対応 (ドロップゾーン & テキストエリア)
    [this.fileDropZone, this.modalTextarea].forEach((dropArea) => {
      dropArea.addEventListener("dragover", (e) => {
        e.preventDefault();
        this.fileDropZone.classList.add("dragover");
      });
      dropArea.addEventListener("dragleave", (e) => {
        e.preventDefault();
        this.fileDropZone.classList.remove("dragover");
      });
      dropArea.addEventListener("drop", (e) => {
        e.preventDefault();
        this.fileDropZone.classList.remove("dragover");
        const file = e.dataTransfer && e.dataTransfer.files && e.dataTransfer.files[0];
        if (file) this.loadFile(file);
      });
    });
  }

  openReaderModal() {
    this.modalOverlay.classList.add("active");
    this.setReaderMode(this.readerMode);
    setTimeout(() => this.modalTextarea.focus(), 50);
  }

  closeModal() {
    this.modalOverlay.classList.remove("active");
    if (this.fileInput) this.fileInput.value = "";
  }

  setReaderMode(mode) {
    this.readerMode = mode;
    if (mode === "string") {
      this.tabModeString.classList.add("active");
      this.tabModeNumber.classList.remove("active");
      this.modeDescText.innerHTML = `
        <b>【文字列モード形式】</b><br>
        1行目に <code>H W</code>、続く $H$ 行に各行 $W$ 文字の文字列。<br>
        <b>文字が <code>#</code> のマスのみ黒色（壁）</b> で表示します。
      `;
      this.modalTextarea.placeholder = "例:\n5 7\n.......\n.###.#.\n...#.#.\n.#.###.\n.#.....";
    } else {
      this.tabModeNumber.classList.add("active");
      this.tabModeString.classList.remove("active");
      this.modeDescText.innerHTML = `
        <b>【数字モード形式】</b><br>
        1行目に <code>H W</code>、続く $H$ 行に空白区切りの $W$ 個の数値。<br>
        各マスに<b>数値を直接表示</b>します。
      `;
      this.modalTextarea.placeholder = "例:\n4 4\n1 2 3 4\n5 6 7 8\n9 10 11 12\n13 14 15 16";
    }
  }

  loadFile(file) {
    const reader = new FileReader();
    reader.onload = (e) => {
      this.modalTextarea.value = e.target.result;
      this.showToast(`ファイル「${file.name}」を読み込みました`);
    };
    reader.onerror = () => {
      alert("ファイルの読み込みに失敗しました。");
    };
    reader.readAsText(file);
  }

  confirmModalImport() {
    const text = this.modalTextarea.value.trim();
    if (!text) {
      alert("テキストを貼り付けるか、ファイルを読み込んでください。");
      return;
    }
    const success = this.importFromText(text, this.readerMode);
    if (success) {
      this.closeModal();
    }
  }

  // カラーサイクルのUIプレビューを描画 (白 -> 黒 -> 赤 ...)
  renderCyclePreview() {
    this.cyclePreviewList.innerHTML = "";
    const active = this.activeCycle;

    active.forEach((color, idx) => {
      const stepEl = document.createElement("div");
      stepEl.className = "cycle-step";

      const chipWrapper = document.createElement("div");
      chipWrapper.className = "cycle-chip-wrapper";

      const chip = document.createElement("div");
      chip.className = "cycle-chip";
      chip.style.backgroundColor = color;
      chip.style.color = this.isColorDark(color) ? "#ffffff" : "#000000";
      chip.textContent = idx + 1;

      // 各色のカスタマイズ用カラーピッカー
      const picker = document.createElement("input");
      picker.type = "color";
      picker.value = this.normalizeHex(color);
      picker.addEventListener("change", (e) => {
        this.palette[idx] = e.target.value;
        this.renderCyclePreview();
      });

      chip.appendChild(picker);
      chipWrapper.appendChild(chip);
      stepEl.appendChild(chipWrapper);

      // 矢印
      const arrow = document.createElement("span");
      arrow.className = "cycle-arrow";
      arrow.textContent = "→";
      stepEl.appendChild(arrow);

      this.cyclePreviewList.appendChild(stepEl);
    });

    // 最後のループマーク
    const loopSpan = document.createElement("span");
    loopSpan.style.fontSize = "0.75rem";
    loopSpan.style.color = "var(--text-sub)";
    loopSpan.textContent = "(ループ)";
    this.cyclePreviewList.appendChild(loopSpan);
  }

  normalizeHex(color) {
    if (!color) return "#ffffff";
    if (color.startsWith("#") && color.length === 7) return color;
    return color;
  }

  resetGrid(h, w) {
    this.h = h;
    this.w = w;
    this.inputH.value = h;
    this.inputW.value = w;
    this.gridData = [];
    for (let r = 0; r < h; r++) {
      const row = [];
      for (let c = 0; c < w; c++) {
        row.push({ color: "#ffffff", text: "" });
      }
      this.gridData.push(row);
    }
    this.selectedCell = null;
    this.history = [];
    this.historyIndex = -1;
    this.saveHistory();
    this.renderGrid();
  }

  renderGrid() {
    this.gridTable.innerHTML = "";

    // 1. 列ヘッダー
    const headerRow = document.createElement("tr");
    const cornerCell = document.createElement("th");
    cornerCell.className = "row-header";
    cornerCell.textContent = "";
    headerRow.appendChild(cornerCell);

    for (let c = 0; c < this.w; c++) {
      const th = document.createElement("th");
      th.className = "col-header";
      th.dataset.col = c;
      th.textContent = this.is1Indexed ? c + 1 : c;
      headerRow.appendChild(th);
    }
    this.gridTable.appendChild(headerRow);

    // 2. データ行
    for (let r = 0; r < this.h; r++) {
      const tr = document.createElement("tr");

      // 行ヘッダー
      const rowTh = document.createElement("th");
      rowTh.className = "row-header";
      rowTh.dataset.row = r;
      rowTh.textContent = this.is1Indexed ? r + 1 : r;
      tr.appendChild(rowTh);

      for (let c = 0; c < this.w; c++) {
        const td = document.createElement("td");
        td.className = "grid-cell";
        td.dataset.row = r;
        td.dataset.col = c;

        this.applyCellStyle(td, this.gridData[r][c]);

        // イベントリスナー
        td.addEventListener("mousedown", (e) => this.onCellMouseDown(r, c, e));
        td.addEventListener("mouseenter", () => this.onCellMouseEnter(r, c));
        td.addEventListener("contextmenu", (e) => {
          e.preventDefault();
          this.onCellRightClick(r, c);
        });

        tr.appendChild(td);
      }
      this.gridTable.appendChild(tr);
    }
  }

  renderHeaders() {
    document.querySelectorAll(".col-header").forEach((th) => {
      const c = parseInt(th.dataset.col, 10);
      if (!isNaN(c)) th.textContent = this.is1Indexed ? c + 1 : c;
    });
    document.querySelectorAll(".row-header").forEach((th) => {
      const r = parseInt(th.dataset.row, 10);
      if (!isNaN(r)) th.textContent = this.is1Indexed ? r + 1 : r;
    });
  }

  applyCellStyle(td, cellData) {
    td.style.backgroundColor = cellData.color;
    td.textContent = cellData.text;

    // 背景色に応じて文字色 (白 / 黒) を自動コントラスト調整
    const isDark = this.isColorDark(cellData.color);
    td.style.color = isDark ? "#ffffff" : "#000000";
  }

  updateCellDOM(r, c) {
    const td = this.gridTable.querySelector(`.grid-cell[data-row="${r}"][data-col="${c}"]`);
    if (td) {
      this.applyCellStyle(td, this.gridData[r][c]);
    }
  }

  // 指定したマスを次のサイクル色に変更する
  cycleCellColor(r, c, reverse = false) {
    const active = this.activeCycle;
    const curColor = this.gridData[r][c].color.toLowerCase();

    // 現在の色がカラーサイクル内の何番目か判定
    let idx = -1;
    for (let i = 0; i < active.length; i++) {
      if (this.colorEquals(curColor, active[i])) {
        idx = i;
        break;
      }
    }

    let nextIdx = 0;
    if (idx !== -1) {
      if (reverse) {
        nextIdx = (idx - 1 + active.length) % active.length;
      } else {
        nextIdx = (idx + 1) % active.length;
      }
    } else {
      nextIdx = 1 % active.length;
    }

    const nextColor = active[nextIdx];
    this.gridData[r][c].color = nextColor;
    this.updateCellDOM(r, c);
    return nextColor;
  }

  colorEquals(c1, c2) {
    if (!c1 || !c2) return false;
    return c1.toLowerCase().trim() === c2.toLowerCase().trim();
  }

  onCellMouseDown(r, c, e) {
    if (e.button === 2) return; // 右クリックは contextmenu で処理
    if (e.button !== 0) return;

    this.isMouseDown = true;
    this.selectCell(r, c);

    if (this.currentMode === "cycle") {
      this.saveHistory();
      // クリックで次の色にサイクル
      const newColor = this.cycleCellColor(r, c, false);
      // ドラッグ塗りのために切り替わった新しい色を記憶
      this.dragColor = newColor;
    } else if (this.currentMode === "eraser") {
      this.saveHistory();
      this.gridData[r][c].color = "#ffffff";
      this.gridData[r][c].text = "";
      this.updateCellDOM(r, c);
      this.dragColor = "#ffffff";
    } else if (this.currentMode === "bucket") {
      this.saveHistory();
      const active = this.activeCycle;
      // バケツ塗り: 現在のセルの次の色で塗りつぶす
      const curColor = this.gridData[r][c].color;
      let idx = -1;
      for (let i = 0; i < active.length; i++) {
        if (this.colorEquals(curColor, active[i])) {
          idx = i;
          break;
        }
      }
      const nextColor = active[(idx + 1) % active.length];
      this.floodFill(r, c, nextColor);
    }
  }

  onCellRightClick(r, c) {
    this.selectCell(r, c);
    if (this.currentMode === "cycle") {
      this.saveHistory();
      // 右クリックで前の色に逆回転
      this.cycleCellColor(r, c, true);
    }
  }

  onCellMouseEnter(r, c) {
    this.highlightHeaders(r, c);

    if (!this.isMouseDown) return;

    if (this.currentMode === "cycle" && this.dragColor) {
      if (!this.colorEquals(this.gridData[r][c].color, this.dragColor)) {
        this.gridData[r][c].color = this.dragColor;
        this.updateCellDOM(r, c);
      }
    } else if (this.currentMode === "eraser") {
      this.gridData[r][c].color = "#ffffff";
      this.gridData[r][c].text = "";
      this.updateCellDOM(r, c);
    }
  }

  selectCell(r, c) {
    document.querySelectorAll(".grid-cell.selected").forEach((el) => el.classList.remove("selected"));
    this.selectedCell = { r, c };
    const td = this.gridTable.querySelector(`.grid-cell[data-row="${r}"][data-col="${c}"]`);
    if (td) td.classList.add("selected");
  }

  highlightHeaders(r, c) {
    document.querySelectorAll(".col-header.highlight, .row-header.highlight").forEach((el) => {
      el.classList.remove("highlight");
    });
    const colTh = this.gridTable.querySelector(`.col-header[data-col="${c}"]`);
    const rowTh = this.gridTable.querySelector(`.row-header[data-row="${r}"]`);
    if (colTh) colTh.classList.add("highlight");
    if (rowTh) rowTh.classList.add("highlight");
  }

  floodFill(startR, startC, targetColor) {
    const originColor = this.gridData[startR][startC].color;
    if (this.colorEquals(originColor, targetColor)) return;

    const queue = [[startR, startC]];
    const visited = Array.from({ length: this.h }, () => Array(this.w).fill(false));
    visited[startR][startC] = true;

    const dr = [1, -1, 0, 0];
    const dc = [0, 0, 1, -1];

    while (queue.length > 0) {
      const [r, c] = queue.shift();
      this.gridData[r][c].color = targetColor;
      this.updateCellDOM(r, c);

      for (let i = 0; i < 4; i++) {
        const nr = r + dr[i];
        const nc = c + dc[i];
        if (nr >= 0 && nr < this.h && nc >= 0 && nc < this.w) {
          if (!visited[nr][nc] && this.colorEquals(this.gridData[nr][nc].color, originColor)) {
            visited[nr][nc] = true;
            queue.push([nr, nc]);
          }
        }
      }
    }
  }

  handleKeyDown(e) {
    // モーダルが開いている場合は無視
    if (this.modalOverlay.classList.contains("active")) return;

    // サイドバー開閉: Ctrl+B / Cmd+B
    if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "b") {
      e.preventDefault();
      this.toggleSidebar();
      return;
    }

    // Undo: Ctrl+Z / Cmd+Z
    if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "z") {
      e.preventDefault();
      if (e.shiftKey) {
        this.redo();
      } else {
        this.undo();
      }
      return;
    }

    // Redo: Ctrl+Y / Cmd+Y
    if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "y") {
      e.preventDefault();
      this.redo();
      return;
    }

    if (!this.selectedCell) return;
    const { r, c } = this.selectedCell;

    // 矢印キーでマス移動
    if (e.key === "ArrowUp") {
      e.preventDefault();
      if (r > 0) this.selectCell(r - 1, c);
      return;
    }
    if (e.key === "ArrowDown") {
      e.preventDefault();
      if (r < this.h - 1) this.selectCell(r + 1, c);
      return;
    }
    if (e.key === "ArrowLeft") {
      e.preventDefault();
      if (c > 0) this.selectCell(r, c - 1);
      return;
    }
    if (e.key === "ArrowRight") {
      e.preventDefault();
      if (c < this.w - 1) this.selectCell(r, c + 1);
      return;
    }

    // Tab / Enter で次マスへ移動
    if (e.key === "Enter" || e.key === "Tab") {
      e.preventDefault();
      if (c < this.w - 1) {
        this.selectCell(r, c + 1);
      } else if (r < this.h - 1) {
        this.selectCell(r + 1, 0);
      }
      return;
    }

    // Escape で選択解除
    if (e.key === "Escape") {
      document.querySelectorAll(".grid-cell.selected").forEach((el) => el.classList.remove("selected"));
      this.selectedCell = null;
      return;
    }

    // Backspace / Delete で文字消去
    if (e.key === "Backspace" || e.key === "Delete") {
      e.preventDefault();
      if (this.gridData[r][c].text !== "") {
        this.saveHistory();
        this.gridData[r][c].text = "";
        this.updateCellDOM(r, c);
      }
      return;
    }

    // スペースキー: 色を次の色にサイクル (キーボード派用)
    if (e.key === " ") {
      e.preventDefault();
      this.saveHistory();
      this.cycleCellColor(r, c, e.shiftKey);
      return;
    }

    // 数字・文字キー入力 (0-9, A-Z, -, + などの直接入力)
    if (e.key.length === 1 && !e.ctrlKey && !e.metaKey && !e.altKey) {
      e.preventDefault();
      this.saveHistory();

      let cur = this.gridData[r][c].text;
      if (cur.length >= 4) cur = "";
      this.gridData[r][c].text = cur + e.key;

      this.updateCellDOM(r, c);
    }
  }

  // 行・列の追加/削除
  addRow() {
    if (this.h >= 50) return;
    this.saveHistory();
    this.h++;
    this.inputH.value = this.h;
    const newRow = [];
    for (let c = 0; c < this.w; c++) {
      newRow.push({ color: "#ffffff", text: "" });
    }
    this.gridData.push(newRow);
    this.renderGrid();
  }

  delRow() {
    if (this.h <= 1) return;
    this.saveHistory();
    this.h--;
    this.inputH.value = this.h;
    this.gridData.pop();
    if (this.selectedCell && this.selectedCell.r >= this.h) this.selectedCell = null;
    this.renderGrid();
  }

  addCol() {
    if (this.w >= 50) return;
    this.saveHistory();
    this.w++;
    this.inputW.value = this.w;
    for (let r = 0; r < this.h; r++) {
      this.gridData[r].push({ color: "#ffffff", text: "" });
    }
    this.renderGrid();
  }

  delCol() {
    if (this.w <= 1) return;
    this.saveHistory();
    this.w--;
    this.inputW.value = this.w;
    for (let r = 0; r < this.h; r++) {
      this.gridData[r].pop();
    }
    if (this.selectedCell && this.selectedCell.c >= this.w) this.selectedCell = null;
    this.renderGrid();
  }

  // 履歴管理 (Undo / Redo)
  saveHistory() {
    this.history = this.history.slice(0, this.historyIndex + 1);

    const snapshot = {
      h: this.h,
      w: this.w,
      data: this.gridData.map((row) => row.map((cell) => ({ ...cell }))),
    };
    this.history.push(snapshot);
    if (this.history.length > this.maxHistory) this.history.shift();
    this.historyIndex = this.history.length - 1;
  }

  undo() {
    if (this.historyIndex > 0) {
      this.historyIndex--;
      this.applySnapshot(this.history[this.historyIndex]);
      this.showToast("元に戻しました (Undo)");
    }
  }

  redo() {
    if (this.historyIndex < this.history.length - 1) {
      this.historyIndex++;
      this.applySnapshot(this.history[this.historyIndex]);
      this.showToast("やり直しました (Redo)");
    }
  }

  applySnapshot(snapshot) {
    this.h = snapshot.h;
    this.w = snapshot.w;
    this.inputH.value = this.h;
    this.inputW.value = this.w;
    this.gridData = snapshot.data.map((row) => row.map((cell) => ({ ...cell })));
    this.renderGrid();
  }

  // テキスト形式エクスポート (AtCoder文字列盤面 / 数字盤面)
  exportText() {
    // セル内に複数文字または数値が入っているか判定
    const hasSpacedTokens = this.gridData.some((row) =>
      row.some((c) => c.text.length > 1 || (!isNaN(c.text) && c.text !== ""))
    );

    let lines = [`${this.h} ${this.w}`];
    for (let r = 0; r < this.h; r++) {
      let rowStr = [];
      for (let c = 0; c < this.w; c++) {
        const item = this.gridData[r][c];
        if (item.text !== "") {
          rowStr.push(item.text);
        } else if (this.colorEquals(item.color, "#1e293b") || this.colorEquals(item.color, "#000000")) {
          rowStr.push("#"); // 壁
        } else {
          rowStr.push(hasSpacedTokens ? "0" : "."); // 道
        }
      }
      lines.push(hasSpacedTokens ? rowStr.join(" ") : rowStr.join(""));
    }
    const result = lines.join("\n");
    navigator.clipboard.writeText(result).then(() => {
      this.showToast("クリップボードに盤面テキスト (H W 付き) をコピーしました！");
    });
  }

  // C++ コード生成エクスポート
  exportCppArray() {
    const hasSpacedTokens = this.gridData.some((row) =>
      row.some((c) => c.text.length > 1 || (!isNaN(c.text) && c.text !== ""))
    );

    let lines = [];
    if (hasSpacedTokens) {
      lines.push(`vector<vector<int>> a = {`);
      for (let r = 0; r < this.h; r++) {
        let nums = [];
        for (let c = 0; c < this.w; c++) {
          const item = this.gridData[r][c];
          const val = parseInt(item.text, 10);
          nums.push(isNaN(val) ? 0 : val);
        }
        lines.push(`    {${nums.join(", ")}}${r + 1 < this.h ? "," : ""}`);
      }
      lines.push(`};`);
    } else {
      lines.push(`vector<string> s = {`);
      for (let r = 0; r < this.h; r++) {
        let rowChars = "";
        for (let c = 0; c < this.w; c++) {
          const item = this.gridData[r][c];
          if (item.text !== "") {
            rowChars += item.text[0];
          } else if (this.colorEquals(item.color, "#1e293b") || this.colorEquals(item.color, "#000000")) {
            rowChars += "#";
          } else {
            rowChars += ".";
          }
        }
        lines.push(`    "${rowChars}"${r + 1 < this.h ? "," : ""}`);
      }
      lines.push(`};`);
    }
    const result = lines.join("\n");
    navigator.clipboard.writeText(result).then(() => {
      this.showToast("C++ コードをクリップボードにコピーしました！");
    });
  }

  // テキストからのインポート処理 (文字列モード / 数字モード)
  importFromText(rawText, mode = "string") {
    const lines = rawText
      .split(/\r?\n/)
      .map((l) => l.trim())
      .filter((l) => l.length > 0);
    if (lines.length === 0) return false;

    // 1行目が "H W" か判定
    const headerMatch = lines[0].match(/^(\d+)\s+(\d+)$/);
    let hasHeader = false;
    let targetH = 0;
    let targetW = 0;
    let dataLines = lines;

    if (headerMatch && lines.length > 1) {
      const hCandidate = parseInt(headerMatch[1], 10);
      const wCandidate = parseInt(headerMatch[2], 10);

      if (mode === "string") {
        // 文字列モードでは1行目が数値2つなら H W
        hasHeader = true;
        targetH = hCandidate;
        targetW = wCandidate;
        dataLines = lines.slice(1);
      } else {
        // 数字モード: データ行数が hCandidate 以上、または次の行の要素数が wCandidate と一致する場合
        const tokensLine1 = lines[1].split(/\s+/);
        if (lines.length - 1 >= hCandidate || tokensLine1.length === wCandidate) {
          hasHeader = true;
          targetH = hCandidate;
          targetW = wCandidate;
          dataLines = lines.slice(1);
        }
      }
    }

    let newH = 0;
    let newW = 0;
    let newGrid = [];

    if (mode === "string") {
      // ===== 文字列モード =====
      // H W の決定
      if (hasHeader && targetH > 0) {
        newH = Math.min(targetH, dataLines.length);
      } else {
        newH = dataLines.length;
      }

      if (hasHeader && targetW > 0) {
        newW = targetW;
      } else {
        newW = Math.max(...dataLines.map((l) => l.length));
      }

      // 範囲を 1 〜 50 にクリップ
      newH = Math.max(1, Math.min(newH, 50));
      newW = Math.max(1, Math.min(newW, 50));

      for (let r = 0; r < newH; r++) {
        const row = [];
        const str = dataLines[r] || "";
        for (let c = 0; c < newW; c++) {
          const char = c < str.length ? str[c] : ".";
          if (char === "#") {
            // 文字が # の時のみ黒で表示 (壁)
            row.push({ color: "#1e293b", text: "" });
          } else if (char === ".") {
            // 道 (.) は白セル
            row.push({ color: "#ffffff", text: "" });
          } else {
            // その他 (S, G, 数字など)
            row.push({ color: "#ffffff", text: char });
          }
        }
        newGrid.push(row);
      }
    } else {
      // ===== 数字モード =====
      const tokenized = dataLines.map((l) => l.split(/\s+/));

      if (hasHeader && targetH > 0) {
        newH = Math.min(targetH, tokenized.length);
      } else {
        newH = tokenized.length;
      }

      if (hasHeader && targetW > 0) {
        newW = targetW;
      } else {
        newW = Math.max(...tokenized.map((row) => row.length));
      }

      newH = Math.max(1, Math.min(newH, 50));
      newW = Math.max(1, Math.min(newW, 50));

      for (let r = 0; r < newH; r++) {
        const row = [];
        const tokens = tokenized[r] || [];
        for (let c = 0; c < newW; c++) {
          const val = c < tokens.length ? tokens[c] : "";
          row.push({ color: "#ffffff", text: val });
        }
        newGrid.push(row);
      }
    }

    this.saveHistory();
    this.h = newH;
    this.w = newW;
    this.gridData = newGrid;
    this.inputH.value = this.h;
    this.inputW.value = this.w;
    this.selectedCell = null;
    this.renderGrid();

    const modeName = mode === "string" ? "文字列モード (#=黒)" : "数字モード";
    this.showToast(`H=${this.h}, W=${this.w} の盤面を【${modeName}】で読み込みました！`);
    return true;
  }

  toggleSidebar() {
    if (!this.appContainer) return;
    const isCollapsed = this.appContainer.classList.contains("sidebar-collapsed");
    this.setSidebarCollapsed(!isCollapsed);
  }

  setSidebarCollapsed(collapsed) {
    if (!this.appContainer) return;
    if (collapsed) {
      this.appContainer.classList.add("sidebar-collapsed");
      if (this.sidebarToggleIcon) this.sidebarToggleIcon.textContent = "▶";
      if (this.sidebarToggleText) this.sidebarToggleText.textContent = "パネルを表示";
      if (this.btnToggleSidebar) this.btnToggleSidebar.classList.add("active");
      if (this.btnHeaderToggleSidebar) this.btnHeaderToggleSidebar.classList.add("active");
    } else {
      this.appContainer.classList.remove("sidebar-collapsed");
      if (this.sidebarToggleIcon) this.sidebarToggleIcon.textContent = "◀";
      if (this.sidebarToggleText) this.sidebarToggleText.textContent = "パネルを隠す";
      if (this.btnToggleSidebar) this.btnToggleSidebar.classList.remove("active");
      if (this.btnHeaderToggleSidebar) this.btnHeaderToggleSidebar.classList.remove("active");
    }
    try {
      localStorage.setItem("grid_visualizer_sidebar_collapsed", collapsed ? "true" : "false");
    } catch (err) {}
  }

  showToast(message) {
    this.toastEl.textContent = message;
    this.toastEl.classList.add("show");
    setTimeout(() => {
      this.toastEl.classList.remove("show");
    }, 2500);
  }

  // 明るさ判定 (YIQ / 輝度計算)
  isColorDark(hexColor) {
    if (!hexColor || hexColor === "#ffffff") return false;
    let c = hexColor.replace("#", "");
    if (c.length === 3) c = c.split("").map((x) => x + x).join("");
    const r = parseInt(c.substr(0, 2), 16);
    const g = parseInt(c.substr(2, 2), 16);
    const b = parseInt(c.substr(4, 2), 16);
    const yiq = (r * 299 + g * 587 + b * 114) / 1000;
    return yiq < 140;
  }
}

// 起動
window.addEventListener("DOMContentLoaded", () => {
  new GridVisualizer();
});
