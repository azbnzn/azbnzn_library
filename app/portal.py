#!/usr/bin/env python3
"""
AtCoder Apps Portal (app/portal.py)

On-demand lightweight web hub for launching and managing AtCoder GUI/CLI tools:
- Zero idle overhead when closed (starts only when launched, exits cleanly on shutdown).
- Lazy starts heavy Python backend servers only when opened, avoiding port/resource waste.
- Serves static HTML/CSS/JS apps safely under /static/<app_id>/.
- Writes runtime state to /tmp/atcoder_apps_portal.json for native launcher discovery.
"""

import argparse
import atexit
import html
import json
import mimetypes
import os
import posixpath
import signal
import socket
import subprocess
import sys
import threading
import time
import urllib.parse
import webbrowser
from http.server import BaseHTTPRequestHandler, HTTPServer
from socketserver import ThreadingMixIn

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
if BASE_DIR not in sys.path:
    sys.path.insert(0, BASE_DIR)

from run import APPLICATIONS  # noqa: E402

STATE_FILE = "/tmp/atcoder_apps_portal.json"
DEFAULT_PORTAL_PORT = 8000
MAX_PORTAL_PORT = 8020

PREFERRED_PATHS = [
    "/Library/Frameworks/Python.framework/Versions/3.13/bin",
    "/opt/homebrew/bin",
    "/usr/local/bin",
    "/usr/bin",
    "/bin",
    "/usr/sbin",
    "/sbin",
]

DEFAULT_APP_PORTS = {
    "cp_debugger": 8080,
    "stress_tester": 8085,
}

APP_META = {
    "grid_visualizer": {"icon": "🟩", "badge": "静的アプリ", "order": 1},
    "array_graph": {"icon": "🕸️", "badge": "静的アプリ", "order": 2},
    "cp_debugger": {"icon": "⚡️", "badge": "Pythonサーバー", "order": 3},
    "stress_tester": {"icon": "🚀", "badge": "Pythonサーバー", "order": 4},
}


def build_rich_env() -> dict:
    """Build environment dict preserving PATH plus Homebrew / Framework Python / Xcode tools."""
    env = os.environ.copy()
    existing = [p for p in env.get("PATH", "").split(":") if p]
    merged = []
    for p in PREFERRED_PATHS + existing:
        if p not in merged:
            merged.append(p)
    env["PATH"] = ":".join(merged)
    return env


def ensure_rich_process_path() -> None:
    """Update current process PATH so subprocesses inherit compiler/debugger tools."""
    os.environ["PATH"] = build_rich_env()["PATH"]


def is_port_free(port: int) -> bool:
    """Check that 127.0.0.1:port neither has an active listener nor fails a bind test."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as probe:
        probe.settimeout(0.15)
        if probe.connect_ex(("127.0.0.1", port)) == 0:
            return False
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        try:
            sock.bind(("127.0.0.1", port))
            return True
        except OSError:
            return False


def find_bindable_port(start_port: int, end_port: int, exclude: set[int] | None = None) -> int:
    """Find first bindable TCP port on 127.0.0.1 excluding reserved ports."""
    excluded = exclude or set()
    for port in range(start_port, end_port + 1):
        if port in excluded:
            continue
        if is_port_free(port):
            return port
    raise RuntimeError(f"空きポートが見つかりませんでした ({start_port}-{end_port})")


class AppManager:
    """Thread-safe lazy lifecycle manager for static and Python-backed apps."""

    def __init__(self, portal_port: int):
        self.portal_port = portal_port
        self._lock = threading.RLock()
        self._apps_by_id = {app["id"]: app for app in APPLICATIONS}
        self._static_roots = {}
        for app in APPLICATIONS:
            if app["type"] == "browser":
                self._static_roots[app["id"]] = os.path.dirname(os.path.realpath(app["target"]))
        # id -> {"proc": Popen, "port": int, "log_path": str, "log_file": file, "started_at": float}
        self._running: dict[str, dict] = {}
        self._env = build_rich_env()

    def _prune_dead_locked(self) -> None:
        dead_ids = []
        for app_id, entry in self._running.items():
            proc = entry["proc"]
            if proc.poll() is not None:
                dead_ids.append(app_id)
        for app_id in dead_ids:
            entry = self._running.pop(app_id)
            self._close_log_quietly(entry.get("log_file"))

    @staticmethod
    def _close_log_quietly(log_file) -> None:
        if log_file is not None:
            try:
                log_file.close()
            except OSError:
                pass

    def _reserved_ports_locked(self) -> set[int]:
        ports = {self.portal_port}
        for entry in self._running.values():
            ports.add(entry["port"])
        return ports

    def static_root_for(self, app_id: str) -> str | None:
        return self._static_roots.get(app_id)

    def static_url_for(self, app_id: str) -> str:
        rel_target = os.path.relpath(
            os.path.realpath(self._apps_by_id[app_id]["target"]),
            self._static_roots[app_id],
        ).replace(os.sep, "/")
        return f"http://127.0.0.1:{self.portal_port}/static/{app_id}/{rel_target}"

    def list_status(self) -> dict:
        with self._lock:
            self._prune_dead_locked()
            items = []
            running_servers = 0
            total_servers = 0

            ordered_apps = sorted(
                APPLICATIONS,
                key=lambda a: (APP_META.get(a["id"], {}).get("order", 99), a["name"]),
            )
            for app in ordered_apps:
                app_id = app["id"]
                meta = APP_META.get(app_id, {"icon": "🧩", "badge": app["type"]})
                if app["type"] == "python":
                    total_servers += 1
                    entry = self._running.get(app_id)
                    is_running = entry is not None and entry["proc"].poll() is None
                    if is_running:
                        running_servers += 1
                        url = f"http://127.0.0.1:{entry['port']}"
                        pid = entry["proc"].pid
                        port = entry["port"]
                    else:
                        url = None
                        pid = None
                        port = None
                else:
                    is_running = True
                    url = self.static_url_for(app_id)
                    pid = None
                    port = self.portal_port

                items.append({
                    "id": app_id,
                    "name": app["name"],
                    "description": app["description"],
                    "type": app["type"],
                    "icon": meta["icon"],
                    "badge": meta["badge"],
                    "running": is_running,
                    "port": port,
                    "pid": pid,
                    "url": url,
                })

            return {
                "ok": True,
                "portal": {
                    "pid": os.getpid(),
                    "port": self.portal_port,
                    "url": f"http://127.0.0.1:{self.portal_port}",
                },
                "running_servers": running_servers,
                "total_servers": total_servers,
                "apps": items,
            }

    def launch(self, app_id: str) -> tuple[int, dict]:
        if app_id not in self._apps_by_id:
            return 404, {"ok": False, "error": f"Unknown app: {app_id}"}

        app = self._apps_by_id[app_id]
        if app["type"] == "browser":
            return 200, {
                "ok": True,
                "id": app_id,
                "type": "browser",
                "url": self.static_url_for(app_id),
                "already_running": True,
            }

        with self._lock:
            self._prune_dead_locked()
            existing = self._running.get(app_id)
            if existing is not None and existing["proc"].poll() is None:
                return 200, {
                    "ok": True,
                    "id": app_id,
                    "type": "python",
                    "port": existing["port"],
                    "pid": existing["proc"].pid,
                    "url": f"http://127.0.0.1:{existing['port']}",
                    "already_running": True,
                }

            target = app["target"]
            if not os.path.isfile(target):
                return 500, {"ok": False, "error": f"起動対象が見つかりません: {target}"}

            preferred_port = DEFAULT_APP_PORTS.get(app_id, 8090)
            reserved = self._reserved_ports_locked()
            try:
                port = find_bindable_port(preferred_port, 8099, exclude=reserved)
            except RuntimeError:
                port = find_bindable_port(8100, 8299, exclude=reserved)

            log_path = f"/tmp/atcoder_app_{app_id}.log"
            log_file = open(log_path, "w", encoding="utf-8", buffering=1)
            cmd = [sys.executable, target, "--no-browser", "--port", str(port)]

            try:
                proc = subprocess.Popen(
                    cmd,
                    cwd=os.path.dirname(target),
                    env=self._env,
                    stdin=subprocess.DEVNULL,
                    stdout=log_file,
                    stderr=subprocess.STDOUT,
                    start_new_session=True,
                )
            except Exception as exc:
                self._close_log_quietly(log_file)
                return 500, {"ok": False, "error": f"プロセス起動に失敗しました: {exc}"}

            deadline = time.monotonic() + 5.0
            ready = False
            while time.monotonic() < deadline:
                if proc.poll() is not None:
                    break
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as probe:
                    probe.settimeout(0.1)
                    if probe.connect_ex(("127.0.0.1", port)) == 0:
                        ready = True
                        break
                time.sleep(0.05)

            if not ready or proc.poll() is not None:
                self._terminate_proc_group(proc)
                self._close_log_quietly(log_file)
                tail = ""
                try:
                    with open(log_path, "r", encoding="utf-8", errors="replace") as f:
                        tail = f.read()[-600:].strip()
                except OSError:
                    pass
                return 500, {
                    "ok": False,
                    "error": f"{app['name']} のサーバー起動がタイムアウトまたは終了しました。",
                    "log_tail": tail,
                }

            self._running[app_id] = {
                "proc": proc,
                "port": port,
                "log_path": log_path,
                "log_file": log_file,
                "started_at": time.time(),
            }
            return 200, {
                "ok": True,
                "id": app_id,
                "type": "python",
                "port": port,
                "pid": proc.pid,
                "url": f"http://127.0.0.1:{port}",
                "already_running": False,
            }

    @staticmethod
    def _terminate_proc_group(proc: subprocess.Popen) -> None:
        if proc.poll() is not None:
            return
        try:
            pgid = os.getpgid(proc.pid)
        except ProcessLookupError:
            return
        except OSError:
            pgid = None

        try:
            if pgid is not None:
                os.killpg(pgid, signal.SIGTERM)
            else:
                proc.terminate()
        except ProcessLookupError:
            return
        except OSError:
            pass

        try:
            proc.wait(timeout=2.0)
            return
        except subprocess.TimeoutExpired:
            pass

        try:
            if pgid is not None:
                os.killpg(pgid, signal.SIGKILL)
            else:
                proc.kill()
        except ProcessLookupError:
            return
        except OSError:
            pass

        try:
            proc.wait(timeout=1.0)
        except subprocess.TimeoutExpired:
            pass

    def stop(self, app_id: str) -> tuple[int, dict]:
        if app_id not in self._apps_by_id:
            return 404, {"ok": False, "error": f"Unknown app: {app_id}"}
        app = self._apps_by_id[app_id]
        if app["type"] != "python":
            return 200, {"ok": True, "id": app_id, "stopped": False, "message": "静的アプリには停止するサーバーがありません。"}

        with self._lock:
            self._prune_dead_locked()
            entry = self._running.pop(app_id, None)
            if entry is None:
                return 200, {"ok": True, "id": app_id, "stopped": False}
            self._terminate_proc_group(entry["proc"])
            self._close_log_quietly(entry.get("log_file"))
            return 200, {"ok": True, "id": app_id, "stopped": True}

    def stop_all(self) -> list[str]:
        with self._lock:
            self._prune_dead_locked()
            stopped_ids = list(self._running.keys())
            for app_id in stopped_ids:
                entry = self._running.pop(app_id)
                self._terminate_proc_group(entry["proc"])
                self._close_log_quietly(entry.get("log_file"))
            return stopped_ids


def render_dashboard_html(initial_status: dict) -> str:
    initial_json = json.dumps(initial_status, ensure_ascii=False)
    portal_port = initial_status["portal"]["port"]
    return f"""<!DOCTYPE html>
<html lang="ja">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>AtCoder Apps Portal</title>
  <style>
    :root {{
      --bg: #0b1120;
      --surface: #111827;
      --surface-2: #1e293b;
      --border: #263248;
      --text: #f1f5f9;
      --muted: #94a3b8;
      --accent: #38bdf8;
      --accent-hover: #0ea5e9;
      --green: #22c55e;
      --green-bg: rgba(34, 197, 94, 0.14);
      --amber: #f59e0b;
      --red: #ef4444;
      --red-bg: rgba(239, 68, 68, 0.14);
    }}
    * {{ box-sizing: border-box; }}
    body {{
      margin: 0;
      min-height: 100vh;
      font-family: -apple-system, BlinkMacSystemFont, "SF Pro Text", "Hiragino Sans", "Noto Sans JP", sans-serif;
      background: radial-gradient(circle at top right, #172554 0%, var(--bg) 50%);
      color: var(--text);
      line-height: 1.5;
    }}
    .container {{
      max-width: 980px;
      margin: 0 auto;
      padding: 32px 20px 48px;
    }}
    header {{
      display: flex;
      flex-wrap: wrap;
      justify-content: space-between;
      align-items: center;
      gap: 16px;
      margin-bottom: 24px;
      padding-bottom: 20px;
      border-bottom: 1px solid var(--border);
    }}
    .brand {{
      display: flex;
      align-items: center;
      gap: 14px;
    }}
    .brand-logo {{
      width: 46px;
      height: 46px;
      border-radius: 12px;
      background: linear-gradient(135deg, #2563eb, #06b6d4);
      display: grid;
      place-items: center;
      font-size: 22px;
      box-shadow: 0 8px 24px rgba(37, 99, 235, 0.35);
      flex-shrink: 0;
    }}
    h1 {{
      margin: 0;
      font-size: 1.4rem;
      font-weight: 700;
      letter-spacing: -0.02em;
    }}
    .subtitle {{
      margin: 2px 0 0;
      font-size: 0.86rem;
      color: var(--muted);
    }}
    .header-actions {{
      display: flex;
      flex-wrap: wrap;
      align-items: center;
      gap: 10px;
    }}
    .pill {{
      display: inline-flex;
      align-items: center;
      gap: 7px;
      padding: 6px 12px;
      border-radius: 999px;
      background: var(--surface-2);
      border: 1px solid var(--border);
      font-size: 0.82rem;
      color: var(--text);
      font-weight: 500;
    }}
    .dot {{
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background: var(--muted);
    }}
    .dot.active {{
      background: var(--green);
      box-shadow: 0 0 8px var(--green);
    }}
    .btn {{
      border: 1px solid transparent;
      border-radius: 10px;
      padding: 9px 15px;
      font-size: 0.88rem;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.15s ease;
      display: inline-flex;
      align-items: center;
      justify-content: center;
      gap: 6px;
      font-family: inherit;
    }}
    .btn:disabled {{
      opacity: 0.55;
      cursor: not-allowed;
    }}
    .btn-primary {{
      background: linear-gradient(135deg, #0284c7, #2563eb);
      color: #fff;
    }}
    .btn-primary:hover:not(:disabled) {{
      filter: brightness(1.1);
      transform: translateY(-1px);
    }}
    .btn-ghost {{
      background: var(--surface-2);
      color: var(--text);
      border-color: var(--border);
    }}
    .btn-ghost:hover:not(:disabled) {{
      background: #283548;
    }}
    .btn-danger-outline {{
      background: var(--red-bg);
      color: #fca5a5;
      border-color: rgba(239, 68, 68, 0.35);
    }}
    .btn-danger-outline:hover:not(:disabled) {{
      background: rgba(239, 68, 68, 0.25);
      color: #fecaca;
    }}
    .info-bar {{
      display: flex;
      flex-wrap: wrap;
      align-items: center;
      justify-content: space-between;
      gap: 10px;
      background: rgba(17, 24, 39, 0.72);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 11px 16px;
      margin-bottom: 22px;
      font-size: 0.84rem;
      color: var(--muted);
    }}
    .grid {{
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(290px, 1fr));
      gap: 16px;
    }}
    .card {{
      background: rgba(17, 24, 39, 0.9);
      border: 1px solid var(--border);
      border-radius: 16px;
      padding: 20px;
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      gap: 16px;
      transition: border-color 0.15s ease, transform 0.15s ease;
    }}
    .card:hover {{
      border-color: #3b4f70;
    }}
    .card-top {{
      display: flex;
      justify-content: space-between;
      align-items: flex-start;
      gap: 12px;
    }}
    .card-title-row {{
      display: flex;
      align-items: center;
      gap: 10px;
    }}
    .app-icon {{
      font-size: 1.6rem;
      line-height: 1;
    }}
    .app-name {{
      margin: 0;
      font-size: 1.06rem;
      font-weight: 700;
    }}
    .app-type {{
      font-size: 0.75rem;
      color: var(--muted);
    }}
    .status-badge {{
      font-size: 0.75rem;
      font-weight: 600;
      padding: 4px 10px;
      border-radius: 999px;
      white-space: nowrap;
      border: 1px solid var(--border);
      background: var(--surface-2);
      color: var(--muted);
    }}
    .status-badge.running {{
      background: var(--green-bg);
      color: #86efac;
      border-color: rgba(34, 197, 94, 0.35);
    }}
    .status-badge.static {{
      background: rgba(56, 189, 248, 0.12);
      color: #7dd3fc;
      border-color: rgba(56, 189, 248, 0.3);
    }}
    .app-desc {{
      margin: 0;
      font-size: 0.88rem;
      color: #cbd5e1;
    }}
    .card-footer {{
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 10px;
      padding-top: 8px;
      border-top: 1px solid rgba(38, 50, 72, 0.65);
    }}
    .card-actions {{
      display: flex;
      gap: 8px;
      flex: 1;
    }}
    .card-actions .btn-primary {{
      flex: 1;
    }}
    .meta-port {{
      font-family: ui-monospace, SFMono-Regular, Menlo, monospace;
      font-size: 0.78rem;
      color: var(--muted);
    }}
    .toast {{
      position: fixed;
      right: 20px;
      bottom: 20px;
      max-width: 420px;
      padding: 12px 16px;
      border-radius: 12px;
      background: #1e293b;
      border: 1px solid #334155;
      color: var(--text);
      font-size: 0.88rem;
      box-shadow: 0 12px 30px rgba(0, 0, 0, 0.45);
      display: none;
      z-index: 100;
    }}
    .toast.show {{ display: block; }}
    .toast.error {{
      border-color: rgba(239, 68, 68, 0.6);
      background: #2b161b;
      color: #fecaca;
    }}
    .shutdown-banner {{
      display: none;
      text-align: center;
      padding: 48px 20px;
      background: var(--surface);
      border: 1px solid var(--border);
      border-radius: 16px;
    }}
    .shutdown-banner.show {{ display: block; }}
  </style>
</head>
<body>
  <div class="container">
    <header>
      <div class="brand">
        <div class="brand-logo">⚡</div>
        <div>
          <h1>AtCoder Apps Portal</h1>
          <p class="subtitle">省電力オンデマンド起動ランチャー（ポート {portal_port}）</p>
        </div>
      </div>
      <div class="header-actions">
        <span class="pill" id="server-summary-pill">
          <span class="dot" id="server-summary-dot"></span>
          <span id="server-summary-text">稼働中サーバー: 0 / 2</span>
        </span>
        <button class="btn btn-ghost" id="btn-stop-all" onclick="stopAll()">全アプリ停止</button>
        <button class="btn btn-danger-outline" id="btn-shutdown" onclick="shutdownPortal()">ポータルごと終了</button>
      </div>
    </header>

    <div class="info-bar">
      <span>各 Python ツールは「開く」を押した時だけ裏で起動します。待機中ツールの CPU・メモリ消費は 0 です。</span>
      <span id="last-updated">状態: 最新</span>
    </div>

    <div class="grid" id="apps-grid"></div>

    <div class="shutdown-banner" id="shutdown-banner">
      <h2>ポータルを終了しました</h2>
      <p style="color: var(--muted);">起動していたすべてのサーバーとポータルプロセスを停止しました（待機CPU・メモリ消費 0）。<br>再開する場合は Spotlight または <strong>AtCoder Apps.app</strong> を開いてください。</p>
    </div>
  </div>

  <div class="toast" id="toast"></div>

  <script>
    let currentState = {initial_json};
    let busyApps = new Set();
    let isShutDown = false;
    let pollTimer = null;

    function showToast(msg, isError = false) {{
      const el = document.getElementById('toast');
      el.textContent = msg;
      el.className = 'toast show' + (isError ? ' error' : '');
      clearTimeout(el._timer);
      el._timer = setTimeout(() => {{ el.className = 'toast'; }}, 4000);
    }}

    function escapeHtml(str) {{
      return String(str)
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;')
        .replace(/'/g, '&#39;');
    }}

    function render(state) {{
      currentState = state;
      const dot = document.getElementById('server-summary-dot');
      const summaryText = document.getElementById('server-summary-text');
      summaryText.textContent = `稼働中サーバー: ${{state.running_servers}} / ${{state.total_servers}}`;
      dot.className = 'dot' + (state.running_servers > 0 ? ' active' : '');

      const stopAllBtn = document.getElementById('btn-stop-all');
      stopAllBtn.disabled = state.running_servers === 0 || isShutDown;

      const grid = document.getElementById('apps-grid');
      grid.innerHTML = state.apps.map(app => {{
        const isPython = app.type === 'python';
        const isBusy = busyApps.has(app.id);
        let statusClass = 'status-badge';
        let statusLabel = '停止中 (省電力)';
        if (!isPython) {{
          statusClass += ' static';
          statusLabel = '常時利用可能';
        }} else if (app.running) {{
          statusClass += ' running';
          statusLabel = `稼働中 :${{app.port}}`;
        }}

        const openText = isBusy ? '起動中...' : '開く';
        const stopBtn = isPython
          ? `<button class="btn btn-ghost" ${{(!app.running || isBusy) ? 'disabled' : ''}} onclick="stopApp('${{escapeHtml(app.id)}}')">停止</button>`
          : '';
        const metaInfo = isPython
          ? (app.running ? `PID ${{app.pid}}` : '未起動')
          : '静的HTML';

        return `
          <div class="card">
            <div>
              <div class="card-top">
                <div class="card-title-row">
                  <span class="app-icon">${{escapeHtml(app.icon)}}</span>
                  <div>
                    <h2 class="app-name">${{escapeHtml(app.name)}}</h2>
                    <div class="app-type">${{escapeHtml(app.badge)}}</div>
                  </div>
                </div>
                <span class="${{statusClass}}">${{escapeHtml(statusLabel)}}</span>
              </div>
              <p class="app-desc" style="margin-top: 12px;">${{escapeHtml(app.description)}}</p>
            </div>
            <div class="card-footer">
              <div class="card-actions">
                <button class="btn btn-primary" ${{isBusy ? 'disabled' : ''}} onclick="launchApp('${{escapeHtml(app.id)}}', '${{escapeHtml(app.type)}}')">${{openText}}</button>
                ${{stopBtn}}
              </div>
              <span class="meta-port">${{escapeHtml(metaInfo)}}</span>
            </div>
          </div>
        `;
      }}).join('');
    }}

    async function refreshStatus() {{
      if (isShutDown) return;
      try {{
        const res = await fetch('/api/apps', {{ cache: 'no-store' }});
        if (!res.ok) return;
        const data = await res.json();
        render(data);
        const now = new Date().toLocaleTimeString('ja-JP');
        document.getElementById('last-updated').textContent = `最終確認: ${{now}}`;
      }} catch (_err) {{
        // Ignore transient network errors while polling
      }}
    }}

    async function launchApp(appId, appType) {{
      if (isShutDown || busyApps.has(appId)) return;

      // Open blank tab synchronously inside click handler to avoid popup blockers
      const newTab = window.open('about:blank', '_blank');
      if (newTab) {{
        newTab.document.write(
          '<!DOCTYPE html><html><head><meta charset="utf-8"><title>起動中...</title></head>' +
          '<body style="background:#0b1120;color:#e2e8f0;font-family:-apple-system,sans-serif;display:flex;align-items:center;justify-content:center;height:100vh;margin:0;">' +
          '<div style="text-align:center;"><div style="font-size:32px;margin-bottom:12px;">⚡</div>' +
          '<div style="font-size:17px;font-weight:600;">アプリを起動しています...</div>' +
          '<div style="font-size:13px;color:#94a3b8;margin-top:6px;">準備が完了次第、自動的に画面を開きます</div></div></body></html>'
        );
        newTab.document.close();
      }}

      busyApps.add(appId);
      render(currentState);

      try {{
        const res = await fetch(`/api/launch/${{encodeURIComponent(appId)}}`, {{ method: 'POST' }});
        const data = await res.json();
        if (!res.ok || !data.ok) {{
          if (newTab && !newTab.closed) newTab.close();
          showToast(data.error || '起動に失敗しました', true);
          return;
        }}
        if (newTab && !newTab.closed) {{
          newTab.location.href = data.url;
        }} else {{
          window.open(data.url, '_blank');
        }}
        showToast(data.already_running ? `アプリを開きました (${{data.url}})` : `起動完了: ${{data.url}}`);
      }} catch (err) {{
        if (newTab && !newTab.closed) newTab.close();
        showToast(`通信エラー: ${{err.message}}`, true);
      }} finally {{
        busyApps.delete(appId);
        await refreshStatus();
      }}
    }}

    async function stopApp(appId) {{
      if (isShutDown || busyApps.has(appId)) return;
      busyApps.add(appId);
      render(currentState);
      try {{
        const res = await fetch(`/api/stop/${{encodeURIComponent(appId)}}`, {{ method: 'POST' }});
        const data = await res.json();
        if (res.ok && data.ok) {{
          showToast(data.stopped ? 'サーバーを停止しました' : 'すでに停止しています');
        }} else {{
          showToast(data.error || '停止に失敗しました', true);
        }}
      }} catch (err) {{
        showToast(`停止エラー: ${{err.message}}`, true);
      }} finally {{
        busyApps.delete(appId);
        await refreshStatus();
      }}
    }}

    async function stopAll() {{
      if (isShutDown) return;
      try {{
        const res = await fetch('/api/stop_all', {{ method: 'POST' }});
        const data = await res.json();
        if (res.ok && data.ok) {{
          showToast('稼働中のすべてのサーバーを停止しました');
        }}
      }} catch (err) {{
        showToast(`停止エラー: ${{err.message}}`, true);
      }} finally {{
        await refreshStatus();
      }}
    }}

    async function shutdownPortal() {{
      if (isShutDown) return;
      isShutDown = true;
      clearInterval(pollTimer);
      document.getElementById('btn-stop-all').disabled = true;
      document.getElementById('btn-shutdown').disabled = true;
      try {{
        await fetch('/api/shutdown', {{ method: 'POST' }});
      }} catch (_err) {{
        // Portal may close socket right after response
      }}
      document.getElementById('apps-grid').style.display = 'none';
      document.getElementById('shutdown-banner').classList.add('show');
      document.getElementById('server-summary-text').textContent = 'ポータル停止済み';
      document.getElementById('server-summary-dot').className = 'dot';
      document.getElementById('last-updated').textContent = '終了完了';
    }}

    document.addEventListener('visibilitychange', () => {{
      if (document.visibilityState === 'visible' && !isShutDown) {{
        refreshStatus();
      }}
    }});

    render(currentState);
    pollTimer = setInterval(() => {{
      if (document.visibilityState === 'visible' && !isShutDown) {{
        refreshStatus();
      }}
    }}, 5000);
  </script>
</body>
</html>
"""


class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    daemon_threads = True
    allow_reuse_address = True


def make_handler(manager: AppManager, shutdown_trigger):
    class PortalHTTPRequestHandler(BaseHTTPRequestHandler):
        def log_message(self, format, *args):
            sys.stderr.write(f"[portal] {self.address_string()} - {format % args}\n")

        def _send_json(self, status_code: int, payload: dict) -> None:
            body = json.dumps(payload, ensure_ascii=False).encode("utf-8")
            self.send_response(status_code)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(body)

        def _send_html(self, status_code: int, content: str) -> None:
            body = content.encode("utf-8")
            self.send_response(status_code)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(body)

        def do_GET(self):
            parsed = urllib.parse.urlparse(self.path)
            path = parsed.path

            if path == "/" or path == "/index.html":
                self._send_html(200, render_dashboard_html(manager.list_status()))
                return

            if path == "/api/health":
                self._send_json(200, {
                    "ok": True,
                    "app": "atcoder_portal",
                    "pid": os.getpid(),
                    "port": manager.portal_port,
                })
                return

            if path == "/api/apps":
                self._send_json(200, manager.list_status())
                return

            if path.startswith("/static/"):
                self._serve_static(path)
                return

            self._send_json(404, {"ok": False, "error": "Not found"})

        def do_POST(self):
            parsed = urllib.parse.urlparse(self.path)
            path = parsed.path

            if path.startswith("/api/launch/"):
                app_id = urllib.parse.unquote(path[len("/api/launch/"):]).strip("/")
                code, payload = manager.launch(app_id)
                self._send_json(code, payload)
                return

            if path.startswith("/api/stop/"):
                app_id = urllib.parse.unquote(path[len("/api/stop/"):]).strip("/")
                code, payload = manager.stop(app_id)
                self._send_json(code, payload)
                return

            if path == "/api/stop_all":
                stopped_ids = manager.stop_all()
                self._send_json(200, {"ok": True, "stopped_ids": stopped_ids})
                return

            if path == "/api/shutdown":
                self._send_json(200, {"ok": True, "message": "Portal shutting down"})
                self.wfile.flush()
                threading.Thread(target=shutdown_trigger, args=(0.05,), daemon=True).start()
                return

            self._send_json(404, {"ok": False, "error": "Not found"})

        def _serve_static(self, raw_path: str) -> None:
            rel = urllib.parse.unquote(raw_path[len("/static/"):])
            norm = posixpath.normpath(rel).lstrip("/")
            if not norm or norm == "." or norm.startswith(".."):
                self._send_json(403, {"ok": False, "error": "Forbidden"})
                return

            parts = norm.split("/", 1)
            app_id = parts[0]
            app_root = manager.static_root_for(app_id)
            if app_root is None:
                self._send_json(404, {"ok": False, "error": "Unknown static app"})
                return

            if len(parts) == 1:
                if not raw_path.endswith("/"):
                    self.send_response(302)
                    self.send_header("Location", f"/static/{app_id}/index.html")
                    self.end_headers()
                    return
                subpath = "index.html"
            else:
                subpath = parts[1]

            candidate = os.path.realpath(os.path.join(app_root, subpath))
            root_prefix = app_root.rstrip(os.sep) + os.sep
            if candidate != app_root and not candidate.startswith(root_prefix):
                self._send_json(403, {"ok": False, "error": "Forbidden path traversal"})
                return

            if os.path.isdir(candidate):
                if not raw_path.endswith("/"):
                    self.send_response(302)
                    self.send_header("Location", raw_path + "/")
                    self.end_headers()
                    return
                candidate = os.path.realpath(os.path.join(candidate, "index.html"))
                if not candidate.startswith(root_prefix):
                    self._send_json(403, {"ok": False, "error": "Forbidden"})
                    return

            if not os.path.isfile(candidate):
                self._send_json(404, {"ok": False, "error": "File not found"})
                return

            ext = os.path.splitext(candidate)[1].lower()
            explicit_types = {
                ".html": "text/html; charset=utf-8",
                ".htm": "text/html; charset=utf-8",
                ".css": "text/css; charset=utf-8",
                ".js": "application/javascript; charset=utf-8",
                ".json": "application/json; charset=utf-8",
                ".svg": "image/svg+xml",
                ".png": "image/png",
                ".ico": "image/x-icon",
            }
            content_type = explicit_types.get(ext) or mimetypes.guess_type(candidate)[0] or "application/octet-stream"

            try:
                with open(candidate, "rb") as f:
                    data = f.read()
            except OSError as exc:
                self._send_json(500, {"ok": False, "error": html.escape(str(exc))})
                return

            self.send_response(200)
            self.send_header("Content-Type", content_type)
            self.send_header("Content-Length", str(len(data)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(data)

    return PortalHTTPRequestHandler


def write_state_file(pid: int, port: int) -> None:
    payload = {"pid": pid, "port": port, "url": f"http://127.0.0.1:{port}"}
    tmp_path = f"{STATE_FILE}.{pid}.tmp"
    with open(tmp_path, "w", encoding="utf-8") as f:
        json.dump(payload, f)
    os.replace(tmp_path, STATE_FILE)


def remove_own_state_file(own_pid: int) -> None:
    try:
        with open(STATE_FILE, "r", encoding="utf-8") as f:
            data = json.load(f)
        if data.get("pid") != own_pid:
            return
    except (OSError, ValueError):
        pass
    try:
        if os.path.exists(STATE_FILE):
            with open(STATE_FILE, "r", encoding="utf-8") as f:
                data = json.load(f)
            if data.get("pid") == own_pid:
                os.unlink(STATE_FILE)
    except (OSError, ValueError):
        pass


def main():
    ensure_rich_process_path()

    parser = argparse.ArgumentParser(description="AtCoder Apps Portal")
    parser.add_argument("--port", type=int, default=None, help="Bind port (default: auto 8000..8020)")
    parser.add_argument("--no-browser", action="store_true", help="Do not open browser automatically")
    args = parser.parse_args()

    port = args.port if args.port is not None else find_bindable_port(DEFAULT_PORTAL_PORT, MAX_PORTAL_PORT)
    manager = AppManager(portal_port=port)

    shutdown_once = threading.Event()
    httpd_holder: list[ThreadedHTTPServer] = []
    own_pid = os.getpid()

    def cleanup_all(delay: float = 0.0) -> None:
        if delay > 0:
            time.sleep(delay)
        if shutdown_once.is_set():
            return
        shutdown_once.set()
        try:
            manager.stop_all()
        finally:
            remove_own_state_file(own_pid)
            if httpd_holder:
                try:
                    httpd_holder[0].shutdown()
                except Exception:
                    pass

    handler_cls = make_handler(manager, cleanup_all)
    httpd = ThreadedHTTPServer(("127.0.0.1", port), handler_cls)
    httpd_holder.append(httpd)

    write_state_file(own_pid, port)
    atexit.register(lambda: cleanup_all(0.0))

    def handle_signal(signum, _frame):
        threading.Thread(target=cleanup_all, args=(0.0,), daemon=True).start()

    signal.signal(signal.SIGTERM, handle_signal)
    signal.signal(signal.SIGINT, handle_signal)

    url = f"http://127.0.0.1:{port}"
    print("=" * 64)
    print(" ⚡️ AtCoder Apps Portal")
    print("=" * 64)
    print(f" [OK] ポータル起動: {url} (PID {own_pid})")
    if not args.no_browser:
        try:
            webbrowser.open(url)
        except Exception:
            pass

    try:
        httpd.serve_forever()
    finally:
        cleanup_all(0.0)
        httpd.server_close()


if __name__ == "__main__":
    main()
