# 📱 AtCoder アプリケーション集 (`app`)

競技プログラミングの考察・シミュレーション・可視化を補助するためのツール・アプリケーション群です。  
今後様々なアプリケーションを追加・一元管理できるように設計されています。

---

## 📂 アプリケーション一覧

| アプリID | アプリ名 | 概要 | 起動コマンド |
| :--- | :--- | :--- | :--- |
| **`grid_visualizer`** | **Grid Visualizer (グリッド考察ボード)** | $H \times W$ のマス目上でドラッグ色塗り、数字入力、AtCoder入出力コピペができる考察ボード | `python3 app/run.py grid` |
| **`array_graph`** | **Array Graph (配列・数列グラフプロッター)** | 配列 $A_1 \dots A_N$ を点 $(i, A_i)$ としてプロット・グラフ化（非負時はボトム $y=0$ 対応） | `python3 app/run.py graph` |
| **`cp_debugger`** | **CP Debugger (競技プログラミング デバッガ)** | バグ行・原因特定 (ASan/UBSan)、ストレステスト、トレース可視化 | `python3 app/run.py debug` |
| **`stress_tester`** | **Parallel Stress Tester (並列ストレステスター)** | マルチコアCPU並列実行による超高速ストレステスト & 最小反例ケース探索 | `python3 app/run.py stress` |

---

## 🚀 アプリの起動方法

### 1. Spotlight / Finder / Dock から起動（ターミナル不要・推奨）
ネイティブ macOS アプリ **`AtCoder Apps.app`** を使うと、ターミナルを開かずにワンクリックで全アプリを起動できます。

- **Spotlight**: `Cmd + Space` で「`AtCoder Apps`」と入力して Enter
- **Finder / Launchpad**: `~/Applications/AtCoder Apps.app` または `app/AtCoder Apps.app` をダブルクリック（Dock にドラッグしておけばいつでも1クリックで起動可能）

#### 省電力・オンデマンド設計
- **待機負荷ゼロ**: macOS のログイン自動起動（LaunchAgent）は使わず、開いていない時は CPU・メモリ消費ともに **0** です。
- **必要な時だけ起動**: ポータル画面で各アプリの **「開く」** を押した時にだけ対応する Python サーバーを裏で起動します（`grid_visualizer` / `array_graph` は静的配信のため追加プロセス不要）。
- **個別停止 & 完全終了**: ポータル画面から個別のサーバー停止、`全アプリ停止`、または **`ポータルごと終了`**（全サーバー + ポータル自体を完全停止）ができます。

#### `.app` ランチャーの再ビルド
環境（Python パスやアイコン等）が変わった場合は以下で再ビルド・再配置できます：
```bash
python3 app/launcher/build_app.py
```

### 2. CLI ランチャーから起動
```bash
# Web ポータルを起動
python3 app/run.py portal

# アプリ一覧を表示して選択起動
python3 app/run.py

# またはアプリ名・エイリアスを指定して直接起動
python3 app/run.py grid    # グリッド考察ボード
python3 app/run.py graph   # 配列・数列グラフプロッター (別名: array, plot)
python3 app/run.py debug   # 競技プログラミング デバッガ (別名: cpd, debugger)
python3 app/run.py stress  # 並列ストレステスター (別名: st, tester)
```

### 3. ブラウザで直接開く
各Webアプリの `index.html` は単体で完結しているため、ダブルクリックまたはターミナルから即座に開くことができます：

```bash
# グリッド考察ボード
open app/grid_visualizer/index.html

# 配列グラフプロッター
open app/array_graph/index.html
```

---

## ➕ 今後新しいアプリケーションを追加する手順

1. `app/` 配下に新しいアプリ用のディレクトリを作成（例: `app/tree_drawer/`）
2. アプリ本体（`index.html` や Python スクリプト等）を実装
3. `app/run.py` の `APPLICATIONS` リストに以下のように追加：

```python
APPLICATIONS = [
    # 既存のアプリ...
    {
        "id": "tree_drawer",
        "aliases": ["tree", "td"],
        "name": "Tree Drawer (木構造ビジュアライザ)",
        "description": "辺のリストから木の構造を自動描画するツール",
        "type": "browser", # または "python"
        "target": os.path.join(BASE_DIR, "tree_drawer", "index.html"),
    },
]
```
これだけで、`python3 app/run.py tree` や Web ポータル (`AtCoder Apps.app` / `python3 app/run.py portal`) から自動で起動・管理できるようになります。
