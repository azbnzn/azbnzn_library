// Array Graph - script.js
// 競プロ用 配列・数列グラフプロッター

class ArrayGraphVisualizer {
  constructor() {
    this.data = []; // 配列要素 [A_0, A_1, ..., A_{N-1}]
    this.n = 0;

    // 表示設定
    this.is1Indexed = true; // デフォルト 1-indexed (i=1..N)
    this.graphType = "scatter"; // デフォルト: 'scatter' (点) | 'line' | 'bar' | 'step'
    this.yAxisMode = "auto_zero"; // 'auto_zero' | 'always_zero' | 'tight'
    this.aspectRatioMode = "1:1"; // デフォルト: '1:1' (等倍縮尺) | 'auto' (自動フィット) | 'custom'
    this.customYScale = 1.0; // カスタム時のY軸倍率
    this.showLabels = true;
    this.showGrid = true;
    this.showPrefix = false;
    this.showZero = true;

    // ビューポート (データ座標空間の表示範囲)
    this.view = {
      xMin: 0,
      xMax: 10,
      yMin: 0,
      yMax: 10,
    };

    // マウス操作状態
    this.isDragging = false;
    this.dragStart = { x: 0, y: 0 };
    this.viewStart = { xMin: 0, xMax: 0, yMin: 0, yMax: 0 };
    this.hoverPoint = null; // { index, x, y, screenX, screenY }

    this.initDOM();
    this.initEvents();
    this.resizeCanvas();
    this.emptyState.style.display = "flex";

    // 保存されていたサイドバー状態の復元
    try {
      const savedCollapsed = localStorage.getItem("array_graph_sidebar_collapsed") === "true";
      if (savedCollapsed) {
        this.setSidebarCollapsed(true, false);
      }
    } catch (err) {}
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

    this.canvas = document.getElementById("plot-canvas");
    this.ctx = this.canvas.getContext("2d");
    this.container = document.getElementById("canvas-container");
    this.tooltip = document.getElementById("plot-tooltip");
    this.emptyState = document.getElementById("empty-state");
    this.toastEl = document.getElementById("toast");
    this.hoverCoordText = document.getElementById("hover-coord-text");

    // 入力関連
    this.inputArea = document.getElementById("input-array-text");
    this.btnPlot = document.getElementById("btn-plot");
    this.btnClear = document.getElementById("btn-clear");
    this.fileDropZone = document.getElementById("file-drop-zone");
    this.fileInput = document.getElementById("file-input");

    // 設定
    this.selectYMode = document.getElementById("select-y-mode");
    this.aspectRatioVal = document.getElementById("aspect-ratio-val");
    this.aspectSliderContainer = document.getElementById("aspect-slider-container");
    this.aspectRatioSlider = document.getElementById("aspect-ratio-slider");
    this.sliderRatioDisplay = document.getElementById("slider-ratio-display");
    this.btnAspect05 = document.getElementById("btn-aspect-05");
    this.btnAspect10 = document.getElementById("btn-aspect-10");
    this.btnAspect20 = document.getElementById("btn-aspect-20");
    this.chkShowLabels = document.getElementById("chk-show-labels");
    this.chkShowGrid = document.getElementById("chk-show-grid");
    this.chkShowPrefix = document.getElementById("chk-show-prefix");
    this.chkShowZero = document.getElementById("chk-show-zero");

    // 統計要素
    this.statN = document.getElementById("stat-n");
    this.statMin = document.getElementById("stat-min");
    this.statMax = document.getElementById("stat-max");
    this.statSum = document.getElementById("stat-sum");
    this.statAvg = document.getElementById("stat-avg");

    // ズーム・エクスポート
    this.btnZoomIn = document.getElementById("btn-zoom-in");
    this.btnZoomOut = document.getElementById("btn-zoom-out");
    this.btnResetView = document.getElementById("btn-reset-view");
    this.btnExportText = document.getElementById("btn-export-text");
    this.btnExportCpp = document.getElementById("btn-export-cpp");
    this.btnSaveImage = document.getElementById("btn-save-image");
  }

  initEvents() {
    // サイドバー開閉イベント
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

    // キーボードショートカット: Ctrl+B / Cmd+B でサイドバー開閉
    window.addEventListener("keydown", (e) => {
      if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "b") {
        e.preventDefault();
        this.toggleSidebar();
      }
    });

    window.addEventListener("resize", () => {
      this.resizeCanvas();
      if (this.aspectRatioMode !== "auto") {
        this.resetView();
      }
      this.render();
    });

    // プロットボタン
    this.btnPlot.addEventListener("click", () => this.plotFromInput());

    // クリアボタン
    this.btnClear.addEventListener("click", () => {
      this.inputArea.value = "";
      this.data = [];
      this.n = 0;
      this.updateStats();
      this.emptyState.style.display = "flex";
      this.clearCanvas();
    });

    // グラフ種別トグル
    document.querySelectorAll("#graph-type-group .toggle-btn").forEach((btn) => {
      btn.addEventListener("click", (e) => {
        document.querySelectorAll("#graph-type-group .toggle-btn").forEach((b) => b.classList.remove("active"));
        const target = e.currentTarget;
        target.classList.add("active");
        this.graphType = target.dataset.type;
        this.render();
      });
    });

    // X軸インデックストグル (1-indexed / 0-indexed)
    document.querySelectorAll("#index-type-group .toggle-btn").forEach((btn) => {
      btn.addEventListener("click", (e) => {
        document.querySelectorAll("#index-type-group .toggle-btn").forEach((b) => b.classList.remove("active"));
        const target = e.currentTarget;
        target.classList.add("active");
        this.is1Indexed = target.dataset.index === "1";
        this.resetView();
        this.render();
      });
    });

    // Y軸設定変更
    this.selectYMode.addEventListener("change", (e) => {
      this.yAxisMode = e.target.value;
      this.resetView();
      this.render();
    });

    // アスペクト比 (縦横比) トグル
    document.querySelectorAll("#aspect-ratio-group .toggle-btn").forEach((btn) => {
      btn.addEventListener("click", (e) => {
        const mode = e.currentTarget.dataset.aspect;
        this.setAspectRatioMode(mode);
      });
    });

    // カスタム比率スライダー
    this.aspectRatioSlider.addEventListener("input", (e) => {
      this.customYScale = parseFloat(e.target.value);
      this.sliderRatioDisplay.textContent = `${this.customYScale.toFixed(2)}x`;
      this.aspectRatioVal.textContent = `1 : ${this.customYScale.toFixed(2)}`;
      this.resetView();
      this.render();
    });

    // プリセット倍率ボタン
    this.btnAspect05.addEventListener("click", () => {
      this.aspectRatioSlider.value = 0.5;
      this.aspectRatioSlider.dispatchEvent(new Event("input"));
    });
    this.btnAspect10.addEventListener("click", () => {
      this.aspectRatioSlider.value = 1.0;
      this.aspectRatioSlider.dispatchEvent(new Event("input"));
    });
    this.btnAspect20.addEventListener("click", () => {
      this.aspectRatioSlider.value = 2.0;
      this.aspectRatioSlider.dispatchEvent(new Event("input"));
    });

    // 表示オプションチェックボックス
    this.chkShowLabels.addEventListener("change", (e) => {
      this.showLabels = e.target.checked;
      this.render();
    });
    this.chkShowGrid.addEventListener("change", (e) => {
      this.showGrid = e.target.checked;
      this.render();
    });
    this.chkShowPrefix.addEventListener("change", (e) => {
      this.showPrefix = e.target.checked;
      this.render();
    });
    this.chkShowZero.addEventListener("change", (e) => {
      this.showZero = e.target.checked;
      this.render();
    });

    // ファイル入力
    this.fileDropZone.addEventListener("click", () => this.fileInput.click());
    this.fileInput.addEventListener("change", (e) => {
      const file = e.target.files && e.target.files[0];
      if (file) this.loadFile(file);
    });

    // ドラッグ＆ドロップ対応
    [this.fileDropZone, this.inputArea].forEach((dropArea) => {
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

    // ズーム＆パン操作 (キャンバス上)
    this.canvas.addEventListener("mousedown", (e) => this.onMouseDown(e));
    window.addEventListener("mousemove", (e) => this.onMouseMove(e));
    window.addEventListener("mouseup", () => this.onMouseUp());
    this.canvas.addEventListener("wheel", (e) => this.onWheel(e), { passive: false });

    // ツールバーボタン
    this.btnZoomIn.addEventListener("click", () => this.zoom(0.88));
    this.btnZoomOut.addEventListener("click", () => this.zoom(1.14));
    this.btnResetView.addEventListener("click", () => {
      this.resetView();
      this.render();
    });

    // エクスポートボタン
    this.btnExportText.addEventListener("click", () => this.exportText());
    this.btnExportCpp.addEventListener("click", () => this.exportCpp());
    this.btnSaveImage.addEventListener("click", () => this.saveImage());
  }

  resizeCanvas() {
    const rect = this.container.getBoundingClientRect();
    const dpr = window.devicePixelRatio || 1;
    this.width = rect.width;
    this.height = rect.height;
    this.canvas.width = rect.width * dpr;
    this.canvas.height = rect.height * dpr;
    this.ctx.setTransform(1, 0, 0, 1, 0, 0);
    this.ctx.scale(dpr, dpr);
  }

  setAspectRatioMode(mode) {
    this.aspectRatioMode = mode;
    document.querySelectorAll("#aspect-ratio-group .toggle-btn").forEach((btn) => {
      btn.classList.toggle("active", btn.dataset.aspect === mode);
    });

    if (mode === "1:1") {
      this.aspectSliderContainer.style.display = "none";
      this.aspectRatioVal.textContent = "1:1 (等倍)";
    } else if (mode === "auto") {
      this.aspectSliderContainer.style.display = "none";
      this.aspectRatioVal.textContent = "自動フィット";
    } else if (mode === "custom") {
      this.aspectSliderContainer.style.display = "flex";
      this.customYScale = parseFloat(this.aspectRatioSlider.value);
      this.sliderRatioDisplay.textContent = `${this.customYScale.toFixed(2)}x`;
      this.aspectRatioVal.textContent = `1 : ${this.customYScale.toFixed(2)}`;
    }

    this.resetView();
    this.render();
  }

  loadFile(file) {
    const reader = new FileReader();
    reader.onload = (e) => {
      this.inputArea.value = e.target.result;
      this.plotFromInput();
      this.showToast(`ファイル「${file.name}」を読み込んでプロットしました`);
    };
    reader.onerror = () => {
      alert("ファイルの読み込みに失敗しました。");
    };
    reader.readAsText(file);
  }

  // 入力テキストからパースしてプロット
  plotFromInput() {
    const text = this.inputArea.value.trim();
    if (!text) {
      alert("配列データを入力してください。");
      return;
    }

    const parsed = this.parseArrayInput(text);
    if (!parsed || parsed.length === 0) {
      alert("数値を読み取れませんでした。形式を確認してください。");
      return;
    }

    this.data = parsed;
    this.n = parsed.length;

    this.updateStats();
    this.emptyState.style.display = "none";
    this.resetView();
    this.render();

    const isAllNonNeg = this.data.every((v) => v >= 0);
    const msg = isAllNonNeg
      ? `N=${this.n} の非負配列をプロットしました (ボトム y=0)`
      : `N=${this.n} の配列をプロットしました (負数を含むため全体表示)`;
    this.showToast(msg);
  }

  // 入力文字列から配列データを抽出
  parseArrayInput(rawText) {
    const lines = rawText
      .split(/\r?\n/)
      .map((l) => l.trim())
      .filter((l) => l.length > 0);
    if (lines.length === 0) return null;

    // 全数値トークンを抽出
    const tokens = [];
    for (const line of lines) {
      // カンマや空白区切りに対応
      const matches = line.match(/-?\d+(?:\.\d+)?/g);
      if (matches) {
        tokens.push(...matches.map(Number));
      }
    }

    if (tokens.length === 0) return null;

    // 1行目が N で、続くトークン数が N と一致またはそれに近いか判定
    if (lines.length > 1) {
      const line1Tokens = lines[0].match(/-?\d+(?:\.\d+)?/g);
      if (line1Tokens && line1Tokens.length === 1) {
        const candidateN = line1Tokens[0];
        if (tokens.length - 1 >= candidateN) {
          return tokens.slice(1, 1 + candidateN);
        }
      }
    }

    // 1行目に N があり2行目以降に数値がある場合
    if (tokens.length > 1 && tokens[0] === tokens.length - 1 && tokens[0] > 0) {
      return tokens.slice(1);
    }

    // 単純な数列としてそのまま利用
    return tokens;
  }

  // 統計情報の更新
  updateStats() {
    if (this.n === 0) {
      this.statN.textContent = "-";
      this.statMin.textContent = "-";
      this.statMax.textContent = "-";
      this.statSum.textContent = "-";
      this.statAvg.textContent = "-";
      return;
    }

    const minVal = Math.min(...this.data);
    const minIdx = this.data.indexOf(minVal) + (this.is1Indexed ? 1 : 0);
    const maxVal = Math.max(...this.data);
    const maxIdx = this.data.indexOf(maxVal) + (this.is1Indexed ? 1 : 0);
    const sum = this.data.reduce((acc, v) => acc + v, 0);
    const avg = sum / this.n;

    this.statN.textContent = this.n;
    this.statMin.textContent = `${minVal} (i=${minIdx})`;
    this.statMax.textContent = `${maxVal} (i=${maxIdx})`;
    this.statSum.textContent = sum.toLocaleString();
    this.statAvg.textContent = Number.isInteger(avg) ? avg : avg.toFixed(2);
  }

  // 表示領域 (view) のリセット
  // ★ 要件: 「もしAの要素が全て非負である場合は、ボトムをy=0にして出力します」
  // ★ 要件: 「縦横の縮尺はデフォルトで1:1にして、ユーザーが変更できるようにして」
  resetView() {
    if (this.n === 0) {
      this.view = { xMin: 0, xMax: 10, yMin: 0, yMax: 10 };
      return;
    }

    const { left, right, top, bottom } = this.plotArea;
    const wPlot = Math.max(10, right - left);
    const hPlot = Math.max(10, bottom - top);

    const xStart = this.is1Indexed ? 1 : 0;
    const xEnd = this.is1Indexed ? this.n : this.n - 1;

    // 基本データ範囲
    const dataXSpan = Math.max(1, xEnd - xStart);
    const xCenter = (xStart + xEnd) / 2;
    const xMargin = this.n > 1 ? 0.6 : 1;
    const targetXSpan = dataXSpan + 2 * xMargin;

    const minY = Math.min(...this.data);
    const maxY = Math.max(...this.data);
    const isAllNonNegative = this.data.every((v) => v >= 0);

    let targetYMin = 0;
    let targetYMax = 10;

    if (minY === maxY) {
      if (minY > 0) {
        targetYMin = 0;
        targetYMax = minY * 1.4;
      } else if (minY === 0) {
        targetYMin = 0;
        targetYMax = 5;
      } else {
        targetYMin = minY * 1.3;
        targetYMax = 0;
      }
    } else {
      const span = maxY - minY;
      const margin = Math.max(1, span * 0.12);

      if (this.yAxisMode === "auto_zero") {
        if (isAllNonNegative) {
          // 【要件準拠】非負ならボトムは y=0
          targetYMin = 0;
          targetYMax = maxY + Math.max(1, (maxY - 0) * 0.12);
        } else {
          // 負数を含む場合は全体を表示
          targetYMin = minY - margin;
          targetYMax = maxY + margin;
        }
      } else if (this.yAxisMode === "always_zero") {
        targetYMin = Math.min(0, minY - margin);
        targetYMax = Math.max(0, maxY + margin);
      } else {
        // tight
        targetYMin = minY - margin;
        targetYMax = maxY + margin;
      }
    }

    const targetYSpan = Math.max(1, targetYMax - targetYMin);

    if (this.aspectRatioMode === "auto") {
      // 画面全体に独立自動フィット
      this.view = {
        xMin: xCenter - targetXSpan / 2,
        xMax: xCenter + targetXSpan / 2,
        yMin: targetYMin,
        yMax: targetYMax,
      };
      return;
    }

    // 縦横比 1:1 (デフォルト) または カスタム比率
    // ratio = (Yのピクセル長/目盛り) / (Xのピクセル長/目盛り)
    // 1:1 のときは ratio = 1.0 (等倍縮尺)
    const ratio = this.aspectRatioMode === "1:1" ? 1.0 : this.customYScale;

    // wPlot >= targetXSpan * sX
    // hPlot >= targetYSpan * (sX * ratio)
    // sX <= min(wPlot / targetXSpan, hPlot / (targetYSpan * ratio))
    const sX = Math.min(wPlot / targetXSpan, hPlot / (targetYSpan * ratio));
    const sY = sX * ratio;

    const viewXSpan = wPlot / sX;
    const viewYSpan = hPlot / sY;

    let yMin, yMax;
    if (this.yAxisMode === "auto_zero" && isAllNonNegative) {
      // 【要件準拠】非負ならボトム y=0
      yMin = 0;
      yMax = viewYSpan;
    } else {
      const yCenter = (targetYMin + targetYMax) / 2;
      yMin = yCenter - viewYSpan / 2;
      yMax = yCenter + viewYSpan / 2;
    }

    this.view = {
      xMin: xCenter - viewXSpan / 2,
      xMax: xCenter + viewXSpan / 2,
      yMin,
      yMax,
    };
  }

  // 座標変換: データ座標 -> スクリーン座標
  get plotArea() {
    return {
      left: 65,
      right: this.width - 25,
      top: 30,
      bottom: this.height - 45,
    };
  }

  toScreenX(x) {
    const { left, right } = this.plotArea;
    const ratio = (x - this.view.xMin) / (this.view.xMax - this.view.xMin);
    return left + ratio * (right - left);
  }

  toScreenY(y) {
    const { top, bottom } = this.plotArea;
    const ratio = (y - this.view.yMin) / (this.view.yMax - this.view.yMin);
    return bottom - ratio * (bottom - top);
  }

  toDataX(screenX) {
    const { left, right } = this.plotArea;
    const ratio = (screenX - left) / (right - left);
    return this.view.xMin + ratio * (this.view.xMax - this.view.xMin);
  }

  toDataY(screenY) {
    const { top, bottom } = this.plotArea;
    const ratio = (bottom - screenY) / (bottom - top);
    return this.view.yMin + ratio * (this.view.yMax - this.view.yMin);
  }

  // メイン描画処理
  render() {
    this.clearCanvas();
    if (this.n === 0) return;

    this.drawGridAndAxes();
    this.drawDataSeries();
    if (this.showPrefix) this.drawPrefixSeries();
    this.drawHoverCrosshair();
  }

  clearCanvas() {
    this.ctx.clearRect(0, 0, this.width, this.height);
  }

  // グリッド線と目盛りの描画
  drawGridAndAxes() {
    const ctx = this.ctx;
    const { left, right, top, bottom } = this.plotArea;

    // 背景領域
    ctx.fillStyle = "#0f172a";
    ctx.fillRect(left, top, right - left, bottom - top);

    // 目盛り生成
    let xTicks, yTicks;
    if (this.aspectRatioMode === "1:1") {
      // 1:1のときは目盛り間隔を同一にしてグリッドを正方形にする
      const sX = (right - left) / (this.view.xMax - this.view.xMin);
      const commonStep = this.calculateNiceStep(sX, true, 60);
      xTicks = this.generateTicksWithStep(this.view.xMin, this.view.xMax, commonStep);
      yTicks = this.generateTicksWithStep(this.view.yMin, this.view.yMax, commonStep);
    } else {
      xTicks = this.generateTicks(this.view.xMin, this.view.xMax, Math.floor((right - left) / 70), true);
      yTicks = this.generateTicks(this.view.yMin, this.view.yMax, Math.floor((bottom - top) / 50), false);
    }

    // グリッド線
    if (this.showGrid) {
      ctx.lineWidth = 1;
      ctx.strokeStyle = "rgba(51, 65, 85, 0.4)";

      // 縦グリッド
      xTicks.forEach((x) => {
        const sx = this.toScreenX(x);
        if (sx >= left && sx <= right) {
          ctx.beginPath();
          ctx.moveTo(sx, top);
          ctx.lineTo(sx, bottom);
          ctx.stroke();
        }
      });

      // 横グリッド
      yTicks.forEach((y) => {
        const sy = this.toScreenY(y);
        if (sy >= top && sy <= bottom) {
          ctx.beginPath();
          ctx.moveTo(left, sy);
          ctx.lineTo(right, sy);
          ctx.stroke();
        }
      });
    }

    // y = 0 の基準線強調 (重要な基準)
    if (this.showZero && this.view.yMin <= 0 && this.view.yMax >= 0) {
      const zeroY = this.toScreenY(0);
      ctx.lineWidth = 2;
      ctx.strokeStyle = "rgba(244, 63, 94, 0.75)"; // 薄い赤/ローズ色
      ctx.beginPath();
      ctx.moveTo(left, zeroY);
      ctx.lineTo(right, zeroY);
      ctx.stroke();

      // y=0 ラベル
      ctx.fillStyle = "rgba(244, 63, 94, 0.9)";
      ctx.font = "bold 10px monospace";
      ctx.textAlign = "right";
      ctx.fillText("y = 0", left - 8, zeroY + 3);
    }

    // 枠線
    ctx.lineWidth = 1;
    ctx.strokeStyle = "#334155";
    ctx.strokeRect(left, top, right - left, bottom - top);

    // 目盛りラベル描画
    ctx.font = "11px monospace";
    ctx.fillStyle = "#94a3b8";

    // X軸ラベル
    ctx.textAlign = "center";
    ctx.textBaseline = "top";
    xTicks.forEach((x) => {
      const sx = this.toScreenX(x);
      if (sx >= left && sx <= right) {
        ctx.fillText(Number.isInteger(x) ? x : x.toFixed(1), sx, bottom + 8);
      }
    });

    // X軸タイトル
    ctx.fillText(this.is1Indexed ? "インデックス i (1-indexed)" : "インデックス i (0-indexed)", (left + right) / 2, bottom + 26);

    // Y軸ラベル
    ctx.textAlign = "right";
    ctx.textBaseline = "middle";
    yTicks.forEach((y) => {
      const sy = this.toScreenY(y);
      if (sy >= top && sy <= bottom) {
        ctx.fillText(this.formatNumber(y), left - 8, sy);
      }
    });

    // Y軸タイトル
    ctx.save();
    ctx.translate(16, (top + bottom) / 2);
    ctx.rotate(-Math.PI / 2);
    ctx.textAlign = "center";
    ctx.fillText("値 A[i]", 0, 0);
    ctx.restore();
  }

  // 配列データの描画 (折れ線 / 棒 / 散布 / 階段)
  drawDataSeries() {
    const ctx = this.ctx;
    const { left, right, top, bottom } = this.plotArea;

    const points = [];
    for (let i = 0; i < this.n; i++) {
      const x = i + (this.is1Indexed ? 1 : 0);
      const y = this.data[i];
      points.push({ index: i, x, y, sx: this.toScreenX(x), sy: this.toScreenY(y) });
    }

    // クリップ設定 (プロット領域外を描画しない)
    ctx.save();
    ctx.beginPath();
    ctx.rect(left, top, right - left, bottom - top);
    ctx.clip();

    const isBar = this.graphType === "bar";
    const isStep = this.graphType === "step";
    const isLine = this.graphType === "line";

    // 1. 棒グラフ (Bar)
    if (isBar) {
      const baseSy = this.toScreenY(0 >= this.view.yMin && 0 <= this.view.yMax ? 0 : this.view.yMin);
      const barWidth = Math.max(3, Math.min(48, ((right - left) / (this.view.xMax - this.view.xMin)) * 0.65));

      points.forEach((pt) => {
        const x = pt.sx - barWidth / 2;
        const y = Math.min(pt.sy, baseSy);
        const h = Math.abs(pt.sy - baseSy);

        const grad = ctx.createLinearGradient(0, pt.sy, 0, baseSy);
        if (pt.y >= 0) {
          grad.addColorStop(0, "rgba(59, 130, 246, 0.85)");
          grad.addColorStop(1, "rgba(6, 182, 212, 0.3)");
        } else {
          grad.addColorStop(0, "rgba(244, 63, 94, 0.85)");
          grad.addColorStop(1, "rgba(244, 63, 94, 0.2)");
        }

        ctx.fillStyle = grad;
        ctx.fillRect(x, y, barWidth, h);

        ctx.strokeStyle = pt.y >= 0 ? "#38bdf8" : "#fb7185";
        ctx.lineWidth = 1.5;
        ctx.strokeRect(x, y, barWidth, h);
      });
    }

    // 2. 階段グラフ (Step)
    if (isStep && points.length > 0) {
      ctx.beginPath();
      ctx.moveTo(points[0].sx, points[0].sy);
      for (let i = 0; i < points.length - 1; i++) {
        const nextX = points[i + 1].sx;
        const curY = points[i].sy;
        ctx.lineTo(nextX, curY);
        ctx.lineTo(nextX, points[i + 1].sy);
      }
      ctx.strokeStyle = "#38bdf8";
      ctx.lineWidth = 2.5;
      ctx.stroke();
    }

    // 3. 折れ線グラフ (Line)
    if (isLine && points.length > 0) {
      // 塗りつぶしグラデーション (エリア)
      const baseSy = this.toScreenY(0 >= this.view.yMin && 0 <= this.view.yMax ? 0 : this.view.yMin);
      ctx.beginPath();
      ctx.moveTo(points[0].sx, baseSy);
      ctx.lineTo(points[0].sx, points[0].sy);
      for (let i = 1; i < points.length; i++) {
        ctx.lineTo(points[i].sx, points[i].sy);
      }
      ctx.lineTo(points[points.length - 1].sx, baseSy);
      ctx.closePath();

      const areaGrad = ctx.createLinearGradient(0, top, 0, bottom);
      areaGrad.addColorStop(0, "rgba(59, 130, 246, 0.35)");
      areaGrad.addColorStop(1, "rgba(59, 130, 246, 0.02)");
      ctx.fillStyle = areaGrad;
      ctx.fill();

      // 線本体
      ctx.beginPath();
      ctx.moveTo(points[0].sx, points[0].sy);
      for (let i = 1; i < points.length; i++) {
        ctx.lineTo(points[i].sx, points[i].sy);
      }
      ctx.strokeStyle = "#38bdf8";
      ctx.lineWidth = 2.5;
      ctx.lineJoin = "round";
      ctx.stroke();
    }

    // 4. データ点 (Points) の描画
    const isScatter = this.graphType === "scatter";
    const pointRadius = isScatter
      ? this.n <= 50 ? 6 : this.n <= 200 ? 4 : 2.5
      : this.n <= 50 ? 5 : this.n <= 200 ? 3 : 1.5;

    points.forEach((pt) => {
      ctx.beginPath();
      ctx.arc(pt.sx, pt.sy, pointRadius, 0, Math.PI * 2);
      ctx.fillStyle = pt.y >= 0 ? "#06b6d4" : "#f43f5e";
      ctx.fill();
      ctx.strokeStyle = "#ffffff";
      ctx.lineWidth = isScatter ? 2 : 1.5;
      ctx.stroke();
    });

    // 5. データラベル (各点の上に数値を表示)
    if (this.showLabels && this.n <= 60) {
      ctx.font = "bold 10px monospace";
      ctx.textAlign = "center";
      points.forEach((pt) => {
        const offset = pt.y >= 0 ? -10 : 16;
        // 背景の薄い影
        ctx.fillStyle = "rgba(15, 23, 42, 0.85)";
        ctx.fillText(pt.y, pt.sx + 1, pt.sy + offset + 1);
        ctx.fillStyle = pt.y >= 0 ? "#e2e8f0" : "#fca5a5";
        ctx.fillText(pt.y, pt.sx, pt.sy + offset);
      });
    }

    ctx.restore();
  }

  // 累積和 (Prefix Sum) の同時描画
  drawPrefixSeries() {
    const ctx = this.ctx;
    const { left, right, top, bottom } = this.plotArea;

    let sum = 0;
    const prefixPoints = [];
    for (let i = 0; i < this.n; i++) {
      sum += this.data[i];
      const x = i + (this.is1Indexed ? 1 : 0);
      prefixPoints.push({ x, y: sum, sx: this.toScreenX(x), sy: this.toScreenY(sum) });
    }

    ctx.save();
    ctx.beginPath();
    ctx.rect(left, top, right - left, bottom - top);
    ctx.clip();

    ctx.setLineDash([5, 4]);
    ctx.beginPath();
    ctx.moveTo(prefixPoints[0].sx, prefixPoints[0].sy);
    for (let i = 1; i < prefixPoints.length; i++) {
      ctx.lineTo(prefixPoints[i].sx, prefixPoints[i].sy);
    }
    ctx.strokeStyle = "#f59e0b"; // アンバー色
    ctx.lineWidth = 2;
    ctx.stroke();
    ctx.setLineDash([]);

    // 累積和マーカー (ひし形)
    prefixPoints.forEach((pt) => {
      ctx.fillStyle = "#f59e0b";
      ctx.beginPath();
      ctx.moveTo(pt.sx, pt.sy - 4);
      ctx.lineTo(pt.sx + 4, pt.sy);
      ctx.lineTo(pt.sx, pt.sy + 4);
      ctx.lineTo(pt.sx - 4, pt.sy);
      ctx.closePath();
      ctx.fill();
    });

    ctx.restore();
  }

  // マウスホバー時のクロスヘアとハイライト
  drawHoverCrosshair() {
    if (!this.hoverPoint) return;
    const { sx, sy, x, y, index } = this.hoverPoint;
    const { left, right, top, bottom } = this.plotArea;

    const ctx = this.ctx;
    ctx.save();
    ctx.beginPath();
    ctx.rect(left, top, right - left, bottom - top);
    ctx.clip();

    // 縦クロスヘア
    ctx.setLineDash([3, 3]);
    ctx.strokeStyle = "rgba(148, 163, 184, 0.6)";
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(sx, top);
    ctx.lineTo(sx, bottom);
    ctx.stroke();

    // 横クロスヘア
    ctx.beginPath();
    ctx.moveTo(left, sy);
    ctx.lineTo(right, sy);
    ctx.stroke();
    ctx.setLineDash([]);

    // ホバー対象の円を大きく光らせる
    ctx.beginPath();
    ctx.arc(sx, sy, 8, 0, Math.PI * 2);
    ctx.fillStyle = "rgba(6, 182, 212, 0.4)";
    ctx.fill();
    ctx.beginPath();
    ctx.arc(sx, sy, 5, 0, Math.PI * 2);
    ctx.fillStyle = "#ffffff";
    ctx.fill();
    ctx.strokeStyle = "#06b6d4";
    ctx.lineWidth = 2;
    ctx.stroke();

    ctx.restore();
  }

  // マウス操作 (パン・ズーム・ホバー)
  onMouseDown(e) {
    if (e.button !== 0) return;
    this.isDragging = true;
    this.dragStart = { x: e.clientX, y: e.clientY };
    this.viewStart = { ...this.view };
    this.canvas.style.cursor = "grabbing";
  }

  onMouseMove(e) {
    const rect = this.canvas.getBoundingClientRect();
    const mouseX = e.clientX - rect.left;
    const mouseY = e.clientY - rect.top;

    if (this.isDragging) {
      const dxPixels = e.clientX - this.dragStart.x;
      const dyPixels = e.clientY - this.dragStart.y;

      const { left, right, top, bottom } = this.plotArea;
      const xSpan = this.viewStart.xMax - this.viewStart.xMin;
      const ySpan = this.viewStart.yMax - this.viewStart.yMin;

      const dxData = -(dxPixels / (right - left)) * xSpan;
      const dyData = (dyPixels / (bottom - top)) * ySpan;

      this.view.xMin = this.viewStart.xMin + dxData;
      this.view.xMax = this.viewStart.xMax + dxData;
      this.view.yMin = this.viewStart.yMin + dyData;
      this.view.yMax = this.viewStart.yMax + dyData;

      this.render();
      return;
    }

    // ホバー判定
    const { left, right, top, bottom } = this.plotArea;
    if (mouseX < left || mouseX > right || mouseY < top || mouseY > bottom || this.n === 0) {
      this.hoverPoint = null;
      this.tooltip.style.display = "none";
      this.hoverCoordText.textContent = "座標: マウスをホバーすると値を表示します";
      this.render();
      return;
    }

    // 最も近いデータ点を検索
    let closestPt = null;
    let minDist = 30; // 検出半径 (ピクセル)

    for (let i = 0; i < this.n; i++) {
      const x = i + (this.is1Indexed ? 1 : 0);
      const y = this.data[i];
      const sx = this.toScreenX(x);
      const sy = this.toScreenY(y);
      const dist = Math.hypot(mouseX - sx, mouseY - sy);
      if (dist < minDist) {
        minDist = dist;
        closestPt = { index: i, x, y, sx, sy };
      }
    }

    if (closestPt) {
      this.hoverPoint = closestPt;
      this.updateTooltip(closestPt, mouseX, mouseY);
    } else {
      this.hoverPoint = null;
      this.tooltip.style.display = "none";
      const dataX = this.toDataX(mouseX);
      const dataY = this.toDataY(mouseY);
      this.hoverCoordText.textContent = `マウス座標: x ≈ ${dataX.toFixed(2)}, y ≈ ${dataY.toFixed(2)}`;
    }
    this.render();
  }

  onMouseUp() {
    this.isDragging = false;
    this.canvas.style.cursor = "crosshair";
  }

  onWheel(e) {
    e.preventDefault();
    const rect = this.canvas.getBoundingClientRect();
    const mouseX = e.clientX - rect.left;
    const mouseY = e.clientY - rect.top;

    // アップクローズ（ズーム）感度を約半分に調整 (Macトラックパッド等の連続スクロールにも滑らかに対応)
    const rawDelta = Math.max(-100, Math.min(100, e.deltaY));
    const zoomFactor = Math.exp(rawDelta * 0.00075);
    this.zoomAtPoint(mouseX, mouseY, zoomFactor);
  }

  zoom(factor) {
    const { left, right, top, bottom } = this.plotArea;
    const centerX = (left + right) / 2;
    const centerY = (top + bottom) / 2;
    this.zoomAtPoint(centerX, centerY, factor);
  }

  zoomAtPoint(screenX, screenY, factor) {
    const dataX = this.toDataX(screenX);
    const dataY = this.toDataY(screenY);

    const newXSpan = (this.view.xMax - this.view.xMin) * factor;
    const newYSpan = (this.view.yMax - this.view.yMin) * factor;

    const xRatio = (screenX - this.plotArea.left) / (this.plotArea.right - this.plotArea.left);
    const yRatio = (this.plotArea.bottom - screenY) / (this.plotArea.bottom - this.plotArea.top);

    this.view.xMin = dataX - xRatio * newXSpan;
    this.view.xMax = this.view.xMin + newXSpan;
    this.view.yMin = dataY - yRatio * newYSpan;
    this.view.yMax = this.view.yMin + newYSpan;

    this.render();
  }

  updateTooltip(pt, mouseX, mouseY) {
    const i = pt.index;
    const displayIdx = pt.x;
    const val = pt.y;

    // 前の要素との差分
    let diffStr = "-";
    if (i > 0) {
      const diff = val - this.data[i - 1];
      diffStr = diff > 0 ? `+${diff}` : `${diff}`;
    }

    // 累積和
    let prefixSum = 0;
    for (let k = 0; k <= i; k++) prefixSum += this.data[k];

    this.tooltip.innerHTML = `
      <div style="font-weight: bold; color: var(--accent-cyan); margin-bottom: 2px;">
        点 (${displayIdx}, ${val})
      </div>
      <div>インデックス: <b>i = ${displayIdx}</b></div>
      <div>要素値: <b>A[${displayIdx}] = ${val}</b></div>
      <div>前との差: <b style="color: ${diffStr.startsWith('+') ? '#10b981' : diffStr.startsWith('-') ? '#f43f5e' : 'inherit'}">${diffStr}</b></div>
      <div>累積和: <b>Σ = ${prefixSum}</b></div>
    `;

    this.tooltip.style.left = `${pt.sx}px`;
    this.tooltip.style.top = `${pt.sy}px`;
    this.tooltip.style.display = "block";

    this.hoverCoordText.textContent = `選択中: i=${displayIdx}, A[${displayIdx}]=${val} (前との差: ${diffStr})`;
  }

  // 目盛りの自動計算 (綺麗なくぎり)
  generateTicks(min, max, maxTicks = 8, isIntegerOnly = false) {
    const span = max - min;
    if (span <= 0) return [min];

    const rawStep = span / Math.max(2, maxTicks);
    const power = Math.floor(Math.log10(rawStep));
    const magnitude = Math.pow(10, power);
    const normalized = rawStep / magnitude;

    let step = magnitude;
    if (normalized >= 5) {
      step = 5 * magnitude;
    } else if (normalized >= 2) {
      step = 2 * magnitude;
    } else {
      step = magnitude;
    }

    if (isIntegerOnly && step < 1) step = 1;

    const first = Math.ceil(min / step) * step;
    const ticks = [];
    for (let val = first; val <= max + step * 0.001; val += step) {
      // 浮動小数点誤差対策
      const cleanVal = Math.round(val / step) * step;
      ticks.push(cleanVal);
    }
    return ticks;
  }

  // 指定ピクセル間隔に応じた綺麗なステップを計算 (1:1 縮尺用)
  calculateNiceStep(pixelsPerUnit, isIntegerOnly = false, targetPixelDist = 60) {
    const rawStep = targetPixelDist / Math.max(0.001, pixelsPerUnit);
    const power = Math.floor(Math.log10(rawStep));
    const magnitude = Math.pow(10, power);
    const normalized = rawStep / magnitude;

    let step = magnitude;
    if (normalized >= 5) {
      step = 5 * magnitude;
    } else if (normalized >= 2) {
      step = 2 * magnitude;
    } else {
      step = magnitude;
    }

    if (isIntegerOnly && step < 1) step = 1;
    return step;
  }

  // 指定ステップ幅での目盛り生成
  generateTicksWithStep(min, max, step) {
    if (step <= 0) return [min];
    const first = Math.ceil(min / step) * step;
    const ticks = [];
    for (let val = first; val <= max + step * 0.001; val += step) {
      const cleanVal = Math.round(val / step) * step;
      ticks.push(cleanVal);
    }
    return ticks;
  }

  formatNumber(val) {
    if (Math.abs(val) >= 1e6 || (Math.abs(val) > 0 && Math.abs(val) < 1e-2)) {
      return val.toExponential(1);
    }
    return Number.isInteger(val) ? val.toString() : val.toFixed(1);
  }

  // エクスポート: テキストコピー
  exportText() {
    if (this.n === 0) return;
    const text = `${this.n}\n${this.data.join(" ")}`;
    navigator.clipboard.writeText(text).then(() => {
      this.showToast("配列データ (N と A_1...A_N) をコピーしました！");
    });
  }

  // エクスポート: C++ コード
  exportCpp() {
    if (this.n === 0) return;
    const cpp = `int n = ${this.n};\nvector<long long> a = {${this.data.join(", ")}};`;
    navigator.clipboard.writeText(cpp).then(() => {
      this.showToast("C++ vector<long long> コードをコピーしました！");
    });
  }

  // エクスポート: 画像保存 (PNG)
  saveImage() {
    if (this.n === 0) return;
    const link = document.createElement("a");
    link.download = `array_graph_N${this.n}.png`;
    link.href = this.canvas.toDataURL("image/png");
    link.click();
    this.showToast("グラフ画像を保存しました！");
  }

  toggleSidebar() {
    if (!this.appContainer) return;
    const isCollapsed = this.appContainer.classList.contains("sidebar-collapsed");
    this.setSidebarCollapsed(!isCollapsed);
  }

  setSidebarCollapsed(collapsed, animate = true) {
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
      localStorage.setItem("array_graph_sidebar_collapsed", collapsed ? "true" : "false");
    } catch (err) {}

    // アニメーション完了後にキャンバスをリサイズして再描画
    if (animate) {
      setTimeout(() => {
        this.resizeCanvas();
        if (this.aspectRatioMode !== "auto") {
          this.resetView();
        }
        this.render();
      }, 260);
    } else {
      this.resizeCanvas();
      this.render();
    }
  }

  showToast(message) {
    this.toastEl.textContent = message;
    this.toastEl.classList.add("show");
    setTimeout(() => {
      this.toastEl.classList.remove("show");
    }, 2500);
  }
}

// 起動
window.addEventListener("DOMContentLoaded", () => {
  new ArrayGraphVisualizer();
});
