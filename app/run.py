#!/usr/bin/env python3
"""
AtCoder Application Manager & Launcher (app/run.py)
今後追加される競プロ補助ツール・Webアプリを一元管理し、ワンクリック・コマンドラインから起動します。
"""

import sys
import os
import subprocess
import webbrowser

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# 登録アプリ一覧
# 各アプリに id, name, description, launch_type, target を定義
APPLICATIONS = [
    {
        "id": "grid_visualizer",
        "aliases": ["grid", "board", "gv"],
        "name": "Grid Visualizer (グリッド考察ボード)",
        "description": "H×Wのグリッド上で色塗り・数字入力を直感的に行える競プロ補助ツール",
        "type": "browser",
        "target": os.path.join(BASE_DIR, "grid_visualizer", "index.html"),
    },
    {
        "id": "array_graph",
        "aliases": ["graph", "array", "plot", "ag", "sp"],
        "name": "Array Graph (配列・数列グラフプロッター)",
        "description": "配列 A_1...A_N のプロット・折れ線グラフ可視化（非負時はボトム y=0 対応）",
        "type": "browser",
        "target": os.path.join(BASE_DIR, "array_graph", "index.html"),
    },
    {
        "id": "cp_debugger",
        "aliases": ["debug", "debugger", "cpd"],
        "name": "CP Debugger (競技プログラミング デバッガ)",
        "description": "バグ行・原因特定 (ASan/UBSan) & ストレステスト & トレース可視化",
        "type": "python",
        "target": os.path.join(BASE_DIR, "cp_debugger", "server.py"),
    },
]

def launch_app(app):
    print(f"\n🚀 起動中: {app['name']}")
    print(f"   説明: {app['description']}\n")
    if app["type"] == "browser":
        target_path = os.path.abspath(app["target"])
        if sys.platform == "darwin":
            subprocess.run(["open", target_path])
        elif sys.platform.startswith("win"):
            os.startfile(target_path)
        else:
            subprocess.run(["xdg-open", target_path])
        print(f"ブラウザで開きました: {target_path}")
    elif app["type"] == "python":
        subprocess.run([sys.executable, app["target"]])
    elif app["type"] == "shell":
        subprocess.run([app["target"]])

def list_apps():
    print("=" * 60)
    print(" 🛠️  AtCoder Applications 一覧")
    print("=" * 60)
    for idx, app in enumerate(APPLICATIONS, 1):
        aliases = f" (別名: {', '.join(app['aliases'])})" if app.get("aliases") else ""
        print(f" [{idx}] {app['name']}")
        print(f"     ID: {app['id']}{aliases}")
        print(f"     説明: {app['description']}")
        print("-" * 60)

def main():
    if len(sys.argv) > 1:
        arg = sys.argv[1].lower().strip()
        if arg in ["--list", "-l", "list"]:
            list_apps()
            return

        # ID または エイリアスで検索
        for app in APPLICATIONS:
            if arg == app["id"].lower() or arg in [a.lower() for a in app.get("aliases", [])]:
                launch_app(app)
                return

        print(f"❌ エラー: アプリ '{arg}' が見つかりませんでした。\n")
        list_apps()
        return

    # 引数なしの場合は対話型メニューを表示
    list_apps()
    print("\n起動したいアプリの [番号] または [ID] を入力してください (q で終了):")
    try:
        choice = input("> ").strip().lower()
    except (KeyboardInterrupt, EOFError):
        print("\n終了します。")
        return

    if choice in ["q", "quit", "exit", ""]:
        print("終了します。")
        return

    # 番号指定
    if choice.isdigit():
        idx = int(choice) - 1
        if 0 <= idx < len(APPLICATIONS):
            launch_app(APPLICATIONS[idx])
            return

    # ID 指定
    for app in APPLICATIONS:
        if choice == app["id"].lower() or choice in [a.lower() for a in app.get("aliases", [])]:
            launch_app(app)
            return

    print("❌ 指定された番号またはIDが見つかりませんでした。")

if __name__ == "__main__":
    main()
