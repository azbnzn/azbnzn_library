#!/usr/bin/env python3
"""
Competitive Programming Debugger Backend Server (app/cp_debugger/server.py)

- Powered by Clang AddressSanitizer (ASan) & UndefinedBehaviorSanitizer (UBSan).
- Automated bug line identification, human-readable Japanese root cause diagnostics.
- Stress Test Studio (Solver vs Naive vs Generator).
- Trace Timeline parser (icecream-cpp and std::cerr).
"""

import os
import sys
import json
import re
import socket
import shutil
import tempfile
import subprocess
import time
import webbrowser
from http.server import HTTPServer, BaseHTTPRequestHandler
from socketserver import ThreadingMixIn

PORT = 8080
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(BASE_DIR, "..", ".."))

ACL_DIR = os.path.join(PROJECT_ROOT, "ac-library")
INCLUDE_DIR = os.path.join(PROJECT_ROOT, "include")
WATCHDOG_HEADER = os.path.join(BASE_DIR, "watchdog.hpp")

# Clang compilation flags
DEFAULT_CXX = "clang++"
CXX_STD = "-std=c++17"
INCLUDE_FLAGS = []
if os.path.isdir(ACL_DIR):
    INCLUDE_FLAGS.extend(["-I", ACL_DIR])
if os.path.isdir(INCLUDE_DIR):
    INCLUDE_FLAGS.extend(["-I", INCLUDE_DIR])


def symbolize_address(binary_path, base_addr, pc):
    """
    Translates (base_addr, pc) into (function_name, file_name, line_number)
    using 'atos' on macOS or 'addr2line' on Linux.
    """
    if sys.platform == "darwin":
        try:
            cmd = ["atos", "-o", binary_path, "-l", base_addr, pc]
            res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=2.0)
            if res.returncode == 0:
                out = res.stdout.strip()
                # Matches: func_name (in bin) (file.cpp:12) or func_name (in bin) (file:12)
                m = re.search(r"^(.*?)\s+\(in\s+.*?\)\s+\((?:.*?/)?([^/:]+):(\d+)\)", out)
                if m:
                    func, fname, line = m.groups()
                    return func.strip(), fname.strip(), int(line)
        except Exception:
            pass
    else:
        try:
            cmd = ["addr2line", "-e", binary_path, "-f", "-C", pc]
            res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=2.0)
            if res.returncode == 0:
                lines = res.stdout.strip().splitlines()
                if len(lines) >= 2:
                    func = lines[0]
                    loc = lines[1]
                    m = re.search(r"(?:.*?/)?([^/:]+):(\d+)", loc)
                    if m:
                        return func.strip(), m.group(1), int(m.group(2))
        except Exception:
            pass
    return None, None, None


def parse_watchdog_tle(stderr_text, binary_path):
    """
    Parses watchdog timeout output to pinpoint the offending line for infinite loop or cin block.
    """
    tle_pc_match = re.search(r"\[WATCHDOG_TLE_PC\]\s*pc=(0x[0-9a-fA-F]+)\s*base=(0x[0-9a-fA-F]+)\s*file=([^\s\n]*)", stderr_text)
    frames_matches = re.findall(r"\[WATCHDOG_FRAME\s*(\d+)\]\s*pc=(0x[0-9a-fA-F]+)\s*base=(0x[0-9a-fA-F]+)\s*file=([^\s\n]*)", stderr_text)

    bin_name = os.path.basename(binary_path)
    is_kernel_blocked = False

    if tle_pc_match:
        pc, base, fpath = tle_pc_match.groups()
        if "libsystem_kernel" in fpath or "libc" in fpath or "ld-linux" in fpath:
            is_kernel_blocked = True
        elif bin_name in fpath or fpath == binary_path:
            func, fname, line = symbolize_address(binary_path, base, pc)
            if line:
                return {
                    "bug_line": line,
                    "function": func,
                    "is_input_block": False,
                    "cause": "infinite_loop"
                }

    # If PC was in kernel/system or not resolved, inspect callstack frames for user's binary
    for f_idx, pc, base, fpath in frames_matches:
        if int(f_idx) == 0:
            continue
        if bin_name in fpath or fpath == binary_path:
            func, fname, line = symbolize_address(binary_path, base, pc)
            if line:
                return {
                    "bug_line": line,
                    "function": func,
                    "is_input_block": is_kernel_blocked,
                    "cause": "input_wait" if is_kernel_blocked else "infinite_loop"
                }

    return None


def find_free_port(start_port=8080, max_port=8099):
    for port in range(start_port, max_port + 1):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            if s.connect_ex(("127.0.0.1", port)) != 0:
                return port
    return start_port


def parse_bracket_aware(text):
    """Parses comma-separated key: value pairs taking brackets and quotes into account."""
    items = []
    current = []
    depth = 0
    in_quote = False
    quote_char = ""

    for c in text:
        if c in '"\'':
            if in_quote and c == quote_char:
                in_quote = False
            elif not in_quote:
                in_quote = True
                quote_char = c
        elif not in_quote:
            if c in "([{<":
                depth += 1
            elif c in ")]}>":
                depth -= 1
            elif c == "," and depth == 0:
                chunk = "".join(current).strip()
                if chunk:
                    items.append(chunk)
                current = []
                continue
        current.append(c)

    chunk = "".join(current).strip()
    if chunk:
        items.append(chunk)

    parsed = []
    for item in items:
        if ":" in item:
            k, v = item.split(":", 1)
            parsed.append({"name": k.strip(), "value": v.strip()})
        else:
            parsed.append({"name": "", "value": item.strip()})
    return parsed


def parse_compile_error(stderr_text):
    """Parses compiler errors and extracts line, col, and message."""
    errors = []
    lines = stderr_text.splitlines()
    error_re = re.compile(r"(?:.*?/)?([a-zA-Z0-9_\-]+\.cpp):(\d+):(\d+):\s*(fatal error|error):\s*(.*)")
    
    first_line = None
    first_col = None
    
    for l in lines:
        m = error_re.match(l.strip())
        if m:
            _, line_str, col_str, err_type, msg = m.groups()
            line_no = int(line_str)
            col_no = int(col_str)
            if first_line is None:
                first_line = line_no
                first_col = col_no
            errors.append({
                "line": line_no,
                "col": col_no,
                "type": err_type,
                "message": msg
            })
            
    summary = errors[0]["message"] if errors else "コンパイルエラーが発生しました。"
    return {
        "bug_line": first_line,
        "bug_col": first_col,
        "summary": summary,
        "errors": errors
    }


def parse_runtime_error(stderr_text, exit_code):
    """
    Parses ASan, UBSan, Assertions, and Signals to pinpoint offending line and cause.
    """
    bug_line = None
    bug_col = None
    diagnosis = {
        "type": "UNKNOWN_RE",
        "title": "実行時エラー (Runtime Error)",
        "description": f"プログラムが非ゼロの終了コード ({exit_code}) で異常終了しました。",
        "detail": "",
        "advice": "配列外参照、0除算、またはアサーション違反が発生していないか確認してください。"
    }

    # 1. UndefinedBehaviorSanitizer Check (e.g. division by zero, signed overflow)
    ub_match = re.search(r"(?:.*?/)?([a-zA-Z0-9_\-]+\.cpp):(\d+):(\d+):\s*runtime error:\s*(.*)", stderr_text)
    if ub_match:
        _, line_str, col_str, ub_msg = ub_match.groups()
        bug_line = int(line_str)
        bug_col = int(col_str)
        diagnosis["detail"] = f"Line {bug_line}:{bug_col} - {ub_msg}"

        if "division by zero" in ub_msg:
            diagnosis["type"] = "DIVISION_BY_ZERO"
            diagnosis["title"] = "0除算 (Division by Zero)"
            diagnosis["description"] = "数値を 0 または未初期化の変数で割ろうとしました。"
            diagnosis["advice"] = "除数となる変数が 0 になるケースがないか、ループ終了条件や初期化漏れを確認してください。"
        elif "signed integer overflow" in ub_msg:
            diagnosis["type"] = "INTEGER_OVERFLOW"
            diagnosis["title"] = "符号付き整数オーバーフロー (Integer Overflow)"
            diagnosis["description"] = f"int (32bit) の表現範囲 [-2×10⁹, 2×10⁹] を超過しました。\n({ub_msg})"
            diagnosis["advice"] = "変数の型を 'long long' (64bit) に変更するか、計算途中でのオーバーフローを防止してください。"
        elif "shift exponent" in ub_msg:
            diagnosis["type"] = "INVALID_SHIFT"
            diagnosis["title"] = "不正ビットシフト (Invalid Bit Shift)"
            diagnosis["description"] = f"負のシフト幅、または型のビット幅(32/64)以上のシフトを実行しました。\n({ub_msg})"
            diagnosis["advice"] = "1LL << k のように 1LL を使用しているか、またシフト幅 k < 60 を満たしているか確認してください。"
        elif "null pointer" in ub_msg:
            diagnosis["type"] = "NULL_POINTER"
            diagnosis["title"] = "ヌルポインタ参照 (Null Pointer Dereference)"
            diagnosis["description"] = "nullptr (ヌルポインタ) を参照しようとしました。"
            diagnosis["advice"] = "ポインタが指すノード (木の親・子など) が nullptr でないか事前チェックしてください。"
        else:
            diagnosis["type"] = "UNDEFINED_BEHAVIOR"
            diagnosis["title"] = "未定義動作 (Undefined Behavior)"
            diagnosis["description"] = f"未定義動作が検出されました: {ub_msg}"
        return bug_line, bug_col, diagnosis

    # 2. AddressSanitizer Check (e.g. heap-buffer-overflow, container-overflow, stack-overflow)
    asan_summary = re.search(r"SUMMARY:\s*AddressSanitizer:\s*([a-zA-Z0-9_\-]+)\s+(?:.*?/)?([a-zA-Z0-9_\-]+\.cpp):(\d+)", stderr_text)
    if asan_summary:
        asan_type, _, line_str = asan_summary.groups()
        bug_line = int(line_str)
        diagnosis["detail"] = f"ASan: {asan_type} at line {bug_line}"

        if asan_type in ["heap-buffer-overflow", "container-overflow", "global-buffer-overflow", "stack-buffer-overflow"]:
            diagnosis["type"] = asan_type.upper().replace("-", "_")
            diagnosis["title"] = "配列外参照 (Buffer / Container Overflow)"
            diagnosis["description"] = f"配列または std::vector の確保範囲外にアクセスしました ({asan_type})。"
            diagnosis["advice"] = "インデックスが負になっていないか、あるいは 0 <= index < size の範囲内か確認してください。"
        elif asan_type == "stack-overflow":
            diagnosis["type"] = "STACK_OVERFLOW"
            diagnosis["title"] = "スタックオーバーフロー (Stack Overflow)"
            diagnosis["description"] = "再帰呼び出しの深すぎ、または巨大なローカル配列の確保によりスタック領域が枯渇しました。"
            diagnosis["advice"] = "再帰の終了条件(ベースケース)を漏らしていないか、訪問済みチェック(visited)が正しく動作しているか確認してください。"
        elif asan_type == "use-after-free":
            diagnosis["type"] = "USE_AFTER_FREE"
            diagnosis["title"] = "解放後メモリ参照 (Use After Free)"
            diagnosis["description"] = "解放済みのメモリ・消滅したオブジェクトの参照にアクセスしました。"
        return bug_line, bug_col, diagnosis

    # Stack trace search for frame in user's cpp
    stack_frame = re.search(r"#\d+\s+0x[0-9a-fA-F]+\s+in\s+.*?([^/\\\s]+\.cpp):(\d+)", stderr_text)
    if stack_frame:
        _, line_str = stack_frame.groups()
        bug_line = int(line_str)

    # 3. Assertion Failed
    assert_match = re.search(r"Assertion failed:\s*\((.*?)\).*?file\s+.*?([a-zA-Z0-9_\-]+\.cpp),\s*line\s+(\d+)", stderr_text)
    if assert_match:
        cond, _, line_str = assert_match.groups()
        bug_line = int(line_str)
        diagnosis["type"] = "ASSERTION_FAILURE"
        diagnosis["title"] = "アサーション違反 (Assertion Failure)"
        diagnosis["description"] = f"条件式 'assert({cond})' が false となりました。"
        diagnosis["advice"] = f"assert 条件 '{cond}' が満たされない入力パターンを精査してください。"
        return bug_line, bug_col, diagnosis

    # 4. Deadly Signal / Segmentation Fault
    if "DEADLYSIGNAL" in stderr_text or "SEGV" in stderr_text or exit_code in [139, -11]:
        diagnosis["type"] = "SIGSEGV"
        diagnosis["title"] = "セグメンテーション違反 (SIGSEGV)"
        diagnosis["description"] = "不正なメモリアドレスへのアクセス、または未確保ポインタの参照です。"
        diagnosis["advice"] = "未初期化ポインタの参照、nullptrアクセス、または範囲外アクセスを確認してください。"

    return bug_line, bug_col, diagnosis


def parse_trace_events(stderr_text):
    """
    Parses icecream-cpp lines:
      ic| solution.cpp:12 in "int main()"- x: 42, y: 10
    or standard trace patterns:
      [TRACE] line 12: x = 42
    """
    events = []
    step = 1

    ic_detailed_re = re.compile(r"ic\|\s*(?:.*?/)?([a-zA-Z0-9_\-]+\.cpp):(\d+)\s+in\s+\"([^\"]+)\"-\s*(.*)")
    ic_simple_re = re.compile(r"ic\|\s*(.*)")
    trace_tag_re = re.compile(r"\[TRACE(?::(\d+))?\]\s*(.*)")

    for line in stderr_text.splitlines():
        line_clean = line.strip()
        if not line_clean:
            continue

        # Match icecream detailed
        m_det = ic_detailed_re.match(line_clean)
        if m_det:
            _, line_str, func_name, var_str = m_det.groups()
            items = parse_bracket_aware(var_str)
            events.append({
                "step": step,
                "type": "icecream",
                "line": int(line_str),
                "function": func_name,
                "raw": line_clean,
                "items": items
            })
            step += 1
            continue

        # Match icecream simple
        m_sim = ic_simple_re.match(line_clean)
        if m_sim:
            var_str = m_sim.group(1)
            items = parse_bracket_aware(var_str)
            events.append({
                "step": step,
                "type": "icecream",
                "line": None,
                "function": "",
                "raw": line_clean,
                "items": items
            })
            step += 1
            continue

        # Match [TRACE] tag
        m_tr = trace_tag_re.match(line_clean)
        if m_tr:
            line_str, content = m_tr.groups()
            line_no = int(line_str) if line_str else None
            items = parse_bracket_aware(content)
            events.append({
                "step": step,
                "type": "trace",
                "line": line_no,
                "function": "",
                "raw": line_clean,
                "items": items
            })
            step += 1
            continue

    return events


def detect_trace_anomalies(events):
    """Detects suspicious values in variables (overflow, NaN, INF, negative)."""
    warnings = []
    suspicious_patterns = [
        (re.compile(r"^(?:18446744073709551615|-?9223372036854775808|2147483647|-2147483648)$"), "型限界値 / アンダーフローの可能性があります"),
        (re.compile(r"^(?:nan|inf|-inf)$", re.IGNORECASE), "NaN または Inf (非数・無限大) が発生しています"),
        (re.compile(r"^-(?:[1-9]\d{8,})$"), "巨大な負の値が発生しています (未初期化またはオーバーフローの疑い)")
    ]

    for ev in events:
        for it in ev.get("items", []):
            val = it.get("value", "")
            for pat, desc in suspicious_patterns:
                if pat.match(val):
                    loc = f"Line {ev['line']}" if ev.get("line") else f"Step {ev['step']}"
                    warnings.append(f"{loc} 変数 '{it['name']}': 値 '{val}' ({desc})")
                    break

    return warnings[:10]  # limit to 10 warnings


def compare_outputs(actual_str, expected_str):
    """
    Compares outputs token-by-token and line-by-line.
    Returns diff summary and details.
    """
    actual_tokens = actual_str.split()
    expected_tokens = expected_str.split()

    if actual_tokens == expected_tokens:
        return {"matched": True}

    actual_lines = actual_str.strip().splitlines()
    expected_lines = expected_str.strip().splitlines()

    first_diff_line = None
    first_exp_line = None
    first_act_line = None

    for i in range(max(len(actual_lines), len(expected_lines))):
        act_l = actual_lines[i] if i < len(actual_lines) else "<EOF>"
        exp_l = expected_lines[i] if i < len(expected_lines) else "<EOF>"
        if act_l != exp_l:
            first_diff_line = i + 1
            first_exp_line = exp_l
            first_act_line = act_l
            break

    # Token discrepancy
    token_diff_idx = None
    exp_tok = None
    act_tok = None
    for i in range(max(len(actual_tokens), len(expected_tokens))):
        act_t = actual_tokens[i] if i < len(actual_tokens) else "<EOF>"
        exp_t = expected_tokens[i] if i < len(expected_tokens) else "<EOF>"
        if act_t != exp_t:
            token_diff_idx = i + 1
            exp_tok = exp_t
            act_tok = act_t
            break

    return {
        "matched": False,
        "first_diff_line": first_diff_line,
        "first_exp_line": first_exp_line,
        "first_act_line": first_act_line,
        "first_diff_token": token_diff_idx,
        "expected_token": exp_tok,
        "actual_token": act_tok,
        "actual_lines": actual_lines[:50],
        "expected_lines": expected_lines[:50]
    }


def compile_cpp(source_code, binary_path, sanitize=True, use_watchdog=True):
    """Compiles C++ source code with Clang. Returns (success, error_data)."""
    src_path = binary_path + ".cpp"
    with open(src_path, "w", encoding="utf-8") as f:
        f.write(source_code)

    cmd = [DEFAULT_CXX, CXX_STD]
    if sanitize:
        cmd.extend([
            "-O0", "-g",
            "-fsanitize=address,undefined",
            "-fno-sanitize-recover=all",
            "-fno-omit-frame-pointer"
        ])
    else:
        cmd.extend(["-O2"])

    if use_watchdog and os.path.isfile(WATCHDOG_HEADER):
        cmd.extend(["-include", WATCHDOG_HEADER])

    cmd.extend(INCLUDE_FLAGS)
    cmd.extend(["-o", binary_path, src_path])

    res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if res.returncode != 0:
        return False, parse_compile_error(res.stderr)
    return True, None


def run_single_debug(code, input_str, expected_out=None, timeout_sec=2.0, sanitize=True):
    """
    Compiles and executes solution with input, returning thorough diagnosis.
    """
    with tempfile.TemporaryDirectory() as tmpdir:
        bin_path = os.path.join(tmpdir, "solution")
        compiled, err_data = compile_cpp(code, bin_path, sanitize=sanitize, use_watchdog=True)
        if not compiled:
            return {
                "verdict": "CE",
                "bug_line": err_data["bug_line"],
                "bug_col": err_data["bug_col"],
                "diagnosis": {
                    "type": "COMPILE_ERROR",
                    "title": "コンパイルエラー (Compile Error)",
                    "description": err_data["summary"],
                    "detail": "\n".join([f"Line {e['line']}:{e['col']} - {e['message']}" for e in err_data["errors"][:5]]),
                    "advice": "構文エラーや型不一致、ヘッダーのインクルード漏れを確認してください。"
                },
                "stdout": "",
                "stderr": "",
                "traces": [],
                "warnings": []
            }

        # Execution environment
        env = os.environ.copy()
        env["ASAN_OPTIONS"] = "color=never:symbolize=1:allocator_may_return_null=1"
        env["UBSAN_OPTIONS"] = "color=never:symbolize=1:print_stacktrace=1"
        env["WATCHDOG_TIMEOUT_MS"] = str(int(timeout_sec * 1000))

        start_t = time.perf_counter()
        try:
            proc = subprocess.Popen(
                [bin_path],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                env=env,
                text=True
            )
            stdout_str, stderr_str = proc.communicate(input=input_str, timeout=timeout_sec + 0.6)
            elapsed_ms = round((time.perf_counter() - start_t) * 1000, 2)
            exit_code = proc.returncode
        except subprocess.TimeoutExpired:
            proc.kill()
            stdout_str, stderr_str = proc.communicate()
            exit_code = 124
            elapsed_ms = int(timeout_sec * 1000)

        # Check for Watchdog TLE (Infinite loop / cin input wait)
        if "[WATCHDOG_TLE_PC]" in stderr_str or exit_code == 124:
            tle_info = parse_watchdog_tle(stderr_str, bin_path)
            if tle_info and tle_info.get("bug_line"):
                b_line = tle_info["bug_line"]
                func_name = tle_info.get("function") or "関数"
                if tle_info.get("is_input_block"):
                    diag = {
                        "type": "INPUT_WAIT_TIMEOUT",
                        "title": "入力待ちタイムアウト (Stdin Read Timeout)",
                        "description": f"Line {b_line} の '{func_name}' (cin / getline) で入力を読み込もうと待機したままタイムアウトしました。",
                        "detail": f"Line {b_line} in {func_name} - std::cin wait",
                        "advice": "テストケースの入力データが足りているか、または不要な cin の呼び出しや空行の読み飛ばしがないか確認してください。"
                    }
                else:
                    diag = {
                        "type": "INFINITE_LOOP",
                        "title": "無限ループ / 制限時間超過 (Infinite Loop TLE)",
                        "description": f"Line {b_line} の '{func_name}' 内で無限ループ（または過大なループ処理）が発生していることを特定しました。",
                        "detail": f"Line {b_line} in {func_name} - loop/recursion timeout",
                        "advice": "while / for ループの終了条件、ループ変数 (i++ や更新処理) の更新漏れ、または再帰のベースケース漏れを確認してください。"
                    }
                return {
                    "verdict": "TLE",
                    "time_ms": elapsed_ms,
                    "bug_line": b_line,
                    "bug_col": None,
                    "diagnosis": diag,
                    "stdout": stdout_str,
                    "stderr": stderr_str,
                    "traces": parse_trace_events(stderr_str),
                    "warnings": []
                }
            else:
                return {
                    "verdict": "TLE",
                    "time_ms": int(timeout_sec * 1000),
                    "bug_line": None,
                    "bug_col": None,
                    "diagnosis": {
                        "type": "TIME_LIMIT_EXCEEDED",
                        "title": "実行時間制限超過 (Time Limit Exceeded)",
                        "description": f"制限時間 ({timeout_sec}秒) 内にプログラムが終了しませんでした。",
                        "detail": "無限ループ、または計算量が大きすぎる処理が原因です。",
                        "advice": "while/forループの終了条件やインクリメント処理、またはcinの読み込み待ちを確認してください。"
                    },
                    "stdout": stdout_str,
                    "stderr": stderr_str,
                    "traces": [],
                    "warnings": []
                }

        traces = parse_trace_events(stderr_str)
        warnings = detect_trace_anomalies(traces)

        # Check for RE
        if exit_code != 0:
            bug_line, bug_col, diagnosis = parse_runtime_error(stderr_str, exit_code)
            return {
                "verdict": "RE",
                "time_ms": elapsed_ms,
                "exit_code": exit_code,
                "bug_line": bug_line,
                "bug_col": bug_col,
                "diagnosis": diagnosis,
                "stdout": stdout_str,
                "stderr": stderr_str,
                "traces": traces,
                "warnings": warnings
            }

        # Exit code == 0 -> Check output correctness
        diff_info = None
        verdict = "AC"
        diagnosis = {
            "type": "AC",
            "title": "正常終了 (Accepted / Finished)",
            "description": "プログラムはエラーなく正常に実行を完了しました。",
            "detail": f"実行時間: {elapsed_ms} ms",
            "advice": ""
        }

        if expected_out is not None and expected_out.strip() != "":
            diff_res = compare_outputs(stdout_str, expected_out)
            if not diff_res["matched"]:
                verdict = "WA"
                diff_info = diff_res
                diagnosis = {
                    "type": "WRONG_ANSWER",
                    "title": "出力不一致 (Wrong Answer)",
                    "description": f"期待される出力と実際の出力が異なります (最初の不一致: 出力{diff_res['first_diff_line']}行目 / トークン #{diff_res['first_diff_token']})。",
                    "detail": f"期待値: '{diff_res['expected_token']}'  ≠  実際: '{diff_res['actual_token']}'",
                    "advice": "コーナーケース (N=0, 負数, 境界値) の処理、1-indexed/0-indexedのズレ、計算ロジックを点検してください。"
                }

        return {
            "verdict": verdict,
            "time_ms": elapsed_ms,
            "exit_code": exit_code,
            "bug_line": None,
            "bug_col": None,
            "diagnosis": diagnosis,
            "diff": diff_info,
            "stdout": stdout_str,
            "stderr": stderr_str,
            "traces": traces,
            "warnings": warnings
        }


def run_stress_test(solver_code, naive_code, gen_code, iterations=50, timeout_sec=1.5, stop_on_fail=True):
    """
    Automated Stress Testing:
    Runs generator -> naive -> solver in a loop until failure or completion.
    """
    with tempfile.TemporaryDirectory() as tmpdir:
        bin_solver = os.path.join(tmpdir, "solver")
        bin_naive = os.path.join(tmpdir, "naive")
        bin_gen = os.path.join(tmpdir, "gen")

        # Compile Generator (O2)
        comp_gen, err_gen = compile_cpp(gen_code, bin_gen, sanitize=False)
        if not comp_gen:
            return {"success": False, "error": f"Generatorのコンパイルに失敗しました: {err_gen['summary']}"}

        # Compile Naive (O2)
        comp_naive, err_naive = compile_cpp(naive_code, bin_naive, sanitize=False)
        if not comp_naive:
            return {"success": False, "error": f"Naive (愚直解) のコンパイルに失敗しました: {err_naive['summary']}"}

        # Compile Solver (with ASan & UBSan)
        comp_solver, err_solver = compile_cpp(solver_code, bin_solver, sanitize=True)
        if not comp_solver:
            return {"success": False, "error": f"Solver (検証対象) のコンパイルに失敗しました: {err_solver['summary']}"}

        env = os.environ.copy()
        env["ASAN_OPTIONS"] = "color=never:symbolize=1"
        env["UBSAN_OPTIONS"] = "color=never:symbolize=1"

        passed = 0
        for it in range(1, iterations + 1):
            # 1. Run Generator
            try:
                gen_res = subprocess.run([bin_gen], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=2.0)
                if gen_res.returncode != 0:
                    return {"success": False, "error": f"ケース #{it}: Generatorが異常終了しました。\n{gen_res.stderr}"}
                test_input = gen_res.stdout
            except subprocess.TimeoutExpired:
                return {"success": False, "error": f"ケース #{it}: Generatorがタイムアウトしました。"}

            # 2. Run Naive (Expected)
            try:
                naive_res = subprocess.run([bin_naive], input=test_input, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=timeout_sec)
                if naive_res.returncode != 0:
                    return {"success": False, "error": f"ケース #{it}: Naive解が異常終了しました。\n{naive_res.stderr}"}
                expected_out = naive_res.stdout
            except subprocess.TimeoutExpired:
                return {"success": False, "error": f"ケース #{it}: Naive解が制限時間 ({timeout_sec}s) を超過しました。生成ケースの規模を小さくしてください。"}

            # 3. Run Solver
            try:
                solver_proc = subprocess.Popen([bin_solver], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, env=env)
                actual_out, solver_err = solver_proc.communicate(input=test_input, timeout=timeout_sec)
                solver_code_ret = solver_proc.returncode
            except subprocess.TimeoutExpired:
                solver_proc.kill()
                solver_proc.communicate()
                return {
                    "success": False,
                    "passed_count": passed,
                    "total_run": it,
                    "verdict": "TLE",
                    "failed_case": {
                        "iteration": it,
                        "input": test_input,
                        "naive_output": expected_out,
                        "solver_output": "",
                        "solver_stderr": "Timeout expired",
                        "verdict": "TLE",
                        "error_summary": f"Solverが制限時間 ({timeout_sec}秒) を超過しました (TLE)。"
                    }
                }

            # Check Solver RE
            if solver_code_ret != 0:
                bug_l, bug_c, diag = parse_runtime_error(solver_err, solver_code_ret)
                return {
                    "success": False,
                    "passed_count": passed,
                    "total_run": it,
                    "verdict": "RE",
                    "failed_case": {
                        "iteration": it,
                        "input": test_input,
                        "naive_output": expected_out,
                        "solver_output": actual_out,
                        "solver_stderr": solver_err,
                        "verdict": "RE",
                        "bug_line": bug_l,
                        "error_summary": f"Solverで実行時エラーが発生しました ({diag['title']})。"
                    }
                }

            # Check WA
            diff = compare_outputs(actual_out, expected_out)
            if not diff["matched"]:
                return {
                    "success": False,
                    "passed_count": passed,
                    "total_run": it,
                    "verdict": "WA",
                    "failed_case": {
                        "iteration": it,
                        "input": test_input,
                        "naive_output": expected_out,
                        "solver_output": actual_out,
                        "solver_stderr": solver_err,
                        "verdict": "WA",
                        "error_summary": f"ケース #{it} で出力不一致 (WA) を検出しました (期待: '{diff['expected_token']}', 実際: '{diff['actual_token']}')。"
                    }
                }

            passed += 1

        return {
            "success": True,
            "passed_count": passed,
            "total_run": iterations,
            "message": f"全 {iterations} ケースに合格！不一致・エラーは検出されませんでした。"
        }


def inject_icecream_code(source_code):
    """
    Injects icecream.hpp include and context config into user's C++ code.
    """
    lines = source_code.splitlines()
    has_header = any("#include <mylib/icecream.hpp>" in l or "icecream.hpp" in l for l in lines)
    
    # 1. Add header if missing
    if not has_header:
        insert_idx = 0
        for i, l in enumerate(lines):
            if l.strip().startswith("#include"):
                insert_idx = i + 1
        lines.insert(insert_idx, "#include <mylib/icecream.hpp>")

    # 2. Add IC_CONFIG inside main() if missing
    code_text = "\n".join(lines)
    if "IC_CONFIG.include_context" not in code_text:
        main_match = re.search(r"(int\s+main\s*\([^)]*\)\s*\{)", code_text)
        if main_match:
            idx = main_match.end()
            code_text = code_text[:idx] + "\n    IC_CONFIG.include_context(true);" + code_text[idx:]

    return code_text


class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    daemon_threads = True


class DebuggerHTTPRequestHandler(BaseHTTPRequestHandler):
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
        else:
            self.send_error(404, "File Not Found")

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

    def do_POST(self):
        path = self.path.split("?")[0]
        content_len = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(content_len).decode("utf-8") if content_len > 0 else "{}"
        
        try:
            req_data = json.loads(body)
        except Exception as e:
            self.send_json({"error": f"Invalid JSON: {str(e)}"}, status=400)
            return

        if path == "/api/debug":
            code = req_data.get("code", "")
            input_str = req_data.get("input", "")
            expected_out = req_data.get("expected_output", None)
            timeout = float(req_data.get("timeout_sec", 2.0))
            sanitize = bool(req_data.get("sanitize", True))

            result = run_single_debug(code, input_str, expected_out, timeout_sec=timeout, sanitize=sanitize)
            self.send_json(result)

        elif path == "/api/stress_test":
            solver_code = req_data.get("solver_code", "")
            naive_code = req_data.get("naive_code", "")
            gen_code = req_data.get("generator_code", "")
            iterations = int(req_data.get("iterations", 50))
            timeout = float(req_data.get("timeout_sec", 1.5))
            stop_on_fail = bool(req_data.get("stop_on_fail", True))

            result = run_stress_test(solver_code, naive_code, gen_code, iterations=iterations, timeout_sec=timeout, stop_on_fail=stop_on_fail)
            self.send_json(result)

        elif path == "/api/inject_trace":
            code = req_data.get("code", "")
            injected = inject_icecream_code(code)
            self.send_json({"code": injected})

        else:
            self.send_error(404, "Unknown API endpoint")

    def send_json(self, data, status=200):
        resp_bytes = json.dumps(data, ensure_ascii=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(resp_bytes)))
        self.end_headers()
        self.wfile.write(resp_bytes)

    def log_message(self, format, *args):
        # Suppress routine GET logging for cleaner terminal
        if self.path.startswith("/api/"):
            super().log_message(format, *args)


def main():
    port = find_free_port(PORT)
    server_address = ("127.0.0.1", port)
    httpd = ThreadedHTTPServer(server_address, DebuggerHTTPRequestHandler)
    
    url = f"http://127.0.0.1:{port}"
    print("=" * 66)
    print(" ⚡️  AtCoder CP Debugger (競技プログラミング デバッガ)")
    print("=" * 66)
    print(f" [OK] サーバ起動完了: {url}")
    print(" [機能] バグ行特定 / Clang ASan・UBSan診断 / ストレステスト / トレース")
    print("=" * 66)
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
