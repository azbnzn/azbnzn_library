/**
 * AtCoder CP Debugger - script.js
 * 100% Deterministic & Contest-Safe Debugger Engine Frontend
 */

document.addEventListener("DOMContentLoaded", () => {
  // DOM Elements - Navigation & Views
  const tabBtnDebug = document.getElementById("tab-btn-debug");
  const tabBtnStress = document.getElementById("tab-btn-stress");
  const viewDebugger = document.getElementById("view-debugger");
  const viewStress = document.getElementById("view-stress");

  // DOM Elements - Editor & Toolbar
  const codeEditor = document.getElementById("code-editor");
  const editorGutter = document.getElementById("editor-gutter");
  const editorOverlay = document.getElementById("editor-highlight-overlay");
  const editorLineCol = document.getElementById("editor-line-col");
  const btnRunDebug = document.getElementById("btn-run-debug");
  const btnInjectIc = document.getElementById("btn-inject-ic");
  const btnJumpError = document.getElementById("btn-jump-error");
  const btnCopyCode = document.getElementById("btn-copy-code");
  const btnLoadFile = document.getElementById("btn-load-file");
  const fileInputCpp = document.getElementById("file-input-cpp");
  const btnSaveFile = document.getElementById("btn-save-file");
  const selectSampleCode = document.getElementById("select-sample-code");
  const selectTimeout = document.getElementById("select-timeout");
  const checkSanitize = document.getElementById("check-sanitize");

  // DOM Elements - Inspector Tabs
  const inspTabBtns = document.querySelectorAll(".insp-tab-btn");
  const inspViews = document.querySelectorAll(".insp-view");

  // DOM Elements - Testcase IO
  const inputStdin = document.getElementById("input-stdin");
  const inputExpected = document.getElementById("input-expected");
  const outputStdout = document.getElementById("output-stdout");
  const btnClearInput = document.getElementById("btn-clear-input");
  const execTimePill = document.getElementById("exec-time-pill");

  // DOM Elements - Diagnosis
  const verdictBanner = document.getElementById("verdict-banner");
  const verdictBadge = document.getElementById("verdict-badge");
  const verdictMeta = document.getElementById("verdict-meta");
  const bugLineCard = document.getElementById("bug-line-card");
  const bugLineText = document.getElementById("bug-line-text");
  const btnJumpFromCard = document.getElementById("btn-jump-from-card");
  const diagnosisExplanationCard = document.getElementById("diagnosis-explanation-card");
  const diagTitle = document.getElementById("diag-title");
  const diagDesc = document.getElementById("diag-desc");
  const diagAdvice = document.getElementById("diag-advice");
  const diagAdviceBox = document.getElementById("diag-advice-box");
  const diagDetailBox = document.getElementById("diag-detail-box");
  const diagDetailPre = document.getElementById("diag-detail-pre");

  // DOM Elements - Diff
  const diffStatusBox = document.getElementById("diff-status-box");
  const diffContainer = document.getElementById("diff-container");
  const diffFirstDiff = document.getElementById("diff-first-diff");
  const diffExpected = document.getElementById("diff-expected");
  const diffActual = document.getElementById("diff-actual");

  // DOM Elements - Trace Timeline
  const traceBadgeCount = document.getElementById("trace-badge-count");
  const traceFilterInput = document.getElementById("trace-filter-input");
  const traceStats = document.getElementById("trace-stats");
  const traceWarningContainer = document.getElementById("trace-warning-container");
  const traceTableBody = document.getElementById("trace-table-body");

  // DOM Elements - Raw Log
  const outputStderr = document.getElementById("output-stderr");
  const btnCopyStderr = document.getElementById("btn-copy-stderr");

  // DOM Elements - Stress Test
  const btnStartStress = document.getElementById("btn-start-stress");
  const btnStopStress = document.getElementById("btn-stop-stress");
  const btnLoadStressSample = document.getElementById("btn-load-stress-sample");
  const selectStressIters = document.getElementById("select-stress-iters");
  const selectStressTimeout = document.getElementById("select-stress-timeout");
  const checkStressStopFail = document.getElementById("check-stress-stop-fail");
  const stressProgressBar = document.getElementById("stress-progress-bar");
  const stressStatusText = document.getElementById("stress-status-text");
  const stressPassedCount = document.getElementById("stress-passed-count");
  const stressFailCard = document.getElementById("stress-fail-card");
  const stressFailTitle = document.getElementById("stress-fail-title");
  const btnTransferToDebug = document.getElementById("btn-transfer-to-debug");
  const stressFailInput = document.getElementById("stress-fail-input");
  const stressFailNaive = document.getElementById("stress-fail-naive");
  const stressFailSolver = document.getElementById("stress-fail-solver");
  const stressCodeSolver = document.getElementById("stress-code-solver");
  const stressCodeNaive = document.getElementById("stress-code-naive");
  const stressCodeGen = document.getElementById("stress-code-gen");
  const btnSyncFromDebugger = document.getElementById("btn-sync-from-debugger");

  // State
  let currentBugLine = null;
  let allTraceEvents = [];
  let isRunningDebug = false;
  let isRunningStress = false;

  const LINE_HEIGHT_PX = 23; // Matches CSS line-height

  // ==========================================================================
  // Sample Code Presets
  // ==========================================================================
  const SAMPLE_CODES = {
    sample_oob: {
      code: `#include <iostream>
#include <vector>

int main() {
    int n;
    if (!(std::cin >> n)) n = 5;
    std::vector<int> a(n);
    for (int i = 0; i < n; ++i) a[i] = (i + 1) * 10;

    // バグ: 1-indexed でループして配列外参照
    long long sum = 0;
    for (int i = 1; i <= n; ++i) {
        sum += a[i]; // i = n のときに境界外アクセス
    }

    std::cout << "Sum: " << sum << std::endl;
    return 0;
}
`,
      input: "5\n",
      expected: "Sum: 150\n"
    },
    sample_div0: {
      code: `#include <iostream>
#include <vector>

int main() {
    int total = 100;
    int count = 0; // 条件により 0 のまま残ったケース

    // バグ: 0除算
    int average = total / count;

    std::cout << "Average: " << average << std::endl;
    return 0;
}
`,
      input: "",
      expected: ""
    },
    sample_overflow: {
      code: `#include <iostream>

int main() {
    // バグ: int (32bit) での累積積計算によるオーバーフロー
    int product = 1;
    for (int i = 1; i <= 15; ++i) {
        product *= i;
    }

    std::cout << "15! = " << product << std::endl;
    return 0;
}
`,
      input: "",
      expected: ""
    },
    sample_loop: {
      code: `#include <iostream>
#include <vector>

void solve() {
    int n = 5;
    int i = 0;
    long long total = 0;

    // バグ: i のインクリメントを忘れて無限ループ
    while (i < n) {
        total += i * 10;
        // i++; を書き忘れている！
    }

    std::cout << "Total: " << total << std::endl;
}

int main() {
    solve();
    return 0;
}
`,
      input: "",
      expected: "Total: 100\n"
    },
    sample_trace: {
      code: `#include <iostream>
#include <vector>
#include <mylib/icecream.hpp>

int main() {
    IC_CONFIG.include_context(true);

    int n = 5;
    std::vector<int> dp(n + 1, 0);
    dp[0] = 1;

    for (int i = 1; i <= n; ++i) {
        dp[i] = dp[i - 1] * 2;
        IC(i, dp[i]); // icecream で途中経過をトレース
    }

    std::cout << dp[n] << std::endl;
    return 0;
}
`,
      input: "",
      expected: "32\n"
    },
    sample_acl: {
      code: `#include <iostream>
#include <atcoder/all>

int main() {
    atcoder::dsu d(5);
    d.merge(0, 1);
    d.merge(1, 2);

    std::cout << (d.same(0, 2) ? "Yes" : "No") << std::endl;
    std::cout << (d.same(0, 3) ? "Yes" : "No") << std::endl;
    return 0;
}
`,
      input: "",
      expected: "Yes\nNo\n"
    }
  };

  // ==========================================================================
  // Editor Management (Gutter & Line Numbers)
  // ==========================================================================
  function updateGutter() {
    const lines = codeEditor.value.split("\n");
    const count = lines.length;
    let gutterHtml = "";

    for (let i = 1; i <= count; i++) {
      const isErr = i === currentBugLine;
      const errBadge = isErr ? '<span class="gutter-error-icon">⚠️</span>' : "";
      gutterHtml += `<div class="gutter-line ${isErr ? 'error' : ''}" data-line="${i}">${errBadge}${i}</div>`;
    }
    editorGutter.innerHTML = gutterHtml;
    updateErrorOverlay();
  }

  function updateErrorOverlay() {
    if (currentBugLine && currentBugLine > 0) {
      const topOffset = (currentBugLine - 1) * LINE_HEIGHT_PX;
      editorOverlay.innerHTML = `<div class="highlight-error-line" style="top: ${topOffset}px;"></div>`;
    } else {
      editorOverlay.innerHTML = "";
    }
  }

  function syncScroll() {
    editorGutter.scrollTop = codeEditor.scrollTop;
    editorOverlay.style.transform = `translateY(-${codeEditor.scrollTop}px)`;
  }

  function updateCursorPos() {
    const text = codeEditor.value.substring(0, codeEditor.selectionStart);
    const lines = text.split("\n");
    const line = lines.length;
    const col = lines[lines.length - 1].length + 1;
    editorLineCol.textContent = `Line ${line}, Col ${col}`;
  }

  function jumpToLine(lineNo) {
    if (!lineNo || lineNo <= 0) return;
    const lines = codeEditor.value.split("\n");
    if (lineNo > lines.length) return;

    let charPos = 0;
    for (let i = 0; i < lineNo - 1; i++) {
      charPos += lines[i].length + 1;
    }

    codeEditor.focus();
    codeEditor.setSelectionRange(charPos, charPos);

    // Scroll line to center
    const targetScroll = (lineNo - 3) * LINE_HEIGHT_PX;
    codeEditor.scrollTop = Math.max(0, targetScroll);
    syncScroll();
  }

  // Handle Tab key in editor
  codeEditor.addEventListener("keydown", (e) => {
    if (e.key === "Tab") {
      e.preventDefault();
      const start = codeEditor.selectionStart;
      const end = codeEditor.selectionEnd;
      codeEditor.value = codeEditor.value.substring(0, start) + "    " + codeEditor.value.substring(end);
      codeEditor.selectionStart = codeEditor.selectionEnd = start + 4;
      updateGutter();
      updateCursorPos();
    } else if ((e.ctrlKey || e.metaKey) && e.key === "Enter") {
      e.preventDefault();
      runDebug();
    }
  });

  codeEditor.addEventListener("input", () => {
    updateGutter();
    updateCursorPos();
  });

  codeEditor.addEventListener("scroll", syncScroll);
  codeEditor.addEventListener("click", updateCursorPos);
  codeEditor.addEventListener("keyup", updateCursorPos);

  // Gutter line click to jump/inspect
  editorGutter.addEventListener("click", (e) => {
    const lineElem = e.target.closest(".gutter-line");
    if (lineElem) {
      const lineNo = parseInt(lineElem.getAttribute("data-line"), 10);
      jumpToLine(lineNo);
    }
  });

  // Jump button handlers
  btnJumpError.addEventListener("click", () => jumpToLine(currentBugLine));
  btnJumpFromCard.addEventListener("click", () => jumpToLine(currentBugLine));

  // Copy code button
  btnCopyCode.addEventListener("click", () => {
    navigator.clipboard.writeText(codeEditor.value).then(() => {
      btnCopyCode.textContent = "✅";
      setTimeout(() => { btnCopyCode.textContent = "📋"; }, 1500);
    });
  });

  // Copy stderr button
  btnCopyStderr.addEventListener("click", () => {
    navigator.clipboard.writeText(outputStderr.textContent).then(() => {
      btnCopyStderr.textContent = "コピー完了";
      setTimeout(() => { btnCopyStderr.textContent = "コピー"; }, 1500);
    });
  });

  // Clear stdin
  btnClearInput.addEventListener("click", () => {
    inputStdin.value = "";
    inputStdin.focus();
  });

  // ==========================================================================
  // Sample Code & File Loading
  // ==========================================================================
  selectSampleCode.addEventListener("change", () => {
    const val = selectSampleCode.value;
    if (val && SAMPLE_CODES[val]) {
      const sample = SAMPLE_CODES[val];
      codeEditor.value = sample.code;
      inputStdin.value = sample.input;
      inputExpected.value = sample.expected;
      clearDiagnosis();
      updateGutter();
      updateCursorPos();
      selectSampleCode.value = "";
    }
  });

  btnLoadFile.addEventListener("click", () => fileInputCpp.click());
  fileInputCpp.addEventListener("change", (e) => {
    const file = e.target.files[0];
    if (file) {
      const reader = new FileReader();
      reader.onload = (evt) => {
        codeEditor.value = evt.target.result;
        clearDiagnosis();
        updateGutter();
        updateCursorPos();
      };
      reader.readAsText(file);
    }
  });

  // Drag and drop file to editor
  codeEditor.addEventListener("dragover", (e) => e.preventDefault());
  codeEditor.addEventListener("drop", (e) => {
    e.preventDefault();
    if (e.dataTransfer.files.length > 0) {
      const file = e.dataTransfer.files[0];
      const reader = new FileReader();
      reader.onload = (evt) => {
        codeEditor.value = evt.target.result;
        clearDiagnosis();
        updateGutter();
        updateCursorPos();
      };
      reader.readAsText(file);
    }
  });

  // Save file
  btnSaveFile.addEventListener("click", () => {
    const blob = new Blob([codeEditor.value], { type: "text/x-c++src;charset=utf-8" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = "solution.cpp";
    a.click();
    URL.revokeObjectURL(url);
  });

  // Auto-inject IceCream
  btnInjectIc.addEventListener("click", async () => {
    try {
      const res = await fetch("/api/inject_trace", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ code: codeEditor.value })
      });
      const data = await res.json();
      if (data.code) {
        codeEditor.value = data.code;
        updateGutter();
        btnInjectIc.textContent = "✅ 挿入完了";
        setTimeout(() => { btnInjectIc.textContent = "🍦 ic 挿入"; }, 1500);
      }
    } catch (err) {
      console.error(err);
    }
  });

  // ==========================================================================
  // Tab Switching (Views & Inspector)
  // ==========================================================================
  tabBtnDebug.addEventListener("click", () => {
    tabBtnDebug.classList.add("active");
    tabBtnStress.classList.remove("active");
    viewDebugger.classList.add("active");
    viewStress.classList.remove("active");
    updateGutter();
  });

  tabBtnStress.addEventListener("click", () => {
    tabBtnStress.classList.add("active");
    tabBtnDebug.classList.remove("active");
    viewStress.classList.add("active");
    viewDebugger.classList.remove("active");
  });

  inspTabBtns.forEach((btn) => {
    btn.addEventListener("click", () => {
      const targetId = btn.getAttribute("data-tab");
      switchInspectorTab(targetId);
    });
  });

  function switchInspectorTab(targetId) {
    inspTabBtns.forEach((b) => b.classList.toggle("active", b.getAttribute("data-tab") === targetId));
    inspViews.forEach((v) => v.classList.toggle("active", v.id === targetId));
  }

  // ==========================================================================
  // Debug Execution & Diagnosis
  // ==========================================================================
  function clearDiagnosis() {
    currentBugLine = null;
    btnJumpError.style.display = "none";
    bugLineCard.style.display = "none";
    diagnosisExplanationCard.style.display = "none";
    diffContainer.style.display = "none";
    traceWarningContainer.style.display = "none";
    execTimePill.style.display = "none";

    verdictBadge.className = "verdict-badge ready";
    verdictBadge.textContent = "READY";
    verdictMeta.textContent = "コードを実行すると診断結果が表示されます";

    outputStdout.textContent = "実行結果がここに表示されます...";
    outputStderr.textContent = "stderr の生出力がここに表示されます...";
    traceBadgeCount.textContent = "0";
    traceTableBody.innerHTML = `<tr><td colspan="4" class="empty-cell">実行待機中...</td></tr>`;
    diffStatusBox.textContent = "期待される出力と実際の出力の差異を比較します。";
    updateErrorOverlay();
    updateGutter();
  }

  async function runDebug() {
    if (isRunningDebug) return;
    const code = codeEditor.value.trim();
    if (!code) {
      alert("C++ コードを入力してください。");
      return;
    }

    isRunningDebug = true;
    btnRunDebug.disabled = true;
    btnRunDebug.textContent = "⏳ 実行・解析中...";
    clearDiagnosis();

    const payload = {
      code: codeEditor.value,
      input: inputStdin.value,
      expected_output: inputExpected.value,
      timeout_sec: parseFloat(selectTimeout.value),
      sanitize: checkSanitize.checked
    };

    try {
      const response = await fetch("/api/debug", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(payload)
      });
      const data = await response.json();
      renderDebugResult(data);
    } catch (err) {
      console.error(err);
      alert("サーバ通信エラーが発生しました。");
    } finally {
      isRunningDebug = false;
      btnRunDebug.disabled = false;
      btnRunDebug.textContent = "▶ 実行 & バグ解析";
    }
  }

  btnRunDebug.addEventListener("click", runDebug);

  function renderDebugResult(data) {
    const { verdict, time_ms, bug_line, bug_col, diagnosis, diff, stdout, stderr, traces, warnings } = data;

    // 1. Stdout & Stderr
    outputStdout.textContent = stdout || "(出力なし)";
    outputStderr.textContent = stderr || "(標準エラー出力なし)";

    if (time_ms !== undefined) {
      execTimePill.textContent = `${time_ms} ms`;
      execTimePill.style.display = "inline-block";
    }

    // 2. Verdict Badge
    verdictBadge.className = `verdict-badge ${verdict.toLowerCase()}`;
    verdictBadge.textContent = verdict;

    // 3. Bug Line & Gutter Indicator
    if (bug_line && bug_line > 0) {
      currentBugLine = bug_line;
      btnJumpError.style.display = "inline-flex";
      btnJumpError.textContent = `🎯 Line ${bug_line} へジャンプ`;

      bugLineCard.style.display = "flex";
      const colStr = bug_col ? `:${bug_col}` : "";
      bugLineText.textContent = `バグ発生箇所: ${bug_line} 行目 ${colStr}`;
      updateGutter();
      updateErrorOverlay();
    } else {
      currentBugLine = null;
      btnJumpError.style.display = "none";
      bugLineCard.style.display = "none";
      updateGutter();
      updateErrorOverlay();
    }

    // 4. Diagnosis Card
    if (diagnosis) {
      verdictMeta.textContent = diagnosis.description || "";
      diagnosisExplanationCard.style.display = "flex";
      diagTitle.textContent = diagnosis.title || verdict;
      diagDesc.textContent = diagnosis.description || "";

      if (diagnosis.advice) {
        diagAdviceBox.style.display = "block";
        diagAdvice.textContent = diagnosis.advice;
      } else {
        diagAdviceBox.style.display = "none";
      }

      if (diagnosis.detail) {
        diagDetailBox.style.display = "block";
        diagDetailPre.textContent = diagnosis.detail;
      } else {
        diagDetailBox.style.display = "none";
      }
    }

    // 5. Output Diff
    if (diff && !diff.matched) {
      diffContainer.style.display = "flex";
      diffStatusBox.textContent = `❌ 出力不一致 (WA) を検出しました。`;
      diffFirstDiff.innerHTML = `
        <strong>最初の不一致:</strong> 出力 ${diff.first_diff_line} 行目 / トークン #${diff.first_diff_token}<br>
        期待値: <code>${escapeHtml(diff.expected_token)}</code> ≠ 実際: <code>${escapeHtml(diff.actual_token)}</code>
      `;
      diffExpected.textContent = (diff.expected_lines || []).join("\n");
      diffActual.textContent = (diff.actual_lines || []).join("\n");
    } else {
      diffContainer.style.display = "none";
      if (verdict === "AC" && inputExpected.value.trim() !== "") {
        diffStatusBox.textContent = `✅ 期待される出力と完全一致しました！`;
      } else {
        diffStatusBox.textContent = `期待される出力を入力すると、Diff比較が表示されます。`;
      }
    }

    // 6. Trace Events & Timeline
    allTraceEvents = traces || [];
    traceBadgeCount.textContent = allTraceEvents.length;
    renderTraces(allTraceEvents);

    // 7. Suspicious Warnings
    if (warnings && warnings.length > 0) {
      traceWarningContainer.style.display = "flex";
      traceWarningContainer.innerHTML = warnings.map(w => `<div class="warning-item">⚠️ ${escapeHtml(w)}</div>`).join("");
    } else {
      traceWarningContainer.style.display = "none";
    }

    // Auto tab selection based on verdict
    if (verdict === "RE" || verdict === "CE" || verdict === "TLE") {
      switchInspectorTab("tab-diagnosis");
      if (bug_line) {
        jumpToLine(bug_line);
      }
    } else if (verdict === "WA") {
      switchInspectorTab("tab-diff");
    } else if (allTraceEvents.length > 0) {
      // If AC with traces, keep testcase or stay
    }
  }

  // ==========================================================================
  // Trace Rendering & Filtering
  // ==========================================================================
  function renderTraces(events) {
    const filter = traceFilterInput.value.trim().toLowerCase();
    const filtered = events.filter((ev) => {
      if (!filter) return true;
      if (ev.function && ev.function.toLowerCase().includes(filter)) return true;
      return ev.items && ev.items.some(it => it.name.toLowerCase().includes(filter) || it.value.toLowerCase().includes(filter));
    });

    traceStats.textContent = `${filtered.length} / ${events.length} 件のトレース`;

    if (filtered.length === 0) {
      traceTableBody.innerHTML = `<tr><td colspan="4" class="empty-cell">${events.length === 0 ? "トレース出力はありません。IC(...) を埋め込むとここに表示されます。" : "該当するトレースはありません。"}</td></tr>`;
      return;
    }

    let rowsHtml = "";
    filtered.forEach((ev) => {
      const lineBadge = ev.line ? `<span class="trace-line-badge">Line ${ev.line}</span>` : "-";
      const varsHtml = (ev.items || []).map(it => {
        const nameSpan = it.name ? `<span class="trace-var-name">${escapeHtml(it.name)}</span> = ` : "";
        return `<span class="trace-var-item">${nameSpan}<span class="trace-var-val">${escapeHtml(it.value)}</span></span>`;
      }).join(", ");

      rowsHtml += `
        <tr class="trace-row" data-line="${ev.line || ''}" title="クリックでこの行へジャンプ">
          <td>#${ev.step}</td>
          <td>${lineBadge}</td>
          <td><code>${escapeHtml(ev.function || '-')}</code></td>
          <td>${varsHtml || escapeHtml(ev.raw)}</td>
        </tr>
      `;
    });

    traceTableBody.innerHTML = rowsHtml;

    // Trace row click to jump
    traceTableBody.querySelectorAll(".trace-row").forEach(row => {
      row.addEventListener("click", () => {
        const line = parseInt(row.getAttribute("data-line"), 10);
        if (line) jumpToLine(line);
      });
    });
  }

  traceFilterInput.addEventListener("input", () => {
    renderTraces(allTraceEvents);
  });

  // ==========================================================================
  // Tab 2: Stress Test Studio
  // ==========================================================================
  const STRESS_SAMPLE = {
    solver: `#include <iostream>
#include <vector>

int main() {
    int n;
    if (!(std::cin >> n)) return 0;
    std::vector<int> a(n);
    for (int i = 0; i < n; ++i) std::cin >> a[i];

    // 検証解 (バグあり): a[i] == 3 のときに足し忘れる
    long long ans = 0;
    for (int i = 0; i < n; ++i) {
        if (a[i] == 3) continue; // バグ！
        ans += a[i];
    }

    std::cout << ans << std::endl;
    return 0;
}
`,
    naive: `#include <iostream>
#include <vector>

int main() {
    int n;
    if (!(std::cin >> n)) return 0;
    std::vector<int> a(n);
    for (int i = 0; i < n; ++i) std::cin >> a[i];

    // 愚直解 (正解保証): 単純な総和
    long long ans = 0;
    for (int x : a) ans += x;

    std::cout << ans << std::endl;
    return 0;
}
`,
    gen: `#include <iostream>
#include <random>

int main() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist_n(1, 6);
    std::uniform_int_distribution<int> dist_val(1, 5);

    int n = dist_n(mt);
    std::cout << n << "\\n";
    for (int i = 0; i < n; ++i) {
        std::cout << dist_val(mt) << (i + 1 == n ? "" : " ");
    }
    std::cout << "\\n";
    return 0;
}
`
  };

  btnLoadStressSample.addEventListener("click", () => {
    stressCodeSolver.value = STRESS_SAMPLE.solver;
    stressCodeNaive.value = STRESS_SAMPLE.naive;
    stressCodeGen.value = STRESS_SAMPLE.gen;
    stressFailCard.style.display = "none";
    stressProgressBar.style.width = "0%";
    stressStatusText.textContent = "サンプルコードを読み込みました。「ストレステスト開始」を押してください。";
    stressPassedCount.textContent = "0 / 0 完了";
  });

  btnSyncFromDebugger.addEventListener("click", () => {
    stressCodeSolver.value = codeEditor.value;
    btnSyncFromDebugger.textContent = "反映完了";
    setTimeout(() => { btnSyncFromDebugger.textContent = "デバッガから取込"; }, 1500);
  });

  async function startStressTest() {
    if (isRunningStress) return;

    const solver = stressCodeSolver.value.trim();
    const naive = stressCodeNaive.value.trim();
    const gen = stressCodeGen.value.trim();

    if (!solver || !naive || !gen) {
      alert("Solver, Naive, Generator のすべてのコードを入力してください。");
      return;
    }

    isRunningStress = true;
    btnStartStress.disabled = true;
    btnStopStress.disabled = false;
    stressFailCard.style.display = "none";
    stressProgressBar.style.width = "30%";
    stressProgressBar.style.background = "linear-gradient(90deg, var(--accent-blue), var(--accent-cyan))";
    stressStatusText.textContent = "コンパイル中 & ストレステスト実行中...";

    const iters = parseInt(selectStressIters.value, 10);
    const timeout = parseFloat(selectStressTimeout.value);
    const stopOnFail = checkStressStopFail.checked;

    try {
      const response = await fetch("/api/stress_test", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          solver_code: solver,
          naive_code: naive,
          generator_code: gen,
          iterations: iters,
          timeout_sec: timeout,
          stop_on_fail: stopOnFail
        })
      });
      const data = await response.json();
      renderStressResult(data, iters);
    } catch (err) {
      console.error(err);
      stressStatusText.textContent = "ストレステスト実行中に通信エラーが発生しました。";
    } finally {
      isRunningStress = false;
      btnStartStress.disabled = false;
      btnStopStress.disabled = true;
    }
  }

  btnStartStress.addEventListener("click", startStressTest);

  function renderStressResult(data, targetIters) {
    if (data.error) {
      stressProgressBar.style.width = "100%";
      stressProgressBar.style.background = "var(--accent-rose)";
      stressStatusText.textContent = `❌ エラー: ${data.error}`;
      return;
    }

    if (data.success) {
      stressProgressBar.style.width = "100%";
      stressProgressBar.style.background = "var(--accent-emerald)";
      stressStatusText.textContent = `🎉 全 ${data.total_run} ケース合格！差異やエラーは検出されませんでした。`;
      stressPassedCount.textContent = `${data.passed_count} / ${data.total_run} 完了`;
      stressFailCard.style.display = "none";
    } else {
      // Failure detected
      const fail = data.failed_case;
      stressProgressBar.style.width = "100%";
      stressProgressBar.style.background = "var(--accent-rose)";
      stressStatusText.textContent = `🚨 ケース #${fail.iteration} でバグ (${fail.verdict}) を検出しました！`;
      stressPassedCount.textContent = `${data.passed_count} / ${targetIters} 合格`;

      stressFailCard.style.display = "flex";
      stressFailTitle.textContent = fail.error_summary || `ケース #${fail.iteration} で不一致を検出！`;
      stressFailInput.textContent = fail.input || "";
      stressFailNaive.textContent = fail.naive_output || "";
      stressFailSolver.textContent = fail.solver_output || fail.solver_stderr || "";

      // Wire Transfer to Debugger button
      btnTransferToDebug.onclick = () => {
        // Load solver code
        codeEditor.value = stressCodeSolver.value;
        // Load failing input
        inputStdin.value = fail.input || "";
        // Load expected naive output
        inputExpected.value = fail.naive_output || "";

        // Switch to Debugger tab
        tabBtnDebug.click();

        // Run Debugger diagnosis immediately!
        runDebug();
      };
    }
  }

  // ==========================================================================
  // Utility
  // ==========================================================================
  function escapeHtml(str) {
    if (!str) return "";
    return String(str)
      .replace(/&/g, "&amp;")
      .replace(/</g, "&lt;")
      .replace(/>/g, "&gt;")
      .replace(/"/g, "&quot;")
      .replace(/'/g, "&#039;");
  }

  // Initial code template
  codeEditor.value = `#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }

    long long ans = 0;
    for (int x : a) ans += x;

    cout << ans << "\\n";
    return 0;
}
`;
  inputStdin.value = "3\n10 20 30\n";
  inputExpected.value = "60\n";

  // Initialize Stress Test with sample
  stressCodeSolver.value = STRESS_SAMPLE.solver;
  stressCodeNaive.value = STRESS_SAMPLE.naive;
  stressCodeGen.value = STRESS_SAMPLE.gen;

  updateGutter();
  updateCursorPos();

  // Check for transferred testcase from Stress Tester
  try {
    const transCode = localStorage.getItem("cpd_transfer_code");
    const transIn = localStorage.getItem("cpd_transfer_input");
    const transExp = localStorage.getItem("cpd_transfer_expected");
    if (transCode || transIn || transExp) {
      if (transCode) codeEditor.value = transCode;
      if (transIn) inputStdin.value = transIn;
      if (transExp) inputExpected.value = transExp;
      localStorage.removeItem("cpd_transfer_code");
      localStorage.removeItem("cpd_transfer_input");
      localStorage.removeItem("cpd_transfer_expected");
      updateGutter();
      updateCursorPos();
      setTimeout(runDebug, 200);
    }
  } catch (e) {}
});
