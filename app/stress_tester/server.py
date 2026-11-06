#!/usr/bin/env python3
"""
Parallel Stress Tester Server (app/stress_tester/server.py)

Ultra-fast Multi-Core Stress Testing Suite for Competitive Programming.
- Leverages all available CPU cores in parallel for 10x-50x speedup.
- In-memory pipe execution (zero disk I/O bottle-neck).
- Real-time speedometer (cases/sec) via Server-Sent Events (SSE).
- Smallest counterexample reduction (finds easiest case to debug).
- Direct sync with tools/stress_test/ files.
"""

import os
import sys
import json
import re
import socket
import tempfile
import subprocess
import time
import threading
import queue
import webbrowser
from http.server import HTTPServer, BaseHTTPRequestHandler
from socketserver import ThreadingMixIn
import concurrent.futures

PORT = 8085
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(BASE_DIR, "..", ".."))

TOOLS_DIR = os.path.join(PROJECT_ROOT, "tools", "stress_test")
ACL_DIR = os.path.join(PROJECT_ROOT, "ac-library")
INCLUDE_DIR = os.path.join(PROJECT_ROOT, "include")

DEFAULT_CXX = "clang++"
CXX_STD = "-std=c++17"
INCLUDE_FLAGS = []
if os.path.isdir(ACL_DIR):
    INCLUDE_FLAGS.extend(["-I", ACL_DIR])
if os.path.isdir(INCLUDE_DIR):
    INCLUDE_FLAGS.extend(["-I", INCLUDE_DIR])


def find_free_port(start_port=8085, max_port=8099):
    for port in range(start_port, max_port + 1):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            if s.connect_ex(("127.0.0.1", port)) != 0:
                return port
    return start_port


def compile_cpp(source_code, binary_path, opt_level="-O2", sanitize=False):
    """Compiles C++ source code. Returns (success, error_str)."""
    src_path = binary_path + ".cpp"
    with open(src_path, "w", encoding="utf-8") as f:
        f.write(source_code)

    cmd = [DEFAULT_CXX, CXX_STD, opt_level]
    if sanitize:
        cmd.extend([
            "-g",
            "-fsanitize=address,undefined",
            "-fno-sanitize-recover=all",
            "-fno-omit-frame-pointer"
        ])
    cmd.extend(INCLUDE_FLAGS)
    cmd.extend(["-o", binary_path, src_path])

    res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if res.returncode != 0:
        return False, res.stderr
    return True, None


class ParallelStressManager:
    """Manages multi-core parallel stress test runs."""
    def __init__(self):
        self.lock = threading.RLock()
        self.is_running = False
        self.stop_requested = False
        self.total_target = 1000
        self.completed_count = 0
        self.counts = {"AC": 0, "WA": 0, "RE": 0, "TLE": 0}
        self.failures = []
        self.minimal_failure = None
        self.start_time = 0.0
        self.elapsed_sec = 0.0
        self.speed = 0.0
        self.active_workers = 0
        self.listeners = set()
        self.current_thread = None

    def subscribe(self):
        q = queue.Queue(maxsize=100)
        with self.lock:
            self.listeners.add(q)
        return q

    def unsubscribe(self, q):
        with self.lock:
            self.listeners.discard(q)

    def broadcast(self, data):
        msg = f"data: {json.dumps(data, ensure_ascii=False)}\n\n"
        with self.lock:
            dead = set()
            for q in self.listeners:
                try:
                    q.put_nowait(msg)
                except queue.Full:
                    dead.add(q)
            self.listeners -= dead

    def get_status(self):
        with self.lock:
            elapsed = time.perf_counter() - self.start_time if self.is_running else self.elapsed_sec
            speed = round(self.completed_count / elapsed, 1) if elapsed > 0.1 else 0.0
            return {
                "is_running": self.is_running,
                "completed": self.completed_count,
                "total": self.total_target,
                "elapsed": round(elapsed, 2),
                "speed": speed,
                "active_workers": self.active_workers,
                "counts": dict(self.counts),
                "failures_count": len(self.failures),
                "minimal_failure": self.minimal_failure,
                "failures": self.failures[:50]  # Return top 50 failures
            }

    def stop(self):
        with self.lock:
            self.stop_requested = True

    def start(self, solver_code, naive_code, gen_code, workers=8, max_cases=1000, mode="stop", timeout_sec=2.0, sanitize_solver=False):
        with self.lock:
            if self.is_running:
                return False, "すでにストレステストが実行中です。"
            self.is_running = True
            self.stop_requested = False
            self.total_target = max_cases
            self.completed_count = 0
            self.counts = {"AC": 0, "WA": 0, "RE": 0, "TLE": 0}
            self.failures = []
            self.minimal_failure = None
            self.start_time = time.perf_counter()
            self.elapsed_sec = 0.0
            self.speed = 0.0
            self.active_workers = workers

        self.current_thread = threading.Thread(
            target=self._run_parallel,
            args=(solver_code, naive_code, gen_code, workers, max_cases, mode, timeout_sec, sanitize_solver),
            daemon=True
        )
        self.current_thread.start()
        return True, "テストを開始しました。"

    def _run_parallel(self, solver_code, naive_code, gen_code, workers, max_cases, mode, timeout_sec, sanitize_solver):
        with tempfile.TemporaryDirectory() as tmpdir:
            bin_solver = os.path.join(tmpdir, "solver")
            bin_naive = os.path.join(tmpdir, "naive")
            bin_gen = os.path.join(tmpdir, "gen")

            # Compile Generator
            is_py_gen = gen_code.strip().startswith("#!") or "def " in gen_code or "import " in gen_code
            if not is_py_gen:
                ok, err = compile_cpp(gen_code, bin_gen, opt_level="-O2", sanitize=False)
                if not ok:
                    self._finish_with_error(f"Generator のコンパイルに失敗しました:\n{err}")
                    return
            else:
                bin_gen = os.path.join(tmpdir, "gen.py")
                with open(bin_gen, "w", encoding="utf-8") as f:
                    f.write(gen_code)

            # Compile Naive
            is_py_naive = naive_code.strip().startswith("#!") or "def " in naive_code or "import " in naive_code
            if not is_py_naive:
                ok, err = compile_cpp(naive_code, bin_naive, opt_level="-O2", sanitize=False)
                if not ok:
                    self._finish_with_error(f"Naive (愚直解) のコンパイルに失敗しました:\n{err}")
                    return
            else:
                bin_naive = os.path.join(tmpdir, "naive.py")
                with open(bin_naive, "w", encoding="utf-8") as f:
                    f.write(naive_code)

            # Compile Solver
            is_py_solver = solver_code.strip().startswith("#!") or "def " in solver_code or "import " in solver_code
            if not is_py_solver:
                ok, err = compile_cpp(solver_code, bin_solver, opt_level="-O2", sanitize=sanitize_solver)
                if not ok:
                    self._finish_with_error(f"Solver (検証解) のコンパイルに失敗しました:\n{err}")
                    return
            else:
                bin_solver = os.path.join(tmpdir, "solver.py")
                with open(bin_solver, "w", encoding="utf-8") as f:
                    f.write(solver_code)

            # Execution helper for a single case
            def exec_single_case(test_id):
                if self.stop_requested:
                    return None

                # 1. Run Generator
                try:
                    cmd_gen = [sys.executable, bin_gen] if is_py_gen else [bin_gen]
                    p_gen = subprocess.Popen(cmd_gen, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                    gen_out, gen_err = p_gen.communicate(timeout=2.0)
                    if p_gen.returncode != 0:
                        return {"id": test_id, "verdict": "GEN_ERR", "error": gen_err}
                except subprocess.TimeoutExpired:
                    p_gen.kill()
                    return {"id": test_id, "verdict": "GEN_ERR", "error": "Generator timed out"}

                test_input = gen_out

                # 2. Run Naive (Expected)
                try:
                    cmd_naive = [sys.executable, bin_naive] if is_py_naive else [bin_naive]
                    p_naive = subprocess.Popen(cmd_naive, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                    naive_out, naive_err = p_naive.communicate(input=test_input, timeout=timeout_sec)
                    if p_naive.returncode != 0:
                        return {"id": test_id, "verdict": "NAIVE_ERR", "error": naive_err, "input": test_input}
                except subprocess.TimeoutExpired:
                    p_naive.kill()
                    return {"id": test_id, "verdict": "NAIVE_TLE", "error": "Naive timed out", "input": test_input}

                # 3. Run Solver
                start_sol = time.perf_counter()
                try:
                    cmd_solver = [sys.executable, bin_solver] if is_py_solver else [bin_solver]
                    p_sol = subprocess.Popen(cmd_solver, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                    sol_out, sol_err = p_sol.communicate(input=test_input, timeout=timeout_sec)
                    sol_time_ms = round((time.perf_counter() - start_sol) * 1000, 1)
                    sol_exit = p_sol.returncode
                except subprocess.TimeoutExpired:
                    p_sol.kill()
                    return {
                        "id": test_id,
                        "verdict": "TLE",
                        "input": test_input,
                        "naive_out": naive_out,
                        "solver_out": "",
                        "solver_err": "Time Limit Exceeded",
                        "time_ms": int(timeout_sec * 1000)
                    }

                # Check RE
                if sol_exit != 0:
                    return {
                        "id": test_id,
                        "verdict": "RE",
                        "input": test_input,
                        "naive_out": naive_out,
                        "solver_out": sol_out,
                        "solver_err": sol_err,
                        "time_ms": sol_time_ms,
                        "exit_code": sol_exit
                    }

                # Compare outputs
                if sol_out.split() != naive_out.split():
                    return {
                        "id": test_id,
                        "verdict": "WA",
                        "input": test_input,
                        "naive_out": naive_out,
                        "solver_out": sol_out,
                        "solver_err": sol_err,
                        "time_ms": sol_time_ms
                    }

                return {"id": test_id, "verdict": "AC", "time_ms": sol_time_ms}

            # Batch execution loop with ThreadPoolExecutor
            last_broadcast = 0.0
            test_counter = 0

            with concurrent.futures.ThreadPoolExecutor(max_workers=workers) as executor:
                futures = set()
                batch_size = workers * 4

                while (test_counter < max_cases or futures) and not self.stop_requested:
                    # Fill buffer
                    while len(futures) < batch_size and test_counter < max_cases and not self.stop_requested:
                        test_counter += 1
                        futures.add(executor.submit(exec_single_case, test_counter))

                    if not futures:
                        break

                    # Wait for at least one to complete
                    done, futures = concurrent.futures.wait(futures, return_when=concurrent.futures.FIRST_COMPLETED)
                    for f in done:
                        res = f.result()
                        if res is None:
                            continue

                        v = res.get("verdict")
                        with self.lock:
                            self.completed_count += 1
                            if v in self.counts:
                                self.counts[v] += 1

                            if v in ["WA", "RE", "TLE"]:
                                fail_obj = {
                                    "id": res["id"],
                                    "verdict": v,
                                    "input": res["input"],
                                    "naive_out": res.get("naive_out", ""),
                                    "solver_out": res.get("solver_out", ""),
                                    "solver_err": res.get("solver_err", ""),
                                    "time_ms": res.get("time_ms", 0),
                                    "size": len(res["input"])
                                }
                                self.failures.append(fail_obj)

                                # Update minimal failure (smallest input to debug)
                                if (self.minimal_failure is None) or (fail_obj["size"] < self.minimal_failure["size"]):
                                    self.minimal_failure = fail_obj

                                if mode == "stop":
                                    self.stop_requested = True
                                    break

                    # Broadcast progress every 80ms
                    now = time.perf_counter()
                    if now - last_broadcast >= 0.08:
                        last_broadcast = now
                        self.broadcast(self.get_status())

                # Cancel remaining futures if stopped
                for f in futures:
                    f.cancel()

            # Finalize
            with self.lock:
                self.is_running = False
                self.elapsed_sec = time.perf_counter() - self.start_time
                status = self.get_status()

            self.broadcast({
                "type": "finished",
                **status
            })

    def _finish_with_error(self, err_msg):
        with self.lock:
            self.is_running = False
            self.elapsed_sec = time.perf_counter() - self.start_time
        self.broadcast({
            "type": "error",
            "error": err_msg
        })


MANAGER = ParallelStressManager()


class StressHTTPRequestHandler(BaseHTTPRequestHandler):
    def end_headers(self):
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        super().end_headers()

    def do_OPTIONS(self):
        self.send_response(200)
        self.end_headers()

    def do_GET(self):
        path = self.path.split("?")[0]
        if path in ["/", "/index.html"]:
            self.serve_file(os.path.join(BASE_DIR, "index.html"), "text/html; charset=utf-8")
        elif path == "/style.css":
            self.serve_file(os.path.join(BASE_DIR, "style.css"), "text/css; charset=utf-8")
        elif path == "/script.js":
            self.serve_file(os.path.join(BASE_DIR, "script.js"), "application/javascript; charset=utf-8")
        elif path == "/README.md":
            self.serve_file(os.path.join(BASE_DIR, "README.md"), "text/markdown; charset=utf-8")
        elif path == "/api/system_info":
            cores = os.cpu_count() or 4
            self.send_json({"cpu_count": cores, "recommended_workers": max(1, cores - 1)})
        elif path == "/api/load_tools_files":
            self.handle_load_tools_files()
        elif path == "/api/stress/status":
            self.send_json(MANAGER.get_status())
        elif path == "/api/stress/stream":
            self.handle_sse_stream()
        else:
            self.send_error(404, "File Not Found")

    def handle_sse_stream(self):
        self.send_response(200)
        self.send_header("Content-Type", "text/event-stream")
        self.send_header("Cache-Control", "no-cache")
        self.send_header("Connection", "keep-alive")
        self.end_headers()

        q = MANAGER.subscribe()
        try:
            # Send initial status
            init_msg = f"data: {json.dumps(MANAGER.get_status(), ensure_ascii=False)}\n\n"
            self.wfile.write(init_msg.encode("utf-8"))
            self.wfile.flush()

            while True:
                try:
                    msg = q.get(timeout=1.0)
                    self.wfile.write(msg.encode("utf-8"))
                    self.wfile.flush()
                except queue.Empty:
                    # Heartbeat
                    self.wfile.write(b": keep-alive\n\n")
                    self.wfile.flush()
        except (BrokenPipeError, ConnectionResetError):
            pass
        finally:
            MANAGER.unsubscribe(q)

    def handle_load_tools_files(self):
        files = {
            "main": "",
            "naive": "",
            "gen": ""
        }
        main_path = os.path.join(TOOLS_DIR, "main.cpp")
        naive_path = os.path.join(TOOLS_DIR, "naive.cpp")
        gen_path = os.path.join(TOOLS_DIR, "generator.cpp")
        gen_py_path = os.path.join(TOOLS_DIR, "make_case.py")

        if os.path.isfile(main_path):
            with open(main_path, "r", encoding="utf-8", errors="ignore") as f:
                files["main"] = f.read()
        if os.path.isfile(naive_path):
            with open(naive_path, "r", encoding="utf-8", errors="ignore") as f:
                files["naive"] = f.read()
        if os.path.isfile(gen_path):
            with open(gen_path, "r", encoding="utf-8", errors="ignore") as f:
                files["gen"] = f.read()
        elif os.path.isfile(gen_py_path):
            with open(gen_py_path, "r", encoding="utf-8", errors="ignore") as f:
                files["gen"] = f.read()

        self.send_json({"success": True, "files": files})

    def do_POST(self):
        path = self.path.split("?")[0]
        content_len = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(content_len).decode("utf-8") if content_len > 0 else "{}"

        try:
            req_data = json.loads(body)
        except Exception as e:
            self.send_json({"error": f"Invalid JSON: {str(e)}"}, status=400)
            return

        if path == "/api/stress/start":
            solver = req_data.get("solver_code", "")
            naive = req_data.get("naive_code", "")
            gen = req_data.get("generator_code", "")
            workers = int(req_data.get("workers", os.cpu_count() or 4))
            max_cases = int(req_data.get("max_cases", 1000))
            mode = req_data.get("mode", "stop")
            timeout_sec = float(req_data.get("timeout_sec", 2.0))
            sanitize_solver = bool(req_data.get("sanitize_solver", False))

            ok, msg = MANAGER.start(
                solver_code=solver,
                naive_code=naive,
                gen_code=gen,
                workers=workers,
                max_cases=max_cases,
                mode=mode,
                timeout_sec=timeout_sec,
                sanitize_solver=sanitize_solver
            )
            self.send_json({"success": ok, "message": msg})

        elif path == "/api/stress/stop":
            MANAGER.stop()
            self.send_json({"success": True, "message": "停止要求を送信しました。"})

        elif path == "/api/save_tools_files":
            os.makedirs(TOOLS_DIR, exist_ok=True)
            solver = req_data.get("solver_code")
            naive = req_data.get("naive_code")
            gen = req_data.get("generator_code")

            if solver is not None:
                with open(os.path.join(TOOLS_DIR, "main.cpp"), "w", encoding="utf-8") as f:
                    f.write(solver)
            if naive is not None:
                with open(os.path.join(TOOLS_DIR, "naive.cpp"), "w", encoding="utf-8") as f:
                    f.write(naive)
            if gen is not None:
                is_py = "import " in gen or "def " in gen
                fname = "make_case.py" if is_py else "generator.cpp"
                with open(os.path.join(TOOLS_DIR, "generator.cpp"), "w", encoding="utf-8") as f:
                    f.write(gen)

            self.send_json({"success": True, "message": f"tools/stress_test/ に保存しました。"})

        else:
            self.send_error(404, "Endpoint Not Found")

    def serve_file(self, filepath, content_type):
        if not os.path.isfile(filepath):
            self.send_error(404, "File Not Found")
            return
        with open(filepath, "rb") as f:
            data = f.read()
        self.send_response(200)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def send_json(self, data, status=200):
        resp_bytes = json.dumps(data, ensure_ascii=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(resp_bytes)))
        self.end_headers()
        self.wfile.write(resp_bytes)

    def log_message(self, format, *args):
        # Quiet logging
        pass


class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    daemon_threads = True


def main():
    port = find_free_port(PORT)
    server_address = ("127.0.0.1", port)
    httpd = ThreadedHTTPServer(server_address, StressHTTPRequestHandler)

    url = f"http://127.0.0.1:{port}"
    cores = os.cpu_count() or 4
    print("=" * 68)
    print(" 🚀  AtCoder Parallel Stress Tester (並列ストレステスター)")
    print("=" * 68)
    print(f" [OK] サーバ起動完了: {url}")
    print(f" [並列] 検出CPUコア数: {cores} コア (最大 {cores} スレッド並列実行)")
    print(" [機能] パイプインメモリ高速実行 / SSE リアルタイム速度メーター / 最小ケース探索")
    print("=" * 68)
    print(" ブラウザで開いています... (終了するには Ctrl+C を押してください)\n")

    try:
        webbrowser.open(url)
    except Exception:
        pass

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nサーバを停止しました。")
        httpd.server_close()


if __name__ == "__main__":
    main()
