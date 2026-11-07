/**
 * AtCoder Parallel Stress Tester - script.js
 * High-performance Multi-Core Parallel Stress Engine Frontend
 */

document.addEventListener("DOMContentLoaded", () => {
  // DOM Elements - Header & Tools
  const badgeCpuCores = document.getElementById("badge-cpu-cores");
  const btnSyncToolsLoad = document.getElementById("btn-sync-tools-load");
  const btnSyncToolsSave = document.getElementById("btn-sync-tools-save");

  // DOM Elements - Control Bar
  const btnStart = document.getElementById("btn-start");
  const btnStop = document.getElementById("btn-stop");
  const inputWorkersRange = document.getElementById("input-workers-range");
  const labelWorkers = document.getElementById("label-workers");
  const selectMaxCases = document.getElementById("select-max-cases");
  const selectMode = document.getElementById("select-mode");
  const selectTimeout = document.getElementById("select-timeout");
  const checkSanitizeSolver = document.getElementById("check-sanitize-solver");
  const btnToggleCodes = document.getElementById("btn-toggle-codes");
  const mainSplit = document.querySelector(".main-split");

  // DOM Elements - Dashboard Metrics
  const metricSpeed = document.getElementById("metric-speed");
  const metricWorkersActive = document.getElementById("metric-workers-active");
  const metricProgressText = document.getElementById("metric-progress-text");
  const progressBarFill = document.getElementById("progress-bar-fill");
  const metricStatusMsg = document.getElementById("metric-status-msg");
  const metricTimeText = document.getElementById("metric-time-text");
  const pillAc = document.getElementById("pill-ac");
  const pillWa = document.getElementById("pill-wa");
  const pillRe = document.getElementById("pill-re");
  const pillTle = document.getElementById("pill-tle");
  const metricFailSummary = document.getElementById("metric-fail-summary");

  // DOM Elements - Code Editors
  const codeTabBtns = document.querySelectorAll(".code-tab-btn");
  const codePanels = document.querySelectorAll(".code-panel");
  const codeSolver = document.getElementById("code-solver");
  const codeNaive = document.getElementById("code-naive");
  const codeGen = document.getElementById("code-gen");
  const btnCopySolver = document.getElementById("btn-copy-solver");
  const btnCopyNaive = document.getElementById("btn-copy-naive");
  const btnCopyGen = document.getElementById("btn-copy-gen");

  // DOM Elements - Inspector & Failures
  const minimalCaseBanner = document.getElementById("minimal-case-banner");
  const minimalMetaText = document.getElementById("minimal-meta-text");
  const btnLoadMinimal = document.getElementById("btn-load-minimal");
  const failureCount = document.getElementById("failure-count");
  const filterBtns = document.querySelectorAll(".filter-btn");
  const failureTbody = document.getElementById("failure-tbody");

  // DOM Elements - Detail Section
  const failureDetailSection = document.getElementById("failure-detail-section");
  const detailVerdictBadge = document.getElementById("detail-verdict-badge");
  const detailCaseTitle = document.getElementById("detail-case-title");
  const btnTransferCpDebug = document.getElementById("btn-transfer-cp-debug");
  const detailInputPre = document.getElementById("detail-input-pre");
  const detailNaivePre = document.getElementById("detail-naive-pre");
  const detailSolverPre = document.getElementById("detail-solver-pre");
  const btnCopyFailInput = document.getElementById("btn-copy-fail-input");
  const btnCopyFailNaive = document.getElementById("btn-copy-fail-naive");
  const btnCopyFailSolver = document.getElementById("btn-copy-fail-solver");

  // State
  let eventSource = null;
  let allFailures = [];
  let currentFilter = "ALL";
  let selectedFailure = null;
  let minimalFailure = null;
  let isRunning = false;

  // 1. Fetch System Info (CPU Cores)
  fetch("/api/system_info")
    .then(res => res.json())
    .then(data => {
      const cores = data.cpu_count || 8;
      badgeCpuCores.textContent = `⚡️ ${cores} Cores Available`;
      inputWorkersRange.max = Math.max(16, cores * 2);
      inputWorkersRange.value = cores;
      labelWorkers.textContent = `${cores} コア`;
    })
    .catch(() => {
      badgeCpuCores.textContent = "⚡️ Multi-Core Engine";
    });

  inputWorkersRange.addEventListener("input", () => {
    labelWorkers.textContent = `${inputWorkersRange.value} コア`;
  });

  // 2. Editor Tabs & Persistence
  function switchCodeTab(targetId) {
    codeTabBtns.forEach(b => b.classList.toggle("active", b.getAttribute("data-target") === targetId));
    codePanels.forEach(p => p.classList.toggle("active", p.id === targetId));
    try {
      localStorage.setItem("pst_active_code_tab", targetId);
    } catch (e) {}
  }

  codeTabBtns.forEach(btn => {
    btn.addEventListener("click", () => {
      const targetId = btn.getAttribute("data-target");
      switchCodeTab(targetId);
    });
  });

  // Collapse / Expand Code Editor Pane
  function setCodesCollapsed(collapsed) {
    if (!mainSplit) return;
    mainSplit.classList.toggle("codes-collapsed", collapsed);
    if (btnToggleCodes) {
      btnToggleCodes.classList.toggle("active", collapsed);
      btnToggleCodes.textContent = collapsed ? "◧ エディタ表示" : "◧ エディタ隠す";
    }
    try {
      localStorage.setItem("pst_codes_collapsed", collapsed ? "true" : "false");
    } catch (e) {}
  }

  function toggleCodes() {
    if (!mainSplit) return;
    setCodesCollapsed(!mainSplit.classList.contains("codes-collapsed"));
  }

  if (btnToggleCodes) {
    btnToggleCodes.addEventListener("click", toggleCodes);
  }

  window.addEventListener("keydown", (e) => {
    if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "b") {
      e.preventDefault();
      toggleCodes();
    } else if ((e.ctrlKey || e.metaKey) && e.key === "Enter") {
      if (!isRunning) {
        e.preventDefault();
        btnStart.click();
      }
    }
  });

  // Draft code auto-save
  codeSolver.addEventListener("input", () => {
    try { localStorage.setItem("pst_draft_solver", codeSolver.value); } catch (e) {}
  });
  codeNaive.addEventListener("input", () => {
    try { localStorage.setItem("pst_draft_naive", codeNaive.value); } catch (e) {}
  });
  codeGen.addEventListener("input", () => {
    try { localStorage.setItem("pst_draft_gen", codeGen.value); } catch (e) {}
  });

  // Copy code buttons
  setupCopy(btnCopySolver, codeSolver);
  setupCopy(btnCopyNaive, codeNaive);
  setupCopy(btnCopyGen, codeGen);
  setupCopy(btnCopyFailInput, detailInputPre);
  setupCopy(btnCopyFailNaive, detailNaivePre);
  setupCopy(btnCopyFailSolver, detailSolverPre);

  function setupCopy(btn, target) {
    if (!btn || !target) return;
    btn.addEventListener("click", () => {
      const text = target.value !== undefined ? target.value : target.textContent;
      navigator.clipboard.writeText(text).then(() => {
        const orig = btn.textContent;
        btn.textContent = "コピー完了";
        setTimeout(() => { btn.textContent = orig; }, 1400);
      });
    });
  }

  // 3. Load & Save tools/stress_test/ files
  btnSyncToolsLoad.addEventListener("click", async () => {
    try {
      const res = await fetch("/api/load_tools_files");
      const data = await res.json();
      if (data.success && data.files) {
        if (data.files.main) codeSolver.value = data.files.main;
        if (data.files.naive) codeNaive.value = data.files.naive;
        if (data.files.gen) codeGen.value = data.files.gen;
        btnSyncToolsLoad.textContent = "✅ 読込完了";
        setTimeout(() => { btnSyncToolsLoad.textContent = "📂 tools/ から読込"; }, 1500);
      }
    } catch (e) {
      alert("tools/ フォルダからの読み込みに失敗しました。");
    }
  });

  btnSyncToolsSave.addEventListener("click", async () => {
    try {
      const res = await fetch("/api/save_tools_files", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          solver_code: codeSolver.value,
          naive_code: codeNaive.value,
          generator_code: codeGen.value
        })
      });
      const data = await res.json();
      if (data.success) {
        btnSyncToolsSave.textContent = "✅ 保存完了";
        setTimeout(() => { btnSyncToolsSave.textContent = "💾 tools/ へ保存"; }, 1500);
      }
    } catch (e) {
      alert("tools/ への保存に失敗しました。");
    }
  });

  // 5. Start & Stop Stress Testing
  btnStart.addEventListener("click", startStressTest);
  btnStop.addEventListener("click", stopStressTest);

  document.addEventListener("keydown", (e) => {
    if ((e.ctrlKey || e.metaKey) && e.key === "Enter") {
      e.preventDefault();
      if (!isRunning) startStressTest();
    }
  });

  async function startStressTest() {
    if (isRunning) return;

    const solver = codeSolver.value.trim();
    const naive = codeNaive.value.trim();
    const gen = codeGen.value.trim();

    if (!solver || !naive || !gen) {
      alert("Solver, Naive, Generator のすべてのコードを入力してください。");
      return;
    }

    // Reset UI
    allFailures = [];
    minimalFailure = null;
    selectedFailure = null;
    failureCount.textContent = "0";
    minimalCaseBanner.style.display = "none";
    failureDetailSection.style.display = "none";
    failureTbody.innerHTML = `<tr><td colspan="5" class="empty-cell">テスト実行中...</td></tr>`;

    isRunning = true;
    btnStart.disabled = true;
    btnStop.disabled = false;
    metricStatusMsg.textContent = "並列テスト中...";
    progressBarFill.style.background = "linear-gradient(90deg, #38bdf8, #10b981)";

    const payload = {
      solver_code: solver,
      naive_code: naive,
      generator_code: gen,
      workers: parseInt(inputWorkersRange.value, 10),
      max_cases: parseInt(selectMaxCases.value, 10),
      mode: selectMode.value,
      timeout_sec: parseFloat(selectTimeout.value),
      sanitize_solver: checkSanitizeSolver.checked
    };

    try {
      const resp = await fetch("/api/stress/start", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(payload)
      });
      const data = await resp.json();
      if (!data.success) {
        alert(data.message || "起動に失敗しました。");
        finishRun();
        return;
      }

      // Connect SSE for Real-time streaming
      connectSSE();
    } catch (err) {
      console.error(err);
      finishRun();
    }
  }

  function stopStressTest() {
    fetch("/api/stress/stop", { method: "POST" });
    metricStatusMsg.textContent = "停止中...";
  }

  function connectSSE() {
    if (eventSource) {
      eventSource.close();
    }

    eventSource = new EventSource("/api/stress/stream");

    eventSource.onmessage = (e) => {
      try {
        const data = JSON.parse(e.data);
        handleStreamUpdate(data);
      } catch (err) {
        console.error("SSE parse error", err);
      }
    };

    eventSource.onerror = () => {
      // Disconnect
      eventSource.close();
      eventSource = null;
    };
  }

  function handleStreamUpdate(data) {
    if (data.type === "error") {
      alert(data.error);
      metricStatusMsg.textContent = "エラーが発生しました";
      finishRun();
      return;
    }

    const { completed, total, elapsed, speed, active_workers, counts, failures, minimal_failure, is_running } = data;

    // Update Speedometer & Workers
    metricSpeed.textContent = Math.round(speed || 0);
    metricWorkersActive.textContent = `${active_workers || inputWorkersRange.value} コア稼働中`;

    // Update Progress
    const pct = total > 0 ? Math.min(100, (completed / total) * 100) : 0;
    progressBarFill.style.width = `${pct}%`;
    metricProgressText.textContent = `${completed.toLocaleString()} / ${total.toLocaleString()} (${pct.toFixed(1)}%)`;
    metricTimeText.textContent = `経過: ${elapsed}s`;

    // Update Counts
    if (counts) {
      pillAc.textContent = `AC: ${counts.AC || 0}`;
      pillWa.textContent = `WA: ${counts.WA || 0}`;
      pillRe.textContent = `RE: ${counts.RE || 0}`;
      pillTle.textContent = `TLE: ${counts.TLE || 0}`;
    }

    // Update Failures List
    if (failures) {
      allFailures = failures;
      failureCount.textContent = allFailures.length;
      renderFailuresTable();

      if (allFailures.length > 0) {
        progressBarFill.style.background = "linear-gradient(90deg, #f59e0b, #ef4444)";
        metricFailSummary.textContent = `🚨 ${allFailures.length} 件の不一致を検出！`;
      }
    }

    // Minimal Failure
    if (minimal_failure) {
      minimalFailure = minimal_failure;
      minimalCaseBanner.style.display = "flex";
      minimalMetaText.textContent = `ケース #${minimalFailure.id} (判定: ${minimalFailure.verdict}, 入力長: ${minimalFailure.size} 文字, 実行: ${minimalFailure.time_ms}ms)`;
    }

    // Finished
    if (data.type === "finished" || is_running === false) {
      finishRun();
      if (allFailures.length === 0) {
        metricStatusMsg.textContent = `🎉 全 ${completed} ケースに合格！バグは検出されませんでした。`;
        metricFailSummary.textContent = "全ケース合格";
      } else {
        metricStatusMsg.textContent = `🛑 ${allFailures.length} 件のバグケースを検出しました。`;
      }
    }
  }

  function finishRun() {
    isRunning = false;
    btnStart.disabled = false;
    btnStop.disabled = true;
    if (eventSource) {
      eventSource.close();
      eventSource = null;
    }
  }

  // 6. Failures Table & Filtering
  filterBtns.forEach(btn => {
    btn.addEventListener("click", () => {
      filterBtns.forEach(b => b.classList.toggle("active", b === btn));
      currentFilter = btn.getAttribute("data-filter");
      renderFailuresTable();
    });
  });

  function renderFailuresTable() {
    const filtered = allFailures.filter(f => currentFilter === "ALL" || f.verdict === currentFilter);

    if (filtered.length === 0) {
      failureTbody.innerHTML = `<tr><td colspan="5" class="empty-cell">${allFailures.length === 0 ? "失敗ケースは検出されていません。" : "フィルターに一致するケースはありません。"}</td></tr>`;
      return;
    }

    let html = "";
    filtered.forEach(f => {
      const isSel = selectedFailure && selectedFailure.id === f.id;
      const isMin = minimalFailure && minimalFailure.id === f.id;
      const badgeCls = `badge-${f.verdict.toLowerCase()}`;
      const minTag = isMin ? '<span style="color:#fbbf24; margin-left:4px;" title="最小入力ケース">🏆</span>' : "";

      html += `
        <tr class="failure-row ${isSel ? 'selected' : ''}" data-id="${f.id}">
          <td>#${f.id}${minTag}</td>
          <td><span class="${badgeCls}">${f.verdict}</span></td>
          <td>${f.size} 文字</td>
          <td>${f.time_ms} ms</td>
          <td>${escapeHtml(f.input.substring(0, 35).replace(/\n/g, " "))}...</td>
        </tr>
      `;
    });
    failureTbody.innerHTML = html;

    // Attach row click listeners
    failureTbody.querySelectorAll(".failure-row").forEach(row => {
      row.addEventListener("click", () => {
        const id = parseInt(row.getAttribute("data-id"), 10);
        const item = allFailures.find(x => x.id === id);
        if (item) selectFailure(item);
      });
    });
  }

  function selectFailure(item) {
    selectedFailure = item;
    renderFailuresTable();

    failureDetailSection.style.display = "flex";
    detailVerdictBadge.textContent = item.verdict;
    detailVerdictBadge.className = `detail-badge badge-${item.verdict.toLowerCase()}`;
    detailCaseTitle.textContent = `ケース #${item.id} (${item.size}文字 / ${item.time_ms}ms)`;

    detailInputPre.textContent = item.input;
    detailNaivePre.textContent = item.naive_out || "(出力なし)";
    detailSolverPre.textContent = item.solver_out || item.solver_err || "(出力なし)";
  }

  btnLoadMinimal.addEventListener("click", () => {
    if (minimalFailure) {
      selectFailure(minimalFailure);
    }
  });

  // 7. Transfer to CP Debugger
  btnTransferCpDebug.addEventListener("click", () => {
    if (!selectedFailure) return;

    // Store in localStorage for CP Debugger to consume
    try {
      localStorage.setItem("cpd_transfer_code", codeSolver.value);
      localStorage.setItem("cpd_transfer_input", selectedFailure.input);
      localStorage.setItem("cpd_transfer_expected", selectedFailure.naive_out);
    } catch (e) {}

    // Open CP Debugger in a new tab or notify
    const cpdUrl = "http://127.0.0.1:8080";
    window.open(cpdUrl, "_blank");
  });

  function escapeHtml(str) {
    if (!str) return "";
    return String(str)
      .replace(/&/g, "&amp;")
      .replace(/</g, "&lt;")
      .replace(/>/g, "&gt;")
      .replace(/"/g, "&quot;")
      .replace(/'/g, "&#039;");
  }

  // Initial values (empty)
  codeSolver.value = "";
  codeNaive.value = "";
  codeGen.value = "";

  // Restore drafts and panel state from localStorage
  try {
    const savedSolver = localStorage.getItem("pst_draft_solver");
    if (savedSolver !== null && savedSolver !== undefined) codeSolver.value = savedSolver;
    const savedNaive = localStorage.getItem("pst_draft_naive");
    if (savedNaive !== null && savedNaive !== undefined) codeNaive.value = savedNaive;
    const savedGen = localStorage.getItem("pst_draft_gen");
    if (savedGen !== null && savedGen !== undefined) codeGen.value = savedGen;

    if (localStorage.getItem("pst_codes_collapsed") === "true") {
      setCodesCollapsed(true);
    }

    const savedTab = localStorage.getItem("pst_active_code_tab");
    if (savedTab) {
      switchCodeTab(savedTab);
    }
  } catch (e) {}
});
