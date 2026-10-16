# C++ Competitive Programming Library (`mylib`)

競技プログラミング（AtCoder / Codeforces / ICPC 等）向けの C++17 アルゴリズム・データ構造ライブラリ集です。  
AtCoder Library (ACL) の設計思想を踏襲しつつ、ACL に含まれない高度なデータ構造や典型アルゴリズムを幅広く網羅しています。

> **💡 各ライブラリのソースコード全文は折りたたまれています。「ソースコードを展開」をクリックすることで全文を閲覧・コピーできます。**

---

## 目次

- [特徴](#特徴)
- [クイック逆引きチートシート（コンテスト用）](#クイック逆引きチートシートコンテスト用)
- [ライブラリ詳細リファレンス](#ライブラリ詳細リファレンス)
  - [1. データ構造 (Data Structures)](#1-データ構造-data-structures)
    - [UnionFind](#unionfind)
    - [WeightedUnionFind (重み付きUF)](#weightedunionfind-重み付きuf)
    - [SparseTable](#sparsetable)
    - [DisjointSparseTable](#disjointsparsetable)
    - [SegTree2D (2次元セグ木)](#segtree2d-2次元セグ木)
    - [DynamicSegTree (動的セグ木)](#dynamicsegtree-動的セグ木)
    - [PersistentSegTree (永続セグ木)](#persistentsegtree-永続セグ木)
    - [SqrtDecomposition (平方分割)](#sqrtdecomposition-平方分割)
    - [LazySqrtDecomposition (遅延平方分割)](#lazysqrtdecomposition-遅延平方分割)
    - [OrderStatisticSet (順序付き集合)](#orderstatisticset-順序付き集合)
    - [CoordinateCompression (座標圧縮)](#coordinatecompression-座標圧縮)
  - [2. 文字列 (Strings)](#2-文字列-strings)
    - [RollingHash (安全なダブルローリングハッシュ)](#rollinghash-安全なダブルローリングハッシュ)
    - [DynamicStringTreap (動的文字列平衡二分木)](#dynamicstringtreap-動的文字列平衡二分木)
    - [Trie / MapTrie](#trie--maptrie)
  - [3. グラフ・木 (Graphs & Trees)](#3-グラフ木-graphs--trees)
    - [Dijkstra (単一始点最短経路)](#dijkstra-単一始点最短経路)
    - [TopologicalSort (トポロジカルソート)](#topologicalsort-トポロジカルソート)
    - [Rerooting (全方位木DP)](#rerooting-全方位木dp)
  - [4. 数学・数論 (Math & Number Theory)](#4-数学数論-math--number-theory)
    - [NumberTheory (高速数論・素因数分解・CRT)](#numbertheory-高速数論素因数分解crt)
    - [BiCoef (二項係数・順列・重複組合せ)](#bicoef-二項係数順列重複組合せ)
    - [LinearDiophantine (一次不定方程式)](#lineardiophantine-一次不定方程式)
    - [Matrix (半環対応汎用行列累乗)](#matrix-半環対応汎用行列累乗)
  - [5. 幾何 (Geometry)](#5-幾何-geometry)
    - [Geometry (2次元計算幾何)](#geometry-2次元計算幾何)
- [一括インクルード (All.hpp)](#一括インクルード-allhpp)
- [使い方と環境設定](#使い方と環境設定)

---

## 特徴

1. **ACL (AtCoder Library) 完全互換・高親和性**
   - 平方分割 (`SqrtDecomposition`)、遅延平方分割 (`LazySqrtDecomposition`) は ACL の `segtree` / `lazy_segtree` と同一の関数シグネチャ・二分探索メソッド (`max_right`, `min_left`) を実装しています。
   - 二項係数 (`BiCoef`) は `atcoder::modint` とシームレスに連携します。
2. **名前空間フリーで即戦力利用可能**
   - `namespace mylib` は除去されており、ファイルをインクルードまたはコードを提出用テンプレートに直接貼り付けてすぐ使えます。
3. **統一された PascalCase ファイル命名規則**
   - すべてのファイル名が `MyFavoriteClass.hpp` 形式に統一されており、クラス名からヘッダーファイルを直感的に特定できます。
4. **一括インクルード対応**
   - `#include "mylib/All.hpp"` または `#include "mylib.hpp"` を記述するだけで全ライブラリが利用可能になります。

---

## クイック逆引きチートシート（コンテスト用）

| やりたいこと / 典型シチュエーション | 推奨ライブラリ | 主な計算量 | ファイル |
| :--- | :--- | :--- | :--- |
| グループ分け・連結判定・成分サイズ | `UnionFind` | $O(\alpha(N))$ | `UnionFind.hpp` |
| $x - y = \text{diff}$ の差分制約・ポテンシャル差 | `WeightedUnionFind<T>` | $O(\alpha(N))$ | `WeightedUnionFind.hpp` |
| 静的配列の区間最小値・最大値・gcd (更新なし) | `SparseTable<S, op>` | 構築 $O(N \log N)$<br>クエリ $O(1)$ | `SparseTable.hpp` |
| 静的配列の区間積・区間和 (非可換・モノイド) | `DisjointSparseTable<S, op>` | 構築 $O(N \log N)$<br>クエリ $O(1)$ | `DisjointSparseTable.hpp` |
| グリッド・2次元平面の矩形領域クエリ | `SegTree2D<S, op, e>` | クエリ $O(\log H \log W)$ | `SegTree2D.hpp` |
| 座標が $10^9$ や $10^{18}$ と巨大なセグ木 | `DynamicSegTree<S, op, e>` | クエリ $O(\log C)$ | `DynamicSegTree.hpp` |
| 過去の状態へのクエリ・ロールバック・復元 | `PersistentSegTree<S, op, e>` | 点更新 $O(\log N)$<br>クエリ $O(\log N)$ | `PersistentSegTree.hpp` |
| ブロック単位の平方分割（ACL segtree 互換） | `SqrtDecomposition<S, op, e>` | クエリ $O(\sqrt{N})$ | `SqrtDecomposition.hpp` |
| 区間加算・区間変更・区間クエリ（遅延評価） | `LazySqrtDecomposition<...>` | クエリ $O(\sqrt{N})$ | `LazySqrtDecomposition.hpp` |
| $k$ 番目に小さい値 / 値 $x$ 未満の要素数 | `OrderStatisticSet<T>` (`OSS`) | $O(\log N)$ | `OrderStatisticSet.hpp` |
| 座標圧縮・添字変換・復元 | `CoordinateCompression<T>` | 構築 $O(N \log N)$<br>取得 $O(\log N)$ / $O(1)$ | `CoordinateCompression.hpp` |
| 文字列一致判定・LCP（最長共通接頭辞） | `DoubleRollingHash` | ハッシュ取得 $O(1)$<br>LCP $O(\log N)$ | `RollingHash.hpp` |
| 文字列の任意位置挿入・削除・区間文字種数 | `DynamicStringTreap` | 各種操作 $O(\log N)$ | `DynamicStringTreap.hpp` |
| 複数文字列のプレフィックス検索・辞書 | `Trie` / `MapTrie<T>` | 挿入・検索 $O(|S|)$ | `Trie.hpp` |
| 単一始点最短経路 (重み付き正辺グラフ) | `dijkstra` | $O((V + E) \log V)$ | `Dijkstra.hpp` |
| 依存関係の順序付け・閉路検出・DAG判定 | `topological_sort` | $O(V + E)$ | `TopologicalSort.hpp` |
| 木のすべての頂点を根としたときの木DP | `Rerooting<...>` | $O(N)$ | `Rerooting.hpp` |
| 高速素数判定 ($N \le 10^{18}$) | `miller_rabin(n)` | $O(k \log^3 n)$ | `NumberTheory.hpp` |
| 高速素因数分解 ($N \le 10^{18}$) | `pollard_rho(n)` / `factorize(n)` | $O(N^{1/4})$ | `NumberTheory.hpp` |
| 拡張ユークリッド互除法・中国剰余定理 | `extgcd`, `modinv`, `crt` | $O(\log(\min(a, b)))$ | `NumberTheory.hpp` |
| 順列・組合せ ($nPr, nCr, nHr$) の高速計算 | `BiCoef<mint>` | 前計算 $O(N)$<br>クエリ $O(1)$ | `BiCoef.hpp` |
| 一次不定方程式 $ax + by = c$ の整数解 | `LinearDiophantine` | $O(\log(\min(a, b)))$ | `LinearDiophantine.hpp` |
| 遷移の高速化・グリッドパス数・最短路DP | `Matrix<T, Semiring>` | $O(K^3 \log N)$ | `Matrix.hpp` |
| 幾何交差判定・凸包・点内外判定・円交差 | `Geometry` | 凸包 $O(N \log N)$ ほか | `Geometry.hpp` |

---

## ライブラリ詳細リファレンス

### 1. データ構造 (Data Structures)

#### UnionFind
素集合データ構造（経路圧縮 + サイズマージ）。ACL `dsu` と同等の機能を提供します。
- **ファイル**: `mylib/UnionFind.hpp`
- **計算量**: ほぼ $O(1)$（ならしアッカーマンの逆関数 $O(\alpha(N))$）

```cpp
#include "mylib/UnionFind.hpp"

UnionFind uf(n);
uf.merge(u, v);      // u と v を結合 (新しく結合されたら true)
if (uf.same(u, v)) { /* 同一連結成分 */ }
int sz = uf.size(u); // u が属する成分の頂点数
vector<vector<int>> groups = uf.groups(); // 全連結成分のグループ一覧
```

<details>
<summary><b>ソースコードを展開 (UnionFind.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <numeric>
#include <vector>

// 素集合データ構造 (Union-Find / Disjoint Set Union)
struct UnionFind {
  std::vector<int> parent_or_size;

  UnionFind() : parent_or_size(0) {}
  UnionFind(int n) : parent_or_size(n, -1) {}

  int root(int x) {
    if (parent_or_size[x] < 0)
      return x;
    return parent_or_size[x] = root(parent_or_size[x]);
  }

  // x と y を結合する (併合されたら true, 既に同連結成分なら false)
  bool merge(int x, int y) {
    int rx = root(x), ry = root(y);
    if (rx == ry)
      return false;
    if (-parent_or_size[rx] < -parent_or_size[ry])
      std::swap(rx, ry);
    parent_or_size[rx] += parent_or_size[ry];
    parent_or_size[ry] = rx;
    return true;
  }

  // x と y が同じ連結成分に属するか判定
  bool same(int x, int y) { return root(x) == root(y); }

  // x が属する連結成分のサイズを取得
  int size(int x) { return -parent_or_size[root(x)]; }

  // 全連結成分のグループ一覧を取得 (ACL dsu::groups() 互換)
  std::vector<std::vector<int>> groups() {
    int n = parent_or_size.size();
    std::vector<std::vector<int>> result;
    std::vector<int> leader_buf(n);
    std::vector<int> group_size(n, 0);
    for (int i = 0; i < n; i++) {
      leader_buf[i] = root(i);
      group_size[leader_buf[i]]++;
    }
    std::vector<std::vector<int>> res(n);
    for (int i = 0; i < n; i++) {
      res[i].reserve(group_size[i]);
    }
    for (int i = 0; i < n; i++) {
      res[leader_buf[i]].push_back(i);
    }
    res.erase(
        std::remove_if(res.begin(), res.end(),
                       [&](const std::vector<int> &v) { return v.empty(); }),
        res.end());
    return res;
  }
};
```

</details>

---

#### WeightedUnionFind (重み付きUF)
ノード間のポテンシャル差（相対的な値の差 $V[y] - V[x] = w$）を同時に管理する Union-Find です。
- **ファイル**: `mylib/WeightedUnionFind.hpp`
- **計算量**: 各操作 $O(\alpha(N))$

```cpp
#include "mylib/WeightedUnionFind.hpp"

WeightedUnionFind<long long> wuf(n);
// pot(y) - pot(x) = w となるように併合 (矛盾があれば false を返す)
if (!wuf.merge(x, y, w)) {
    cout << "矛盾が発生しました" << endl;
}
if (wuf.same(x, y)) {
    long long diff = wuf.diff(x, y); // pot(y) - pot(x) を取得
}
```

<details>
<summary><b>ソースコードを展開 (WeightedUnionFind.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

using namespace std;

// ポテンシャル付き Union-Find (Weighted Union-Find)
// 各頂点 x にポテンシャル V[x] を持たせ、辺 (x, y, w) で V[y] - V[x] = w の関係を管理する。
//
// Type: ポテンシャルの型 (通常 long long や int, double など)
template <typename Type = long long> struct WeightedUnionFind {
  int n;
  vector<int> parent_or_size;
  vector<Type> diff_weight; // 親ノードに対する相対ポテンシャル: V[x] - V[parent[x]]

  WeightedUnionFind() : n(0) {}
  explicit WeightedUnionFind(int n)
      : n(n), parent_or_size(n, -1), diff_weight(n, 0) {}

  // 代表元を返す (経路圧縮を行いながら親への重み差を累積更新)
  int root(int x) {
    assert(0 <= x && x < n);
    if (parent_or_size[x] < 0)
      return x;
    int r = root(parent_or_size[x]);
    diff_weight[x] += diff_weight[parent_or_size[x]];
    return parent_or_size[x] = r;
  }

  // 根からの相対ポテンシャル V[x] - V[root(x)] を取得
  Type weight(int x) {
    root(x);
    return diff_weight[x];
  }

  // x と y が同じ連結成分に属するか判定
  bool same(int x, int y) { return root(x) == root(y); }

  // V[y] - V[x] を取得 (同じ連結成分に属している必要がある)
  Type diff(int x, int y) {
    assert(same(x, y));
    return weight(y) - weight(x);
  }

  // V[y] - V[x] = w となるように x と y を結合する
  // 既に同じ連結成分に属している場合:
  // - 既存の diff(x, y) == w ならば true を返す (矛盾なし)
  // - 既存の diff(x, y) != w ならば false を返す (矛盾検出)
  // 新しく結合された場合は true を返す
  bool merge(int x, int y, Type w) {
    assert(0 <= x && x < n);
    assert(0 <= y && y < n);

    // root(x) と root(y) の差に換算:
    // w_root = V[root(y)] - V[root(x)] = weight(x) + w - weight(y)
    Type w_root = weight(x) + w - weight(y);
    int rx = root(x);
    int ry = root(y);

    if (rx == ry) {
      // 矛盾判定
      return diff(x, y) == w;
    }

    // サイズによる結合 (Union by size)
    if (-parent_or_size[rx] < -parent_or_size[ry]) {
      swap(rx, ry);
      w_root = -w_root;
    }

    parent_or_size[rx] += parent_or_size[ry];
    parent_or_size[ry] = rx;
    diff_weight[ry] = w_root;
    return true;
  }

  // x が属する連結成分のサイズ
  int size(int x) { return -parent_or_size[root(x)]; }

  // 全連結成分のグループ一覧
  vector<vector<int>> groups() {
    vector<int> leader_buf(n);
    vector<int> group_size(n, 0);
    for (int i = 0; i < n; i++) {
      leader_buf[i] = root(i);
      group_size[leader_buf[i]]++;
    }
    vector<vector<int>> res(n);
    for (int i = 0; i < n; i++) {
      res[i].reserve(group_size[i]);
    }
    for (int i = 0; i < n; i++) {
      res[leader_buf[i]].push_back(i);
    }
    res.erase(
        remove_if(res.begin(), res.end(),
                  [&](const vector<int> &v) { return v.empty(); }),
        res.end());
    return res;
  }
};
```

</details>

---

#### SparseTable
羃等性（$x \circ x = x$）を持つ半群に対する静的区間クエリを $O(1)$ で処理します。
- **ファイル**: `mylib/SparseTable.hpp`
- **適した演算**: 区間最小値 (`min`), 区間最大値 (`max`), 区間最大公約数 (`gcd`), 区間 bitwise AND/OR
- **計算量**: 構築 $O(N \log N)$、クエリ $O(1)$、空間 $O(N \log N)$

```cpp
#include "mylib/SparseTable.hpp"

int op_min(int a, int b) { return min(a, b); }

vector<int> a = {3, 1, 4, 1, 5, 9, 2, 6};
SparseTable<int, op_min> st(a);
int ans = st.query(2, 6); // 半開区間 [2, 6) の最小値 -> 1
```

<details>
<summary><b>ソースコードを展開 (SparseTable.hpp)</b></summary>

```cpp
#pragma once
#include <bits/stdc++.h>

using namespace std;

// 羃等性 (idempotent: op(x, x) = x) をもつ半群に対する区間クエリを O(1) で処理する
// テンプレート引数: 要素の型 S, 演算関数ポインタ op (例: min, max, gcd)
template <class S, S (*op)(S, S)> class SparseTable {
private:
  int n;
  vector<vector<S>> table;
  vector<int> log_table;

public:
  // デフォルトコンストラクタ
  SparseTable() : n(0) {}

  // コンストラクタ：vectorを渡して構築 O(N log N)
  SparseTable(const vector<S> &v) : n(v.size()) {
    build(v);
  }

  void build(const vector<S> &v) {
    n = v.size();
    if (n == 0) {
      table.clear();
      log_table.clear();
      return;
    }

    // ログテーブルの事前計算
    log_table.assign(n + 1, 0);
    for (int i = 2; i <= n; i++) {
      log_table[i] = log_table[i >> 1] + 1;
    }

    int max_log = log_table[n];
    table.assign(max_log + 1, vector<S>(n));

    // 初期状態（長さ1の区間）
    for (int i = 0; i < n; i++) {
      table[0][i] = v[i];
    }

    // テーブルの構築
    for (int k = 1; k <= max_log; k++) {
      for (int i = 0; i + (1 << k) <= n; i++) {
        table[k][i] = op(table[k - 1][i], table[k - 1][i + (1 << (k - 1))]);
      }
    }
  }

  // 区間 [l, r) のクエリを処理 O(1)
  S query(int l, int r) const {
    assert(0 <= l && l < r && r <= n);
    int k = log_table[r - l];
    return op(table[k][l], table[k][r - (1 << k)]);
  }
};
```

</details>

---

#### DisjointSparseTable
**羃等性を必要としない** 任意の結合則を満たす半群（加算、行列積、一次関数合成など）に対し、静的区間クエリを $O(1)$ で処理します。
- **ファイル**: `mylib/DisjointSparseTable.hpp`
- **計算量**: 構築 $O(N \log N)$、クエリ $O(1)$

```cpp
#include "mylib/DisjointSparseTable.hpp"

long long op_add(long long a, long long b) { return a + b; }

vector<long long> a = {1, 2, 3, 4, 5};
DisjointSparseTable<long long, op_add> dst(a);
long long sum = dst.query(1, 4); // [1, 4) の和 -> 2 + 3 + 4 = 9
```

<details>
<summary><b>ソースコードを展開 (DisjointSparseTable.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

// Disjoint Sparse Table
// 結合律のみを満たす任意の半群 (Semigroup) に対し、静的な区間積クエリを O(1) で処理するデータ構造。
// 通常の Sparse Table と異なり、羃等性 (op(x, x) == x) は不要です (行列積や非可換演算も可)。
//
// 構築: O(N log N)
// クエリ: O(1)
//
// S: 要素の型
// op: 2つの S を結合する二項演算
// e: 単位元を返す関数 (省略時は nullptr。空区間クエリ時に使用)
template <class S, S (*op)(S, S), S (*e)() = nullptr>
struct DisjointSparseTable {
  int n;
  vector<vector<S>> table;

  DisjointSparseTable() : n(0) {}

  explicit DisjointSparseTable(const vector<S> &v) : n(v.size()) {
    if (n == 0)
      return;

    int max_log = 1;
    while ((1 << max_log) <= n)
      max_log++;

    table.assign(max_log, vector<S>(n));
    table[0] = v;

    for (int i = 1; i < max_log; i++) {
      int shift = 1 << i;
      for (int m = shift; m < n; m += (shift << 1)) {
        // m を境界として左へ累積
        table[i][m - 1] = v[m - 1];
        for (int j = m - 2; j >= m - shift && j >= 0; j--) {
          table[i][j] = op(v[j], table[i][j + 1]);
        }
        // m を境界として右へ累積
        table[i][m] = v[m];
        int r_lim = min(n, m + shift);
        for (int j = m + 1; j < r_lim; j++) {
          table[i][j] = op(table[i][j - 1], v[j]);
        }
      }
    }
  }

  // 半開区間 [l, r) の総積を取得 (O(1))
  S prod(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    if (l == r) {
      if constexpr (e != nullptr) {
        return e();
      } else {
        assert(false && "Empty query [l, r) with no identity element provided.");
      }
    }
    int r_idx = r - 1;
    if (l == r_idx) {
      return table[0][l];
    }
    // l と r_idx の最上位の異なるビット (MSB)
    int diff = l ^ r_idx;
    int p = 31 - __builtin_clz(diff);
    return op(table[p][l], table[p][r_idx]);
  }

  // 1点取得 (O(1))
  S get(int p) const {
    assert(0 <= p && p < n);
    return table[0][p];
  }

  S operator[](int p) const { return get(p); }
};
```

</details>

---

#### SegTree2D (2次元セグ木)
可換モノイドに対する 2次元グリッド・平面上の矩形領域クエリと 1点更新を処理します。
- **ファイル**: `mylib/SegTree2D.hpp`
- **注意**: 演算 `op` は可換（$op(a, b) = op(b, a)$）である必要があります。
- **計算量**: 構築 $O(HW)$、点更新 $O(\log H \log W)$、矩形クエリ $O(\log H \log W)$

```cpp
#include "mylib/SegTree2D.hpp"

long long op_add(long long a, long long b) { return a + b; }
long long e_zero() { return 0; }

SegTree2D<long long, op_add, e_zero> seg(H, W);
seg.set(r, c, val);                   // (r, c) に val をセット
seg.add(r, c, val);                   // (r, c) に val を加算
long long res = seg.query(h1, w1, h2, w2); // [h1, h2) x [w1, w2) の長方形区間積
```

<details>
<summary><b>ソースコードを展開 (SegTree2D.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

// 2次元セグメント木 (可換モノイドに対する 2D Segment Tree)
// 注意: 演算 op は可換 (op(a, b) == op(b, a)) である必要があります (例: min, max, +, gcd, XORなど)
// S: 要素の型
// op: 2つの S を結合する可換演算
// e: 単位元を返す関数
template <class S, S (*op)(S, S), S (*e)()> struct SegTree2D {
  int H, W;
  vector<S> dat;

  SegTree2D() : H(0), W(0) {}
  SegTree2D(int h, int w) : H(h), W(w) {
    if (H > 0 && W > 0) {
      dat.assign(4 * H * W, e());
    }
  }

  // 2次元配列 v から O(HW) で構築
  SegTree2D(const vector<vector<S>> &v) {
    build(v);
  }

  // 2次元配列 v から O(HW) で初期化
  void build(const vector<vector<S>> &v) {
    H = v.size();
    W = (H == 0 ? 0 : v[0].size());
    if (H == 0 || W == 0)
      return;
    dat.assign(4 * H * W, e());

    for (int r = 0; r < H; r++) {
      for (int c = 0; c < W; c++) {
        dat[id(r + H, c + W)] = v[r][c];
      }
    }
    for (int r = 0; r < H; r++) {
      for (int c = W - 1; c > 0; c--) {
        dat[id(r + H, c)] =
            op(dat[id(r + H, 2 * c)], dat[id(r + H, 2 * c + 1)]);
      }
    }
    for (int r = H - 1; r > 0; r--) {
      for (int c = 1; c < 2 * W; c++) {
        dat[id(r, c)] = op(dat[id(2 * r, c)], dat[id(2 * r + 1, c)]);
      }
    }
  }

  // (r, c) の要素を val に更新する (O(log H * log W))
  void set(int r, int c, S val) {
    assert(0 <= r && r < H && 0 <= c && c < W);
    int cr = r + H;
    int cc = c + W;
    dat[id(cr, cc)] = val;
    for (int i = cc >> 1; i > 0; i >>= 1) {
      dat[id(cr, i)] = op(dat[id(cr, 2 * i)], dat[id(cr, 2 * i + 1)]);
    }
    for (cr >>= 1; cr > 0; cr >>= 1) {
      dat[id(cr, cc)] = op(dat[id(2 * cr, cc)], dat[id(2 * cr + 1, cc)]);
      for (int i = cc >> 1; i > 0; i >>= 1) {
        dat[id(cr, i)] = op(dat[id(cr, 2 * i)], dat[id(cr, 2 * i + 1)]);
      }
    }
  }

  // (r, c) の要素を取得する (O(1))
  S get(int r, int c) const {
    assert(0 <= r && r < H && 0 <= c && c < W);
    return dat[id(r + H, c + W)];
  }

  // 矩形区間 [r1, r2) × [c1, c2) の演算結果を取得する (O(log H * log W))
  S prod(int r1, int c1, int r2, int c2) const {
    assert(0 <= r1 && r1 <= r2 && r2 <= H);
    assert(0 <= c1 && c1 <= c2 && c2 <= W);
    if (r1 == r2 || c1 == c2)
      return e();
    S res = e();
    for (r1 += H, r2 += H; r1 < r2; r1 >>= 1, r2 >>= 1) {
      if (r1 & 1)
        res = op(res, prod_col(r1++, c1, c2));
      if (r2 & 1)
        res = op(res, prod_col(--r2, c1, c2));
    }
    return res;
  }

private:
  inline int id(int r, int c) const { return r * 2 * W + c; }

  S prod_col(int r, int c1, int c2) const {
    S res = e();
    for (c1 += W, c2 += W; c1 < c2; c1 >>= 1, c2 >>= 1) {
      if (c1 & 1)
        res = op(res, dat[id(r, c1++)]);
      if (c2 & 1)
        res = op(res, dat[id(r, --c2)]);
    }
    return res;
  }
};
```

</details>

---

#### DynamicSegTree (動的セグ木)
インデックスが $0 \le p < 10^{18}$ などの巨大な範囲に及ぶ場合、アクセスされたノードのみを動的に生成するセグメント木です。
- **ファイル**: `mylib/DynamicSegTree.hpp`
- **計算量**: 1点更新 $O(\log C)$、区間積取得 $O(\log C)$、二分探索 $O(\log C)$（$C$ は座標範囲）

```cpp
#include "mylib/DynamicSegTree.hpp"

long long op_max(long long a, long long b) { return max(a, b); }
long long e_neg() { return -2e18; }

DynamicSegTree<long long, op_max, e_neg> seg(1000000000000000000LL); // 10^18
seg.set(1234567890123LL, 100);
long long ans = seg.prod(0, 2000000000000LL);
```

<details>
<summary><b>ソースコードを展開 (DynamicSegTree.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

// 動的セグメント木 (Dynamic Segment Tree / Sparse Segment Tree)
// 座標が 0 <= p < MAX_COORD (例: 10^9 や 10^18) のような巨大な範囲で、
// オンデマンドにノードを生成しながら点更新・区間積・二分探索を O(log(MAX)) で処理する。
//
// S: 要素の型
// op: 2つの S を結合する演算
// e: 単位元を返す関数
template <class S, S (*op)(S, S), S (*e)()> struct DynamicSegTree {
  using Index = long long;

  struct Node {
    S val;
    int left, right; // 子ノードのインデックス (-1 は未生成)
    Node(S val) : val(val), left(-1), right(-1) {}
  };

  Index L, R; // カバーする座標範囲 [L, R)
  vector<Node> pool;
  int root;

  // デフォルトコンストラクタ: 範囲 [0, max_range)
  // max_range のデフォルトは 1LL << 30 (約 1.07 * 10^9)
  DynamicSegTree(Index max_range = (1LL << 30))
      : DynamicSegTree(0, max_range) {}

  DynamicSegTree(Index l, Index r) : L(l), R(r) { clear(); }

  void clear() {
    pool.clear();
    root = new_node();
  }

  // p 番目の要素を x に更新 (O(log(MAX)))
  void set(Index p, S x) {
    assert(L <= p && p < R);
    set(root, L, R, p, x);
  }

  // p 番目の要素に x を作用: A[p] = op(A[p], x) (O(log(MAX)))
  void apply(Index p, S x) {
    assert(L <= p && p < R);
    apply(root, L, R, p, x);
  }

  // p 番目の要素を取得 (O(log(MAX)))
  S get(Index p) const {
    assert(L <= p && p < R);
    return get(root, L, R, p);
  }

  // 半開区間 [l, r) の総積を取得 (l >= r の場合は e()) (O(log(MAX)))
  S prod(Index l, Index r) const {
    if (l >= r)
      return e();
    assert(L <= l && r <= R);
    return prod(root, L, R, l, r);
  }

  // 全区間 [L, R) の総積を取得 (O(1))
  S all_prod() const { return pool[root].val; }

  // 二分探索: f(prod(l, r)) = true となる最大の r を求める (O(log(MAX)))
  template <class F> Index max_right(Index l, F f) const {
    assert(L <= l && l <= R);
    assert(f(e()));
    if (l == R)
      return R;
    S current = e();
    return max_right(root, L, R, l, f, current);
  }

  // 二分探索: f(prod(l, r)) = true となる最小の l を求める (O(log(MAX)))
  template <class F> Index min_left(Index r, F f) const {
    assert(L <= r && r <= R);
    assert(f(e()));
    if (r == L)
      return L;
    S current = e();
    return min_left(root, L, R, r, f, current);
  }

  // 生成されたノード数を取得
  int size() const { return pool.size(); }

private:
  int new_node() {
    pool.emplace_back(e());
    return (int)pool.size() - 1;
  }

  void set(int u, Index l, Index r, Index p, S x) {
    if (r - l == 1) {
      pool[u].val = x;
      return;
    }
    Index m = l + ((r - l) >> 1);
    if (p < m) {
      if (pool[u].left == -1) {
        int nxt = new_node();
        pool[u].left = nxt;
      }
      set(pool[u].left, l, m, p, x);
    } else {
      if (pool[u].right == -1) {
        int nxt = new_node();
        pool[u].right = nxt;
      }
      set(pool[u].right, m, r, p, x);
    }
    update(u);
  }

  void apply(int u, Index l, Index r, Index p, S x) {
    if (r - l == 1) {
      pool[u].val = op(pool[u].val, x);
      return;
    }
    Index m = l + ((r - l) >> 1);
    if (p < m) {
      if (pool[u].left == -1) {
        int nxt = new_node();
        pool[u].left = nxt;
      }
      apply(pool[u].left, l, m, p, x);
    } else {
      if (pool[u].right == -1) {
        int nxt = new_node();
        pool[u].right = nxt;
      }
      apply(pool[u].right, m, r, p, x);
    }
    update(u);
  }

  S get(int u, Index l, Index r, Index p) const {
    if (u == -1)
      return e();
    if (r - l == 1)
      return pool[u].val;
    Index m = l + ((r - l) >> 1);
    if (p < m)
      return get(pool[u].left, l, m, p);
    else
      return get(pool[u].right, m, r, p);
  }

  S prod(int u, Index l, Index r, Index ql, Index qr) const {
    if (u == -1 || qr <= l || r <= ql)
      return e();
    if (ql <= l && r <= qr)
      return pool[u].val;
    Index m = l + ((r - l) >> 1);
    return op(prod(pool[u].left, l, m, ql, qr),
              prod(pool[u].right, m, r, ql, qr));
  }

  void update(int u) {
    S lv = (pool[u].left == -1 ? e() : pool[pool[u].left].val);
    S rv = (pool[u].right == -1 ? e() : pool[pool[u].right].val);
    pool[u].val = op(lv, rv);
  }

  template <class F>
  Index max_right(int u, Index l, Index r, Index ql, F f, S &current) const {
    if (u == -1) {
      // 未生成区間はすべて e() なので current は変化しない
      return min(r, R);
    }
    if (r <= ql)
      return r;
    if (ql <= l) {
      S nxt = op(current, pool[u].val);
      if (f(nxt)) {
        current = nxt;
        return r;
      }
      if (r - l == 1)
        return l;
    }
    Index m = l + ((r - l) >> 1);
    Index res = max_right(pool[u].left, l, m, ql, f, current);
    if (res < m)
      return res;
    return max_right(pool[u].right, m, r, ql, f, current);
  }

  template <class F>
  Index min_left(int u, Index l, Index r, Index qr, F f, S &current) const {
    if (u == -1) {
      return max(l, L);
    }
    if (qr <= l)
      return l;
    if (r <= qr) {
      S nxt = op(pool[u].val, current);
      if (f(nxt)) {
        current = nxt;
        return l;
      }
      if (r - l == 1)
        return r;
    }
    Index m = l + ((r - l) >> 1);
    Index res = min_left(pool[u].right, m, r, qr, f, current);
    if (res > m)
      return res;
    return min_left(pool[u].left, l, m, qr, f, current);
  }
};
```

</details>

---

#### PersistentSegTree (永続セグ木)
更新を行うたびに新しい根ノードを生成し、過去のすべてのバージョンに対するクエリ・二分探索を保持する完全永続セグメント木です。
- **ファイル**: `mylib/PersistentSegTree.hpp`
- **典型用途**: 区間内に特定範囲 $[L, R]$ の値が何個あるか、区間 $k$ 番目の値、バージョン履歴分岐
- **計算量**: 1点更新 $O(\log N)$（ノード生成 $O(\log N)$ 個）、区間積 $O(\log N)$

```cpp
#include "mylib/PersistentSegTree.hpp"

long long op_add(long long a, long long b) { return a + b; }
long long e_zero() { return 0; }

PersistentSegTree<long long, op_add, e_zero> seg(N);
int v0 = seg.build(initial_vector); // 初期バージョン
int v1 = seg.set(v0, index, val);   // v0 を元に index を更新したバージョン v1 を生成
long long res_v0 = seg.prod(v0, l, r);
long long res_v1 = seg.prod(v1, l, r);
```

<details>
<summary><b>ソースコードを展開 (PersistentSegTree.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

// 完全永続セグメント木 (Fully Persistent Segment Tree)
// 各更新で新しい根ノードのインデックス (version) を返し、過去のあらゆるバージョンに対する
// 点更新・区間取得・k番目の値・二分探索を O(log N) で行える。
//
// S: 要素の型
// op: 2つの S を結合する演算
// e: 単位元を返す関数
template <class S, S (*op)(S, S), S (*e)()> struct PersistentSegTree {
  struct Node {
    S val;
    int left = -1, right = -1;
    Node(S val) : val(val), left(-1), right(-1) {}
    Node(S val, int l, int r) : val(val), left(l), right(r) {}
  };

  int n;
  vector<Node> pool;

  PersistentSegTree() : n(0) {}
  PersistentSegTree(int n) : n(n) {}

  // 初期配列 v から初期バージョン (根ノードID) を O(N) で構築
  int build(const vector<S> &v) {
    n = v.size();
    if (n == 0)
      return -1;
    return build_rec(0, n, v);
  }

  // 空 (すべて e()) の初期バージョン (根ノードID) を作成
  int build_empty(int size) {
    n = size;
    if (n == 0)
      return -1;
    return build_empty_rec(0, n);
  }

  // version root の状態をベースに A[p] = x とした新しいバージョンの根IDを返す (O(log N))
  int set(int root, int p, S x) {
    assert(0 <= p && p < n);
    return set_rec(root, 0, n, p, x);
  }

  // version root の状態をベースに A[p] = op(A[p], x) とした新しいバージョンの根IDを返す (O(log N))
  int apply(int root, int p, S x) {
    assert(0 <= p && p < n);
    return apply_rec(root, 0, n, p, x);
  }

  // version root における A[p] を取得 (O(log N))
  S get(int root, int p) const {
    assert(0 <= p && p < n);
    return get_rec(root, 0, n, p);
  }

  // version root における区間 [l, r) の総積を取得 (O(log N))
  S prod(int root, int l, int r) const {
    if (l >= r)
      return e();
    assert(0 <= l && r <= n);
    return prod_rec(root, 0, n, l, r);
  }

  // version root の全区間の総積
  S all_prod(int root) const {
    if (root == -1)
      return e();
    return pool[root].val;
  }

  // 【頻出典型】区間 k 番目の値 (k-th element)
  // 永続カウントセグ木において、root_r - root_l の差分を取り、
  // 区間 [0, n) の中で k 番目 (0-indexed: 0 <= k < total_count) のインデックスを O(log N) で見つける
  // ※ S が数値カウント型 (加算が op) であることを前提とします
  int kth_element(int root_l, int root_r, S k) const {
    assert(k >= 0);
    return kth_element_rec(root_l, root_r, 0, n, k);
  }

  // 二分探索: f(prod(l, r)) = true となる最大の r を求める (O(log N))
  template <class F> int max_right(int root, int l, F f) const {
    assert(0 <= l && l <= n);
    assert(f(e()));
    if (l == n)
      return n;
    S current = e();
    return max_right_rec(root, 0, n, l, f, current);
  }

  // 二分探索: f(prod(l, r)) = true となる最小の l を求める (O(log N))
  template <class F> int min_left(int root, int r, F f) const {
    assert(0 <= r && r <= n);
    assert(f(e()));
    if (r == 0)
      return 0;
    S current = e();
    return min_left_rec(root, 0, n, r, f, current);
  }

  // 生成された全ノード数を取得
  int node_count() const { return pool.size(); }

private:
  int new_node(S val, int l = -1, int r = -1) {
    pool.emplace_back(val, l, r);
    return (int)pool.size() - 1;
  }

  int build_rec(int l, int r, const vector<S> &v) {
    if (r - l == 1) {
      return new_node(v[l]);
    }
    int m = (l + r) / 2;
    int lc = build_rec(l, m, v);
    int rc = build_rec(m, r, v);
    return new_node(op(pool[lc].val, pool[rc].val), lc, rc);
  }

  int build_empty_rec(int l, int r) {
    if (r - l == 1) {
      return new_node(e());
    }
    int m = (l + r) / 2;
    int lc = build_empty_rec(l, m);
    int rc = build_empty_rec(m, r);
    return new_node(e(), lc, rc);
  }

  int set_rec(int u, int l, int r, int p, S x) {
    if (r - l == 1) {
      return new_node(x);
    }
    int m = (l + r) / 2;
    int lc = (u == -1 ? -1 : pool[u].left);
    int rc = (u == -1 ? -1 : pool[u].right);
    if (p < m) {
      lc = set_rec(lc, l, m, p, x);
    } else {
      rc = set_rec(rc, m, r, p, x);
    }
    S lv = (lc == -1 ? e() : pool[lc].val);
    S rv = (rc == -1 ? e() : pool[rc].val);
    return new_node(op(lv, rv), lc, rc);
  }

  int apply_rec(int u, int l, int r, int p, S x) {
    if (r - l == 1) {
      S cur = (u == -1 ? e() : pool[u].val);
      return new_node(op(cur, x));
    }
    int m = (l + r) / 2;
    int lc = (u == -1 ? -1 : pool[u].left);
    int rc = (u == -1 ? -1 : pool[u].right);
    if (p < m) {
      lc = apply_rec(lc, l, m, p, x);
    } else {
      rc = apply_rec(rc, m, r, p, x);
    }
    S lv = (lc == -1 ? e() : pool[lc].val);
    S rv = (rc == -1 ? e() : pool[rc].val);
    return new_node(op(lv, rv), lc, rc);
  }

  S get_rec(int u, int l, int r, int p) const {
    if (u == -1)
      return e();
    if (r - l == 1)
      return pool[u].val;
    int m = (l + r) / 2;
    if (p < m)
      return get_rec(pool[u].left, l, m, p);
    else
      return get_rec(pool[u].right, m, r, p);
  }

  S prod_rec(int u, int l, int r, int ql, int qr) const {
    if (u == -1 || qr <= l || r <= ql)
      return e();
    if (ql <= l && r <= qr)
      return pool[u].val;
    int m = (l + r) / 2;
    return op(prod_rec(pool[u].left, l, m, ql, qr),
              prod_rec(pool[u].right, m, r, ql, qr));
  }

  int kth_element_rec(int ul, int ur, int l, int r, S k) const {
    if (r - l == 1)
      return l;
    int m = (l + r) / 2;
    int l_left = (ul == -1 ? -1 : pool[ul].left);
    int r_left = (ur == -1 ? -1 : pool[ur].left);
    S cnt_l = (l_left == -1 ? e() : pool[l_left].val);
    S cnt_r = (r_left == -1 ? e() : pool[r_left].val);
    S left_count = cnt_r - cnt_l; // [l, m) の区間内の出現頻度
    if (k < left_count) {
      return kth_element_rec(l_left, r_left, l, m, k);
    } else {
      int l_right = (ul == -1 ? -1 : pool[ul].right);
      int r_right = (ur == -1 ? -1 : pool[ur].right);
      return kth_element_rec(l_right, r_right, m, r, k - left_count);
    }
  }

  template <class F>
  int max_right_rec(int u, int l, int r, int ql, F f, S &current) const {
    if (u == -1)
      return min(r, n);
    if (r <= ql)
      return r;
    if (ql <= l) {
      S nxt = op(current, pool[u].val);
      if (f(nxt)) {
        current = nxt;
        return r;
      }
      if (r - l == 1)
        return l;
    }
    int m = (l + r) / 2;
    int res = max_right_rec(pool[u].left, l, m, ql, f, current);
    if (res < m)
      return res;
    return max_right_rec(pool[u].right, m, r, ql, f, current);
  }

  template <class F>
  int min_left_rec(int u, int l, int r, int qr, F f, S &current) const {
    if (u == -1)
      return max(l, 0);
    if (qr <= l)
      return l;
    if (r <= qr) {
      S nxt = op(pool[u].val, current);
      if (f(nxt)) {
        current = nxt;
        return l;
      }
      if (r - l == 1)
        return r;
    }
    int m = (l + r) / 2;
    int res = min_left_rec(pool[u].right, m, r, qr, f, current);
    if (res > m)
      return res;
    return min_left_rec(pool[u].left, l, m, qr, f, current);
  }
};
```

</details>

---

#### SqrtDecomposition (平方分割)
ACL `segtree` と同一のインターフェースを持つ平方分割ライブラリです。
- **ファイル**: `mylib/SqrtDecomposition.hpp`
- **計算量**: 構築 $O(N)$、1点更新 $O(1)$、区間取得 $O(\sqrt{N})$、二分探索 (`max_right`, `min_left`) $O(\sqrt{N})$

```cpp
#include "mylib/SqrtDecomposition.hpp"

long long op_min(long long a, long long b) { return min(a, b); }
long long e_min() { return 2e18; }

SqrtDecomposition<long long, op_min, e_min> sq(a);
sq.set(i, val);
long long ans = sq.prod(l, r);
int idx = sq.max_right(l, [](long long v) { return v >= 10; });
```

<details>
<summary><b>ソースコードを展開 (SqrtDecomposition.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

// 平方分割 (Sqrt Decomposition) - ACL segtree 完全互換
// S: 要素の型
// op: 2つの S を結合する演算
// e: 単位元を返す関数
template <class S, S (*op)(S, S), S (*e)()> struct SqrtDecomposition {
  int n;
  int block_size;
  int num_blocks;
  vector<S> data;
  vector<S> block;

  SqrtDecomposition() : SqrtDecomposition(0) {}
  explicit SqrtDecomposition(int n, int b_size = 0)
      : SqrtDecomposition(vector<S>(n, e()), b_size) {}

  explicit SqrtDecomposition(const vector<S> &v, int b_size = 0) : n(v.size()) {
    if (b_size <= 0) {
      block_size = max(1, (int)sqrt(n));
    } else {
      block_size = b_size;
    }
    num_blocks = (n + block_size - 1) / block_size;
    data = v;
    block.assign(num_blocks, e());
    for (int k = 0; k < num_blocks; k++) {
      update_block(k);
    }
  }

  // p 番目の要素を x に更新 (O(B) = O(√N))
  void set(int p, S x) {
    assert(0 <= p && p < n);
    data[p] = x;
    update_block(p / block_size);
  }

  // p 番目の要素を取得 (O(1))
  S get(int p) const {
    assert(0 <= p && p < n);
    return data[p];
  }

  // 半開区間 [l, r) の総積を取得 (O(√N))
  S prod(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    if (l == r)
      return e();

    int kl = l / block_size;
    int kr = (r - 1) / block_size;

    if (kl == kr) {
      S res = e();
      for (int i = l; i < r; i++) {
        res = op(res, data[i]);
      }
      return res;
    }

    S res = e();
    // 左端の半端部分
    int l_end = (kl + 1) * block_size;
    for (int i = l; i < l_end; i++) {
      res = op(res, data[i]);
    }
    // 中間の完全ブロック
    for (int k = kl + 1; k < kr; k++) {
      res = op(res, block[k]);
    }
    // 右端の半端部分
    int r_start = kr * block_size;
    for (int i = r_start; i < r; i++) {
      res = op(res, data[i]);
    }
    return res;
  }

  // 全区間 [0, n) の総積を取得 (O(√N))
  S all_prod() const {
    S res = e();
    for (int k = 0; k < num_blocks; k++) {
      res = op(res, block[k]);
    }
    return res;
  }

  // 二分探索: f(prod(l, r)) = true となる最大の r を求める (O(√N))
  template <class F> int max_right(int l, F f) const {
    assert(0 <= l && l <= n);
    assert(f(e()));
    if (l == n)
      return n;

    S current = e();
    int k = l / block_size;

    // 左端の半端部分
    int block_end = min(n, (k + 1) * block_size);
    for (int i = l; i < block_end; i++) {
      S nxt = op(current, data[i]);
      if (!f(nxt))
        return i;
      current = nxt;
    }
    k++;

    // 中間のブロック
    while (k < num_blocks) {
      S nxt = op(current, block[k]);
      if (!f(nxt)) {
        // このブロック内で探索
        int start = k * block_size;
        int end = min(n, (k + 1) * block_size);
        for (int i = start; i < end; i++) {
          S nxt_elem = op(current, data[i]);
          if (!f(nxt_elem))
            return i;
          current = nxt_elem;
        }
      }
      current = nxt;
      k++;
    }
    return n;
  }

  // 二分探索: f(prod(l, r)) = true となる最小の l を求める (O(√N))
  template <class F> int min_left(int r, F f) const {
    assert(0 <= r && r <= n);
    assert(f(e()));
    if (r == 0)
      return 0;

    S current = e();
    int k = (r - 1) / block_size;

    // 右端の半端部分
    int block_start = k * block_size;
    for (int i = r - 1; i >= block_start; i--) {
      S nxt = op(data[i], current);
      if (!f(nxt))
        return i + 1;
      current = nxt;
    }
    k--;

    // 中間のブロック
    while (k >= 0) {
      S nxt = op(block[k], current);
      if (!f(nxt)) {
        // このブロック内で探索
        int start = k * block_size;
        int end = min(n, (k + 1) * block_size);
        for (int i = end - 1; i >= start; i--) {
          S nxt_elem = op(data[i], current);
          if (!f(nxt_elem))
            return i + 1;
          current = nxt_elem;
        }
      }
      current = nxt;
      k--;
    }
    return 0;
  }

private:
  void update_block(int k) {
    int start = k * block_size;
    int end = min(n, (k + 1) * block_size);
    S res = e();
    for (int i = start; i < end; i++) {
      res = op(res, data[i]);
    }
    block[k] = res;
  }
};
```

</details>

---

#### LazySqrtDecomposition (遅延平方分割)
ACL `lazy_segtree` と同一のインターフェースを持つ遅延評価平方分割ライブラリです。
- **ファイル**: `mylib/LazySqrtDecomposition.hpp`
- **計算量**: 区間作用 $O(\sqrt{N})$、区間取得 $O(\sqrt{N})$、二分探索 $O(\sqrt{N})$

```cpp
#include "mylib/LazySqrtDecomposition.hpp"

// S, op, e, F, mapping, composition, id を定義して渡す
LazySqrtDecomposition<S, op, e, F, mapping, composition, id> lsq(n);
lsq.apply(l, r, f);            // 区間 [l, r) に作用素 f を適用
S res = lsq.prod(l, r);        // 区間 [l, r) の積を取得
```

<details>
<summary><b>ソースコードを展開 (LazySqrtDecomposition.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

// 遅延評価平方分割 (Lazy Sqrt Decomposition) - ACL lazy_segtree 完全互換
// S: 要素の型
// op: 2つの S を結合する演算
// e: 単位元を返す関数
// F: 作用素の型
// mapping: 作用素 f を要素 s に適用する関数 mapping(f, s)
// composition: 作用素 f, g を合成する関数 composition(f, g) (先に g を適用し、その後に f を適用)
// id: 恒等写像を返す関数
template <class S, S (*op)(S, S), S (*e)(), class F, S (*mapping)(F, S),
          F (*composition)(F, F), F (*id)()>
struct LazySqrtDecomposition {
  int n;
  int block_size;
  int num_blocks;
  vector<S> data;
  vector<S> block_val;
  vector<F> lazy;
  vector<bool> has_lazy;

  LazySqrtDecomposition() : LazySqrtDecomposition(0) {}
  explicit LazySqrtDecomposition(int n, int b_size = 0)
      : LazySqrtDecomposition(vector<S>(n, e()), b_size) {}

  explicit LazySqrtDecomposition(const vector<S> &v, int b_size = 0)
      : n(v.size()) {
    if (b_size <= 0) {
      block_size = max(1, (int)sqrt(n));
    } else {
      block_size = b_size;
    }
    num_blocks = (n + block_size - 1) / block_size;
    data = v;
    block_val.assign(num_blocks, e());
    lazy.assign(num_blocks, id());
    has_lazy.assign(num_blocks, false);
    for (int k = 0; k < num_blocks; k++) {
      update_block(k);
    }
  }

  // p 番目の要素を x に更新 (O(√N))
  void set(int p, S x) {
    assert(0 <= p && p < n);
    int k = p / block_size;
    push(k);
    data[p] = x;
    update_block(k);
  }

  // p 番目の要素を取得 (O(√N))
  S get(int p) {
    assert(0 <= p && p < n);
    int k = p / block_size;
    push(k);
    return data[p];
  }

  // p 番目の要素に作用素 f を適用 (O(√N))
  void apply(int p, F f) {
    assert(0 <= p && p < n);
    int k = p / block_size;
    push(k);
    data[p] = mapping(f, data[p]);
    update_block(k);
  }

  // 区間 [l, r) に作用素 f を適用 (O(√N))
  void apply(int l, int r, F f) {
    assert(0 <= l && l <= r && r <= n);
    if (l == r)
      return;

    int kl = l / block_size;
    int kr = (r - 1) / block_size;

    if (kl == kr) {
      push(kl);
      for (int i = l; i < r; i++) {
        data[i] = mapping(f, data[i]);
      }
      update_block(kl);
      return;
    }

    // 左端の半端部分
    push(kl);
    int l_end = (kl + 1) * block_size;
    for (int i = l; i < l_end; i++) {
      data[i] = mapping(f, data[i]);
    }
    update_block(kl);

    // 中間の完全ブロック
    for (int k = kl + 1; k < kr; k++) {
      apply_to_block(k, f);
    }

    // 右端の半端部分
    push(kr);
    int r_start = kr * block_size;
    for (int i = r_start; i < r; i++) {
      data[i] = mapping(f, data[i]);
    }
    update_block(kr);
  }

  // 半開区間 [l, r) の総積を取得 (O(√N))
  S prod(int l, int r) {
    assert(0 <= l && l <= r && r <= n);
    if (l == r)
      return e();

    int kl = l / block_size;
    int kr = (r - 1) / block_size;

    if (kl == kr) {
      push(kl);
      S res = e();
      for (int i = l; i < r; i++) {
        res = op(res, data[i]);
      }
      return res;
    }

    S res = e();
    // 左端の半端部分
    push(kl);
    int l_end = (kl + 1) * block_size;
    for (int i = l; i < l_end; i++) {
      res = op(res, data[i]);
    }

    // 中間の完全ブロック
    for (int k = kl + 1; k < kr; k++) {
      res = op(res, block_val[k]);
    }

    // 右端の半端部分
    push(kr);
    int r_start = kr * block_size;
    for (int i = r_start; i < r; i++) {
      res = op(res, data[i]);
    }

    return res;
  }

  // 全区間 [0, n) の総積を取得 (O(√N))
  S all_prod() const {
    S res = e();
    for (int k = 0; k < num_blocks; k++) {
      res = op(res, block_val[k]);
    }
    return res;
  }

  // 二分探索: g(prod(l, r)) = true となる最大の r を求める (O(√N))
  template <class G> int max_right(int l, G g) {
    assert(0 <= l && l <= n);
    assert(g(e()));
    if (l == n)
      return n;

    S current = e();
    int k = l / block_size;

    // 左端の半端部分
    push(k);
    int block_end = min(n, (k + 1) * block_size);
    for (int i = l; i < block_end; i++) {
      S nxt = op(current, data[i]);
      if (!g(nxt))
        return i;
      current = nxt;
    }
    k++;

    // 中間の完全ブロック
    while (k < num_blocks) {
      S nxt = op(current, block_val[k]);
      if (!g(nxt)) {
        // このブロック内で探索
        push(k);
        int start = k * block_size;
        int end = min(n, (k + 1) * block_size);
        for (int i = start; i < end; i++) {
          S nxt_elem = op(current, data[i]);
          if (!g(nxt_elem))
            return i;
          current = nxt_elem;
        }
      }
      current = nxt;
      k++;
    }
    return n;
  }

  // 二分探索: g(prod(l, r)) = true となる最小の l を求める (O(√N))
  template <class G> int min_left(int r, G g) {
    assert(0 <= r && r <= n);
    assert(g(e()));
    if (r == 0)
      return 0;

    S current = e();
    int k = (r - 1) / block_size;

    // 右端の半端部分
    push(k);
    int block_start = k * block_size;
    for (int i = r - 1; i >= block_start; i--) {
      S nxt = op(data[i], current);
      if (!g(nxt))
        return i + 1;
      current = nxt;
    }
    k--;

    // 中間の完全ブロック
    while (k >= 0) {
      S nxt = op(block_val[k], current);
      if (!g(nxt)) {
        // このブロック内で探索
        push(k);
        int start = k * block_size;
        int end = min(n, (k + 1) * block_size);
        for (int i = end - 1; i >= start; i--) {
          S nxt_elem = op(data[i], current);
          if (!g(nxt_elem))
            return i + 1;
          current = nxt_elem;
        }
      }
      current = nxt;
      k--;
    }
    return 0;
  }

private:
  void push(int k) {
    if (!has_lazy[k])
      return;
    int start = k * block_size;
    int end = min(n, (k + 1) * block_size);
    for (int i = start; i < end; i++) {
      data[i] = mapping(lazy[k], data[i]);
    }
    lazy[k] = id();
    has_lazy[k] = false;
  }

  void update_block(int k) {
    int start = k * block_size;
    int end = min(n, (k + 1) * block_size);
    S res = e();
    for (int i = start; i < end; i++) {
      res = op(res, data[i]);
    }
    block_val[k] = res;
  }

  void apply_to_block(int k, F f) {
    block_val[k] = mapping(f, block_val[k]);
    if (has_lazy[k]) {
      lazy[k] = composition(f, lazy[k]);
    } else {
      lazy[k] = f;
      has_lazy[k] = true;
    }
  }
};
```

</details>

---

#### OrderStatisticSet (順序付き集合)
Treap による平衡二分探索木です。重複要素を扱える multiset 版として動作し、値からインデックス、インデックスから値を高速に取得できます。
- **ファイル**: `mylib/OrderStatisticSet.hpp`（短縮エイリアス: `OSS<T>`）
- **計算量**: 挿入・削除・検索・$k$ 番目取得 いずれも $O(\log N)$

```cpp
#include "mylib/OrderStatisticSet.hpp"

OrderStatisticSet<long long> oss;
oss.insert(10);
oss.insert(20);
oss.insert(10); // 重複値もOK

int cnt = oss.order_of_key(15); // 15 未満の要素数
long long val = oss.kth(0);      // 昇順 0 番目 (最小値) の要素を取得
oss.erase_one(10);               // 10 を 1 つ削除
```

<details>
<summary><b>ソースコードを展開 (OrderStatisticSet.hpp)</b></summary>

```cpp
#pragma once
#include <iostream>
#include <utility>

// 平衡二分探索木 (Treap) による順序付き集合
template <typename T> class OrderStatisticSet {
private:
  // クラス内部に隠蔽した軽量乱数生成器 (Xorshift)
  class XorShift {
    uint64_t state;

  public:
    explicit XorShift(uint64_t seed = 88172645463325252ULL) : state(seed) {
      if (state == 0)
        state = 1;
    }
    uint32_t operator()() {
      state ^= state << 13;
      state ^= state >> 7;
      state ^= state << 17;
      return static_cast<uint32_t>(state);
    }
  };

  struct Node {
    T key;
    uint32_t priority;
    int size;
    Node *left = nullptr;
    Node *right = nullptr;

    Node(T k, uint32_t p) : key(std::move(k)), priority(p), size(1) {}
  };

  Node *root = nullptr;
  XorShift rng;

  // nullptr を安全に処理する内部用ヘルパー関数
  int get_size(Node *t) const { return t ? t->size : 0; }

  void update_size(Node *t) {
    if (t) {
      t->size = 1 + get_size(t->left) + get_size(t->right);
    }
  }

  void clear(Node *t) {
    if (!t)
      return;
    clear(t->left);
    clear(t->right);
    delete t;
  }

  Node *clone(Node *t) const {
    if (!t)
      return nullptr;
    Node *node = new Node(t->key, t->priority);
    node->size = t->size;
    node->left = clone(t->left);
    node->right = clone(t->right);
    return node;
  }

  std::pair<Node *, Node *> split_less(Node *t, const T &key) {
    if (!t)
      return {nullptr, nullptr};
    if (t->key < key) {
      auto [l, r] = split_less(t->right, key);
      t->right = l;
      update_size(t);
      return {t, r};
    } else {
      auto [l, r] = split_less(t->left, key);
      t->left = r;
      update_size(t);
      return {l, t};
    }
  }

  std::pair<Node *, Node *> split_less_equal(Node *t, const T &key) {
    if (!t)
      return {nullptr, nullptr};
    if (key < t->key) {
      auto [l, r] = split_less_equal(t->left, key);
      t->left = r;
      update_size(t);
      return {l, t};
    } else {
      auto [l, r] = split_less_equal(t->right, key);
      t->right = l;
      update_size(t);
      return {t, r};
    }
  }

  Node *merge(Node *l, Node *r) {
    if (!l || !r)
      return l ? l : r;
    if (l->priority > r->priority) {
      l->right = merge(l->right, r);
      update_size(l);
      return l;
    } else {
      r->left = merge(l, r->left);
      update_size(r);
      return r;
    }
  }

public:
  OrderStatisticSet() : rng() {}
  explicit OrderStatisticSet(uint64_t seed) : rng(seed) {}

  ~OrderStatisticSet() { clear(root); }

  OrderStatisticSet(const OrderStatisticSet &other) : rng(other.rng) {
    root = clone(other.root);
  }

  OrderStatisticSet &operator=(const OrderStatisticSet &other) {
    if (this != &other) {
      Node *new_root = clone(other.root);
      clear(root);
      root = new_root;
      rng = other.rng;
    }
    return *this;
  }

  OrderStatisticSet(OrderStatisticSet &&other) noexcept
      : root(other.root), rng(std::move(other.rng)) {
    other.root = nullptr;
  }

  OrderStatisticSet &operator=(OrderStatisticSet &&other) noexcept {
    if (this != &other) {
      clear(root);
      root = other.root;
      rng = std::move(other.rng);
      other.root = nullptr;
    }
    return *this;
  }

  // --- 公開インターフェース ---

  // 1. 全体の要素数
  int size() const { return get_size(root); }
  bool empty() const { return size() == 0; }

  // 2. 要素の追加・削除
  bool insert(T key) {
    auto [l, r] = split_less(root, key);
    auto [mid, r2] = split_less_equal(r, key);

    bool inserted = false;
    if (!mid) {
      try {
        mid = new Node(std::move(key), rng());
        inserted = true;
      } catch (...) {
        root = merge(l, r2);
        throw;
      }
    }
    root = merge(l, merge(mid, r2));
    return inserted;
  }

  bool erase(const T &key) {
    auto [l, r] = split_less(root, key);
    auto [mid, r2] = split_less_equal(r, key);

    bool erased = (mid != nullptr);
    clear(mid);
    root = merge(l, r2);
    return erased;
  }

  // 3. 値 x のインデックス (x未満の要素数)
  int get_index(const T &x) const {
    Node *curr = root;
    int index = 0;
    while (curr) {
      if (curr->key < x) {
        index += get_size(curr->left) + 1;
        curr = curr->right;
      } else {
        curr = curr->left;
      }
    }
    return index;
  }

  // 4. 下から k 番目の要素 (0-indexed)
  const T *get_from_bottom(int k) const {
    if (k < 0 || k >= size())
      return nullptr;
    Node *curr = root;
    while (curr) {
      int left_size = get_size(curr->left);
      if (k == left_size)
        return &(curr->key);
      if (k < left_size) {
        curr = curr->left;
      } else {
        k -= left_size + 1;
        curr = curr->right;
      }
    }
    return nullptr;
  }

  // 5. 上から k 番目の要素 (0-indexed)
  const T *get_from_top(int k) const {
    if (k < 0 || k >= size())
      return nullptr;
    return get_from_bottom(size() - 1 - k);
  }

  // 6. x 以上の最小の要素
  const T *lower_bound(const T &x) const {
    Node *curr = root;
    const T *res = nullptr;
    while (curr) {
      if (!(curr->key < x)) {
        res = &(curr->key);
        curr = curr->left;
      } else {
        curr = curr->right;
      }
    }
    return res;
  }

  // 7. x より大きい最小の要素
  const T *upper_bound(const T &x) const {
    Node *curr = root;
    const T *res = nullptr;
    while (curr) {
      if (x < curr->key) {
        res = &(curr->key);
        curr = curr->left;
      } else {
        curr = curr->right;
      }
    }
    return res;
  }
};

// エイリアス定義
template <typename T> using OSS = OrderStatisticSet<T>;
```

</details>

---

#### CoordinateCompression (座標圧縮)
1次元数列の値をソート・重複排除し、`0` から始まる連番に写像します。
- **ファイル**: `mylib/CoordinateCompression.hpp`
- **計算量**: 構築 $O(N \log N)$、検索 $O(\log N)$、復元 $O(1)$

```cpp
#include "mylib/CoordinateCompression.hpp"

// 1. 簡易版関数 (引数の vector をそのまま圧縮値に変換)
vector<int> compressed = compress(original_vector);

// 2. 構造体版 (双方向変換や二分探索クエリに対応)
CoordinateCompression<long long> cc(a);
int idx = cc(val);      // 元の値 val の圧縮後インデックス (0-indexed)
long long orig = cc[idx]; // 圧縮後インデックスから元の値を復元
int sz = cc.size();     // 異なる値の種類数
```

<details>
<summary><b>ソースコードを展開 (CoordinateCompression.hpp)</b></summary>

```cpp
#pragma once
#include <bits/stdc++.h>

using namespace std;

// 1次元の座標圧縮 (引数と同じ型の vector を返す)
template <typename T = long long>
vector<T> compress(vector<T> vec) {
  vector<T> c = vec;
  sort(c.begin(), c.end());
  c.erase(unique(c.begin(), c.end()), c.end());
  for (auto &i : vec) {
    i = lower_bound(c.begin(), c.end(), i) - c.begin();
  }
  return vec;
}

// 座標圧縮の構造体 (復元や二分探索クエリも行える拡張版)
template <typename T = long long>
struct CoordinateCompression {
  vector<T> vals;

  CoordinateCompression() {}
  CoordinateCompression(const vector<T> &vec) : vals(vec) {
    build();
  }

  void add(const T &x) { vals.push_back(x); }

  void build() {
    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
  }

  // 圧縮後のインデックス (0-indexed)
  int operator()(const T &x) const {
    return lower_bound(vals.begin(), vals.end(), x) - vals.begin();
  }

  // インデックスから元の値を取得
  const T &operator[](int i) const { return vals[i]; }

  int size() const { return vals.size(); }
};
```

</details>

---

### 2. 文字列 (Strings)

#### RollingHash (安全なダブルローリングハッシュ)
$2^{61}-1$（メルセンヌ素数）を法とし、実行時に安全な乱数基数を生成する耐撃墜性（Anti-Hack）の高いローリングハッシュです。
- **ファイル**: `mylib/RollingHash.hpp`（互換: `DoubleRollingHash.hpp`）
- **計算量**: 構築 $O(N)$、部分文字列ハッシュ $O(1)$、ハッシュ結合 $O(1)$、最長共通接頭辞 (LCP) $O(\log N)$

```cpp
#include "mylib/RollingHash.hpp"

string s = "abracadabra";
DoubleRollingHash rh(s);

// [l, r) のハッシュ値を取得
auto h1 = rh.get(0, 4); // "abra"
auto h2 = rh.get(7, 11); // "abra"
assert(h1 == h2);

// LCP (最長共通接頭辞) の長さを二分探索で取得
int len = rh.lcp(rh, 0, 7); // s[0...] と s[7...] の LCP -> 4
```

<details>
<summary><b>ソースコードを展開 (RollingHash.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <random>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// 2^61 - 1 を法とする安全なダブルローリングハッシュ
struct DoubleRollingHash {
  static const uint64_t mod = (1ULL << 61) - 1;
  uint64_t base1, base2;
  vector<uint64_t> hash1, power1;
  vector<uint64_t> hash2, power2;

  // 2^61 - 1 を法とする安全かつ高速な乗算
  static inline uint64_t mul(uint64_t a, uint64_t b) {
    __uint128_t t = (__uint128_t)a * b;
    t = (t >> 61) + (t & mod);
    if (t >= mod)
      t -= mod;
    return (uint64_t)t;
  }

  // 乱数による基数生成
  static inline uint64_t generate_base() {
    static mt19937_64 mt(
        chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<uint64_t> rand(2, mod - 2);
    return rand(mt);
  }

  // クラス全体で共有するデフォルト基数 (複数インスタンス間でハッシュを比較可能にする)
  static inline pair<uint64_t, uint64_t> get_default_bases() {
    static uint64_t b1 = 0, b2 = 0;
    if (b1 == 0) {
      b1 = generate_base();
      b2 = generate_base();
      while (b1 == b2) {
        b2 = generate_base();
      }
    }
    return {b1, b2};
  }

  DoubleRollingHash() : base1(0), base2(0) {}

  // 文字列から構築 (共有基数を使用)
  DoubleRollingHash(const string &s) {
    auto [b1, b2] = get_default_bases();
    init(s, b1, b2);
  }

  // 基数を明示的に指定して構築
  DoubleRollingHash(const string &s, uint64_t b1, uint64_t b2) {
    init(s, b1, b2);
  }

  // 数列 vector<T> から構築 (共有基数を使用)
  template <typename T> DoubleRollingHash(const vector<T> &v) {
    auto [b1, b2] = get_default_bases();
    init(v, b1, b2);
  }

  template <typename T>
  DoubleRollingHash(const vector<T> &v, uint64_t b1, uint64_t b2) {
    init(v, b1, b2);
  }

  void init(const string &s, uint64_t b1, uint64_t b2) {
    vector<uint64_t> v(s.begin(), s.end());
    init(v, b1, b2);
  }

  template <typename T>
  void init(const vector<T> &v, uint64_t b1, uint64_t b2) {
    base1 = b1;
    base2 = b2;
    int n = v.size();
    hash1.assign(n + 1, 0);
    power1.assign(n + 1, 1);
    hash2.assign(n + 1, 0);
    power2.assign(n + 1, 1);

    for (int i = 0; i < n; i++) {
      uint64_t val = (uint64_t)v[i] % mod;
      hash1[i + 1] = mul(hash1[i], base1) + val;
      if (hash1[i + 1] >= mod)
        hash1[i + 1] -= mod;
      power1[i + 1] = mul(power1[i], base1);

      hash2[i + 1] = mul(hash2[i], base2) + val;
      if (hash2[i + 1] >= mod)
        hash2[i + 1] -= mod;
      power2[i + 1] = mul(power2[i], base2);
    }
  }

  // [l, r) の部分列のハッシュ値をペアで取得 (0-indexed)
  pair<uint64_t, uint64_t> get(int l, int r) const {
    assert(0 <= l && l <= r && r < (int)hash1.size());
    uint64_t res1 = hash1[r] + mod - mul(hash1[l], power1[r - l]);
    if (res1 >= mod)
      res1 -= mod;

    uint64_t res2 = hash2[r] + mod - mul(hash2[l], power2[r - l]);
    if (res2 >= mod)
      res2 -= mod;

    return {res1, res2};
  }

  // ハッシュ同士の結合 (h1 の後ろに長さ len2 の h2 を連結)
  pair<uint64_t, uint64_t> concat(pair<uint64_t, uint64_t> h1,
                                 pair<uint64_t, uint64_t> h2, int len2) const {
    uint64_t res1 = mul(h1.first, power1[len2]) + h2.first;
    if (res1 >= mod)
      res1 -= mod;

    uint64_t res2 = mul(h1.second, power2[len2]) + h2.second;
    if (res2 >= mod)
      res2 -= mod;

    return {res1, res2};
  }

  // 2つの文字列 (または同一文字列の異なる区間) の最長共通接頭辞 (LCP) の長さを O(log N) で取得
  int lcp(const DoubleRollingHash &other, int l1 = 0, int l2 = 0) const {
    int max_len = min((int)hash1.size() - 1 - l1, (int)other.hash1.size() - 1 - l2);
    int low = 0, high = max_len + 1;
    while (high - low > 1) {
      int mid = (low + high) / 2;
      if (get(l1, l1 + mid) == other.get(l2, l2 + mid)) {
        low = mid;
      } else {
        high = mid;
      }
    }
    return low;
  }
};

// エイリアス定義
using RollingHash = DoubleRollingHash;
```

</details>

---

#### DynamicStringTreap (動的文字列平衡二分木)
文字列に対する任意位置への文字挿入・削除・更新・取得、および半開区間 $[l, r)$ 内に出現する異なる文字種数（小文字アルファベット）を $O(\log N)$ で処理します。
- **ファイル**: `mylib/DynamicStringTreap.hpp`
- **計算量**: 各種更新・クエリ $O(\log N)$

```cpp
#include "mylib/DynamicStringTreap.hpp"

DynamicStringTreap treap("atcoder");
treap.insert(2, 'x');                  // 2 文字目の直後に 'x' を挿入
treap.erase(0);                        // 0 文字目を削除
treap.set(1, 'z');                     // 1 文字目を 'z' に変更
char c = treap.get(3);                 // 3 文字目を取得
int distinct = treap.count_distinct(0, treap.size()); // 含まれる文字種数を取得
string str = treap.to_string();        // 全体文字列の復元
```

<details>
<summary><b>ソースコードを展開 (DynamicStringTreap.hpp)</b></summary>

```cpp
#pragma once
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

// 動的文字列平衡二分木 (Dynamic String Treap)
// 文字列に対する任意位置への文字挿入・削除・更新・取得、
// および区間内の異なる文字種数 (アルファベット小文字 'a'-'z') のカウントを O(log N) で処理する。
struct DynamicStringTreap {
  struct Node {
    char val;
    uint32_t priority;
    int size;
    int mask; // 出現文字のビットマスク (1 << (c - 'a'))
    int l, r; // 0 を null 扱いとするインデックス
  };

  vector<Node> tree;
  int root = 0;

  static inline uint32_t xor128() {
    static uint32_t x = 123456789, y = 362436069, z = 521288629,
                    w = 88675123;
    uint32_t t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
  }

  int new_node(char c) {
    int idx = tree.size();
    int mask = 0;
    if (c >= 'a' && c <= 'z') {
      mask = 1 << (c - 'a');
    }
    tree.push_back({c, xor128(), 1, mask, 0, 0});
    return idx;
  }

  void update(int t) {
    if (!t)
      return;
    tree[t].size = 1 + tree[tree[t].l].size + tree[tree[t].r].size;
    int m = 0;
    if (tree[t].val >= 'a' && tree[t].val <= 'z') {
      m = 1 << (tree[t].val - 'a');
    }
    tree[t].mask = m | tree[tree[t].l].mask | tree[tree[t].r].mask;
  }

  // [0, k) と [k, n) に分割
  void split(int t, int k, int &l, int &r) {
    if (!t) {
      l = r = 0;
      return;
    }
    int left_sz = tree[tree[t].l].size;
    if (k <= left_sz) {
      split(tree[t].l, k, l, tree[t].l);
      r = t;
      update(r);
    } else {
      split(tree[t].r, k - left_sz - 1, tree[t].r, r);
      l = t;
      update(l);
    }
  }

  // l と r を結合
  void merge(int &t, int l, int r) {
    if (!l || !r) {
      t = l ? l : r;
      return;
    }
    if (tree[l].priority > tree[r].priority) {
      merge(tree[l].r, tree[l].r, r);
      t = l;
    } else {
      merge(tree[r].l, l, tree[r].l);
      t = r;
    }
    update(t);
  }

  DynamicStringTreap() {
    clear();
  }

  explicit DynamicStringTreap(const string &s) {
    clear();
    tree.reserve(s.size() + 1);
    for (char c : s) {
      merge(root, root, new_node(c));
    }
  }

  void clear() {
    tree.clear();
    // 0 番ノードは null (ダミー) として確保
    tree.push_back({'\0', 0, 0, 0, 0, 0});
    root = 0;
  }

  int size() const { return tree[root].size; }
  bool empty() const { return size() == 0; }

  // i 番目の文字の直後に c を挿入 (i=0 のときは先頭に挿入) (O(log N))
  void insert(int i, char c) {
    assert(0 <= i && i <= size());
    int l = 0, r = 0;
    split(root, i, l, r);
    merge(l, l, new_node(c));
    merge(root, l, r);
  }

  // 末尾に文字 c を追加 (O(log N))
  void push_back(char c) { insert(size(), c); }

  // i 文字目を c に変更 (0-indexed) (O(log N))
  void set(int i, char c) {
    assert(0 <= i && i < size());
    int l = 0, m = 0, r = 0;
    split(root, i, l, r);
    split(r, 1, m, r);
    tree[m].val = c;
    update(m);
    merge(r, m, r);
    merge(root, l, r);
  }

  // i 文字目を取得 (0-indexed) (O(log N))
  char get(int i) {
    assert(0 <= i && i < size());
    int l = 0, m = 0, r = 0;
    split(root, i, l, r);
    split(r, 1, m, r);
    char res = tree[m].val;
    merge(r, m, r);
    merge(root, l, r);
    return res;
  }

  // i 文字目を削除 (0-indexed) (O(log N))
  void erase(int i) {
    assert(0 <= i && i < size());
    int l = 0, m = 0, r = 0;
    split(root, i, l, r);
    split(r, 1, m, r);
    // m (i文字目) はマージせずに破棄
    merge(root, l, r);
  }

  // 区間 [l, r) の文字種類数を取得 (0-indexed, 半開区間) (O(log N))
  int count_distinct(int l, int r) {
    if (l >= r || !root)
      return 0;
    assert(0 <= l && r <= size());
    int left_part = 0, mid = 0, right_part = 0;
    split(root, r, left_part, right_part);
    split(left_part, l, left_part, mid);

    int res = __builtin_popcount(tree[mid].mask);

    merge(left_part, left_part, mid);
    merge(root, left_part, right_part);
    return res;
  }

  // 現在の文字列全体を取得 (O(N))
  string to_string() {
    string res;
    res.reserve(size());
    dfs_dump(root, res);
    return res;
  }

  // 部分文字列 [l, r) を取得 (O(log N + (r - l)))
  string substr(int l, int r) {
    if (l >= r || !root)
      return "";
    assert(0 <= l && r <= size());
    int left_part = 0, mid = 0, right_part = 0;
    split(root, r, left_part, right_part);
    split(left_part, l, left_part, mid);

    string res;
    res.reserve(r - l);
    dfs_dump(mid, res);

    merge(left_part, left_part, mid);
    merge(root, left_part, right_part);
    return res;
  }

private:
  void dfs_dump(int t, string &res) const {
    if (!t)
      return;
    dfs_dump(tree[t].l, res);
    res += tree[t].val;
    dfs_dump(tree[t].r, res);
  }
};
```

</details>

---

#### Trie / MapTrie
プレフィックス木（Trie木）。小文字アルファベット用の高速な `Trie` と、任意のキー型を扱える `MapTrie<T>` を提供します。
- **ファイル**: `mylib/Trie.hpp`
- **計算量**: 挿入・検索 $O(|S|)$ または $O(|S| \log \Sigma)$

```cpp
#include "mylib/Trie.hpp"

// 小文字アルファベット用
Trie trie;
trie.add("apple");
trie.add("apply");
trie.build_aho_corasick(); // 必要に応じて Aho-Corasick マッチングオートマトンを構築

// 汎用型プレフィックス木 (例: 数列の辞書)
MapTrie<long long> mt;
mt.insert(vector<long long>{1, 5, 10});
```

<details>
<summary><b>ソースコードを展開 (Trie.hpp)</b></summary>

```cpp
#pragma once
#include <bits/stdc++.h>

using namespace std;

// T: 扱う値の型 (long long や int など)
template <typename T = long long> struct MapTrie {
  struct Node {
    map<T, int> nex;    // 遷移先（mapなので自動でキーの昇順＝辞書順になる）
    vector<int> accept; // このノードを終端とする数列のIDを格納
    int pass = 0;       // このノードを通過した回数
  };

  vector<Node> nodes;
  vector<int> id_to_node; // 数列ID -> Trie木上のノードID のマッピング

  // コンストラクタ（最大ノード数を渡してMLE/TLEを防止）
  MapTrie(int max_nodes = 300005) {
    nodes.reserve(max_nodes);
    id_to_node.reserve(max_nodes);

    nodes.emplace_back();    // 根ノード (ノードID: 0)
    id_to_node.push_back(0); // 初期状態 (数列ID: 0) は根ノードにいる
  }

  // 【操作1】既存の数列(parent_id)の末尾に値(val)を追加し、新しい数列(new_id)とする
  void append(int parent_id, T val, int new_id) {
    assert(0 <= parent_id && parent_id < (int)id_to_node.size());
    int cur = id_to_node[parent_id];
    assert(cur != -1);

    // 遷移先がなければ新しくノードを作る
    if (!nodes[cur].nex.count(val)) {
      int next_id = nodes.size();
      nodes[cur].nex[val] = next_id;
      nodes.emplace_back();
    }

    int nex_node = nodes[cur].nex[val];
    nodes[nex_node].pass++;
    nodes[nex_node].accept.push_back(new_id); // 添え字の小さい順に追加される

    // id_to_node のサイズを自動拡張して記録
    if (new_id >= (int)id_to_node.size()) {
      id_to_node.resize(new_id + 1, -1);
    }
    id_to_node[new_id] = nex_node;
  }

  // 【操作2】配列を丸ごと追加する（普通のTrie木としての使い方）
  void insert(const vector<T> &seq, int seq_id) {
    int cur = 0;
    nodes[0].pass++;
    for (const auto &val : seq) {
      if (!nodes[cur].nex.count(val)) {
        int next_id = nodes.size();
        nodes[cur].nex[val] = next_id;
        nodes.emplace_back();
      }
      cur = nodes[cur].nex[val];
      nodes[cur].pass++;
    }
    nodes[cur].accept.push_back(seq_id);
    if (seq_id >= (int)id_to_node.size()) {
      id_to_node.resize(seq_id + 1, -1);
    }
    id_to_node[seq_id] = cur;
  }

  // 内部用のDFS関数（辞書順に数列IDを回収）
  void dfs(int cur, vector<int> &res) const {
    for (int seq_id : nodes[cur].accept) {
      res.push_back(seq_id);
    }
    for (const auto &[val, nex_node] : nodes[cur].nex) {
      dfs(nex_node, res);
    }
  }

  // 辞書順に並べた数列のIDリストを取得
  vector<int> get_lexicographical_order() const {
    vector<int> res;
    res.reserve(nodes.size());
    dfs(0, res);
    return res;
  }
};

// 小文字アルファベット 'a'-'z' 用の Trie木 / Aho-Corasick 多相オートマトン
struct Trie {
  struct Node {
    vector<int> next;
    int fail = 0;
    bool isOk = true; // パターン終端でない(禁止文字列を含まない)フラグ
    int count = 0;   // このノードを終端とするパターンの個数
    Node() {
      next.assign(26, -1);
    }
  };

  vector<Node> nodes;
  int root = 0;

  Trie() { nodes.emplace_back(); }

  // 文字列パターンの追加
  void add(const string &s) {
    int id = 0;
    for (char c : s) {
      int idx = c - 'a';
      if (nodes[id].next[idx] == -1) {
        int next_id = nodes.size();
        nodes[id].next[idx] = next_id;
        nodes.emplace_back();
      }
      id = nodes[id].next[idx];
    }
    nodes[id].isOk = false;
    nodes[id].count++;
  }

  // Aho-Corasick automaton の構築 (fail辺と遷移関数の補完)
  void build() {
    queue<int> que;
    for (int i = 0; i < 26; i++) {
      if (nodes[0].next[i] != -1) {
        que.push(nodes[0].next[i]);
      } else {
        nodes[0].next[i] = 0;
      }
    }

    while (!que.empty()) {
      int t = que.front();
      que.pop();

      for (int i = 0; i < 26; i++) {
        if (nodes[t].next[i] != -1) {
          int v = nodes[t].next[i];
          nodes[v].fail = nodes[nodes[t].fail].next[i];
          nodes[v].isOk &= nodes[nodes[v].fail].isOk;
          nodes[v].count += nodes[nodes[v].fail].count;
          que.push(v);
        } else {
          nodes[t].next[i] = nodes[nodes[t].fail].next[i];
        }
      }
    }
  }

  // 文字列 text 内に登録されたいずれかのパターンが含まれているか判定
  bool contains_any(const string &text) const {
    int now = 0;
    for (char c : text) {
      now = nodes[now].next[c - 'a'];
      if (!nodes[now].isOk)
        return true;
    }
    return false;
  }
};
```

</details>

---

### 3. グラフ・木 (Graphs & Trees)

#### Dijkstra (単一始点最短経路)
非負重み付きグラフに対する単一始点最短経路法です。
- **ファイル**: `mylib/Dijkstra.hpp`
- **計算量**: $O((V + E) \log V)$（到達不可ノードは `-1` を返す）

```cpp
#include "mylib/Dijkstra.hpp"

int n = 5;
vector<vector<pair<int, long long>>> g(n);
g[u].push_back({v, cost});
g[v].push_back({u, cost});

vector<long long> dist = dijkstra(g, 0); // 始点 0 からの最短距離
```

<details>
<summary><b>ソースコードを展開 (Dijkstra.hpp)</b></summary>

```cpp
#pragma once
#include <bits/stdc++.h>

using namespace std;

// ダイクストラ法 (単一始点最短経路)
// g: 隣接リスト g[from] = {{to, cost}, ...}
// start_pos: 始点
// 戻り値: 各頂点への最短距離 (到達不可は -1)
vector<long long> dijkstra(const vector<vector<pair<int, long long>>> &g,
                           int start_pos) {
  int n = g.size();
  vector<long long> dist(n, -1);
  // {コスト, 頂点} の昇順優先度付きキュー (コストは long long)
  using P = pair<long long, int>;
  priority_queue<P, vector<P>, greater<P>> todo;

  todo.push({0, start_pos});
  while (!todo.empty()) {
    auto [c, p] = todo.top();
    todo.pop();
    if (dist[p] != -1) {
      continue;
    }
    dist[p] = c;
    for (const auto &edge : g[p]) {
      int to = edge.first;
      long long cost = edge.second;
      if (dist[to] == -1) {
        todo.push({c + cost, to});
      }
    }
  }
  return dist;
}

// スペルミス互換用エイリアス
inline vector<long long> dijksta(const vector<vector<pair<int, long long>>> &g,
                                 int start_pos) {
  return dijkstra(g, start_pos);
}
```

</details>

---

#### TopologicalSort (トポロジカルソート)
Kahn のアルゴリズム（入次数を用いたキュー処理）による有向グラフのトポロジカルソートです。閉路が存在する場合は空の配列を返します。
- **ファイル**: `mylib/TopologicalSort.hpp`
- **計算量**: $O(V + E)$

```cpp
#include "mylib/TopologicalSort.hpp"

vector<vector<int>> g(n);
g[from].push_back(to);

vector<int> topo = topological_sort(g);
if (topo.empty()) {
    cout << "グラフに閉路が存在します (DAG ではありません)" << endl;
} else {
    // topo にソート済み頂点順序が格納される
}
```

<details>
<summary><b>ソースコードを展開 (TopologicalSort.hpp)</b></summary>

```cpp
#pragma once
#include <bits/stdc++.h>

using namespace std;

// Kahnのアルゴリズムによるトポロジカルソート
// g: 有向グラフの隣接リスト
// 戻り値: トポロジカル順序の頂点列 (閉路が存在しDAGでない場合は空の配列を返す)
vector<int> topological_sort(const vector<vector<int>> &g) {
  int n = g.size();
  vector<int> in_degree(n, 0);
  for (int i = 0; i < n; i++) {
    for (int v : g[i])
      in_degree[v]++;
  }

  queue<int> q;
  for (int i = 0; i < n; i++) {
    if (in_degree[i] == 0)
      q.push(i);
  }

  vector<int> res;
  res.reserve(n);
  while (!q.empty()) {
    int u = q.front();
    q.pop();
    res.push_back(u);
    for (int v : g[u]) {
      in_degree[v]--;
      if (in_degree[v] == 0)
        q.push(v);
    }
  }

  // 閉路が含まれている（DAGでない）場合は空の配列を返す
  if ((int)res.size() != n)
    return {};
  return res;
}
```

</details>

---

#### Rerooting (全方位木DP)
任意の可換モノイド・作用に対する全方位木DP（Rerooting DP）を $O(N)$ で行います。
- **ファイル**: `mylib/Rerooting.hpp`
- **計算量**: 頂点数 $N$ に対して $O(N)$ 回の `merge` と `apply`

```cpp
#include "mylib/Rerooting.hpp"

using Cost = long long;
using Data = long long;

Data merge(Data a, Data b) { return max(a, b); }
Data e() { return 0; }
Data leaf() { return 0; }
Data apply(Data d, int from, int to, Cost c) { return d + c; }

Rerooting<Cost, Data, merge, e, leaf, apply> tree(n);
tree.add_bi_edge(u, v, cost);
vector<Data> ans = tree.run(); // ans[i] に頂点 i を根としたときの木全体の答え
```

<details>
<summary><b>ソースコードを展開 (Rerooting.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

// 辺の定義
template <typename Cost = long long> struct Edge {
  int src, to;
  Cost cost;
  Edge(int s, int t, Cost c = 1) : src(s), to(t), cost(c) {}
  operator int() const { return to; }
};

// グラフクラス
template <typename Cost = long long> struct Graph : vector<vector<Edge<Cost>>> {
  Graph(int n = 0) : vector<vector<Edge<Cost>>>(n) {}

  // 有向辺の追加 (片方向)
  void add_edge(int s, int t, Cost c = 1) {
    (*this)[s].emplace_back(s, t, c);
  }

  // 無向辺の追加 (両方向)
  void add_undirected_edge(int s, int t, Cost c = 1) {
    (*this)[s].emplace_back(s, t, c);
    (*this)[t].emplace_back(t, s, c);
  }
};

// =========================================================================
// 1. 全方位木DP (6.cpp 互換スタイル)
// =========================================================================
// Cost: 辺の重みの型 (通常 long long や int)
// Data: DPで保持する値の型
// merge: 2つの部分木の結果をマージする関数 (Data, Data) -> Data
// e: マージの単位元を返す関数 () -> Data
// leaf: 葉ノードの初期値を返す関数 () -> Data
// apply: 子部分木の値 memo[d] に辺 (d -> c, cost) を適用して親 c に渡す値を返す関数
//        (Data val, int from, int to, Cost cost) -> Data
template <typename Cost, typename Data,
          Data (*merge)(Data, Data),
          Data (*e)(),
          Data (*leaf)(),
          Data (*apply)(Data, int, int, Cost)>
struct Rerooting : Graph<Cost> {
  vector<Data> dp;   // 各頂点を根としたときの木全体の答え
  vector<Data> memo; // dfs1 での部分木の答え

  Rerooting(int n = 0) : Graph<Cost>(n) {}

  // 全方位木DPを実行し、各頂点を根としたときの答えの配列を返す
  vector<Data> run(int root = 0) {
    int n = this->size();
    if (n == 0)
      return {};
    if (n == 1) {
      dp.assign(1, leaf());
      return dp;
    }
    memo.assign(n, e());
    dp.assign(n, e());
    dfs1(root, -1);
    dfs2(root, -1, e());
    return dp;
  }

  // 互換用エイリアス
  vector<Data> solve(int root = 0) { return run(root); }

private:
  void dfs1(int c, int p) {
    bool upd = false;
    for (Edge<Cost> &d : (*this)[c]) {
      if (d == p)
        continue;
      dfs1(d, c);
      upd = true;
      memo[c] = merge(memo[c], apply(memo[d], d, c, d.cost));
    }
    if (!upd)
      memo[c] = leaf();
  }

  void dfs2(int c, int p, const Data &val) {
    vector<Data> ds{val};
    for (Edge<Cost> &d : (*this)[c]) {
      if (d == p)
        continue;
      ds.push_back(apply(memo[d], d, c, d.cost));
    }
    int n = ds.size(), idx = 1;
    vector<Data> head(n + 1, e()), tail(n + 1, e());
    for (int i = 0; i++ < n;)
      head[i] = merge(head[i - 1], ds[i - 1]);
    for (int i = n; i-- > 0;)
      tail[i] = merge(tail[i + 1], ds[i]);
    dp[c] = head[n];
    for (Edge<Cost> &d : (*this)[c]) {
      if (d == p)
        continue;
      Data sub = merge(head[idx], tail[idx + 1]);
      dfs2(d, c, apply(sub, c, d, d.cost));
      idx++;
    }
  }
};

// =========================================================================
// 2. 汎用全方位木DP (頂点作用 add_node 対応版)
// =========================================================================
// 頂点属性（頂点重み、頂点自身を1つカウントするなど）がある問題でも
// 葉や根を特別扱いすることなく自然かつ正確に解くことができるモデルです。
//
// merge: (Data, Data) -> Data (子同士のマージ)
// e: () -> Data (マージの単位元)
// apply_edge: (Data, int from, int to, Cost cost) -> Data (辺の伝搬)
// add_node: (Data, int v) -> Data (頂点 v の値を反映する。葉は add_node(e(), v) になる)
namespace rerooting_internal {
  template <typename Data>
  inline Data default_add_node(Data d, int) { return d; }
}

template <typename Cost, typename Data,
          Data (*merge)(Data, Data),
          Data (*e)(),
          Data (*apply_edge)(Data, int, int, Cost),
          Data (*add_node)(Data, int) = rerooting_internal::default_add_node<Data>>
struct RerootingTree : Graph<Cost> {
  vector<Data> dp;     // 各頂点を根としたときの木全体の答え
  vector<Data> sub_dp; // dfs1 における部分木の答え

  RerootingTree(int n = 0) : Graph<Cost>(n) {}

  vector<Data> run(int root = 0) {
    int n = this->size();
    if (n == 0) return {};
    if (n == 1) {
      dp.assign(1, add_node(e(), 0));
      return dp;
    }
    dp.assign(n, e());
    sub_dp.assign(n, e());
    dfs1(root, -1);
    dfs2(root, -1, e());
    return dp;
  }

  vector<Data> solve(int root = 0) { return run(root); }

private:
  void dfs1(int c, int p) {
    Data res = e();
    for (Edge<Cost> &d : (*this)[c]) {
      if (d == p) continue;
      dfs1(d, c);
      res = merge(res, apply_edge(sub_dp[d], d, c, d.cost));
    }
    sub_dp[c] = add_node(res, c);
  }

  void dfs2(int c, int p, const Data &val_from_parent) {
    int deg = (*this)[c].size();
    vector<Data> ds;
    ds.reserve(deg);

    for (Edge<Cost> &d : (*this)[c]) {
      if (d == p) {
        ds.push_back(val_from_parent);
      } else {
        ds.push_back(apply_edge(sub_dp[d], d, c, d.cost));
      }
    }

    int sz = ds.size();
    vector<Data> head(sz + 1, e()), tail(sz + 1, e());
    for (int i = 0; i < sz; i++) {
      head[i + 1] = merge(head[i], ds[i]);
    }
    for (int i = sz - 1; i >= 0; i--) {
      tail[i] = merge(ds[i], tail[i + 1]);
    }

    dp[c] = add_node(head[sz], c);

    for (int i = 0; i < sz; i++) {
      Edge<Cost> &d = (*this)[c][i];
      if (d == p) continue;
      Data without_d = merge(head[i], tail[i + 1]);
      Data to_child = apply_edge(add_node(without_d, c), c, d, d.cost);
      dfs2(d, c, to_child);
    }
  }
};
```

</details>

---

### 4. 数学・数論 (Math & Number Theory)

#### NumberTheory (高速数論・素因数分解・CRT)
$10^{18}$ までの巨大整数に対応する高度数論関数群です。
- **ファイル**: `mylib/NumberTheory.hpp`
- **含まれる関数**:
  - `extgcd(a, b, x, y)`: 拡張ユークリッド互除法 ($ax + by = \gcd(a, b)$)
  - `modinv(a, m)`: $\gcd(a, m) = 1$ のときのモジュラ逆元
  - `crt(r, m)`: 中国剰余定理 ($x \equiv r_i \pmod{m_i}$)
  - `miller_rabin(n)`: ミラー・ラビン素数判定法 ($N \le 10^{18}$ を決定論的かつ高速に判定)
  - `pollard_rho(n)`: ポラード・ロー素因数探索 ($O(N^{1/4})$)
  - `factorize(n)`: 素因数分解 (`vector<pair<long long, int>>` を昇順で返す)
  - `divisors(n)`: 正の約数列挙 (昇順ソート済みで返す)

```cpp
#include "mylib/NumberTheory.hpp"

// 素数判定
if (miller_rabin(n)) { /* 素数 */ }

// 素因数分解
auto factors = factorize(120); // {{2, 3}, {3, 1}, {5, 1}}

// 約数列挙
auto divs = divisors(120); // {1, 2, 3, 4, 5, 6, 8, 10, ...}

// 中国剰余定理
vector<long long> r = {2, 3};
vector<long long> m = {3, 5};
auto [rem, mod] = crt(r, m); // x = rem (mod lcm(m))
```

<details>
<summary><b>ソースコードを展開 (NumberTheory.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

using namespace std;

// 高速数論ライブラリ (Number Theory Library)
// - Miller-Rabin 素数判定法 (2^64 未満で決定論的に O(log N))
// - Pollard's rho 素因数分解法 (N <= 10^18 を O(N^{1/4}))
// - LinearSieve (osa_k法 / 線形篩による O(log x) 素因数分解テーブル)
// - 中国剰余定理 (Extended CRT)
// - 拡張ユークリッド互除法 (extgcd) / mod逆元 (mod_inv)
namespace NumberTheory {

// 2^64 未満の安全な剰余乗算 (a * b) % mod
inline uint64_t mul_mod(uint64_t a, uint64_t b, uint64_t mod) {
  return (uint64_t)((__uint128_t)a * b % mod);
}

// べき乗剰余 (a^b) % mod
inline uint64_t pow_mod(uint64_t a, uint64_t b, uint64_t mod) {
  uint64_t res = 1 % mod;
  a %= mod;
  while (b > 0) {
    if (b & 1)
      res = mul_mod(res, a, mod);
    a = mul_mod(a, a, mod);
    b >>= 1;
  }
  return res;
}

// 拡張ユークリッドの互除法: a*x + b*y = gcd(a, b) を満たす (x, y) を求め gcd(a, b) を返す
inline long long extgcd(long long a, long long b, long long &x, long long &y) {
  if (b == 0) {
    x = 1;
    y = 0;
    return a;
  }
  long long d = extgcd(b, a % b, y, x);
  y -= a / b * x;
  return d;
}

// モジュラ逆元 (gcd(a, mod) == 1 を前提とする)
inline long long mod_inv(long long a, long long mod) {
  long long x, y;
  long long g = extgcd(a, mod, x, y);
  assert(g == 1);
  return (x % mod + mod) % mod;
}

// --- Miller-Rabin 素数判定法 (2^64 未満で決定論的に動作) ---
inline bool is_prime(uint64_t n) {
  if (n <= 1)
    return false;
  if (n == 2 || n == 3 || n == 5 || n == 7)
    return true;
  if (n % 2 == 0 || n % 3 == 0 || n % 5 == 0 || n % 7 == 0)
    return false;
  if (n < 121)
    return true; // 11^2

  uint64_t d = n - 1;
  int s = 0;
  while ((d & 1) == 0) {
    d >>= 1;
    s++;
  }

  // 2^64 未満で誤判定のない基数集合
  static const uint64_t bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
  for (uint64_t a : bases) {
    if (n <= a)
      break;
    uint64_t x = pow_mod(a, d, n);
    if (x == 1 || x == n - 1)
      continue;
    bool composite = true;
    for (int r = 1; r < s; r++) {
      x = mul_mod(x, x, n);
      if (x == n - 1) {
        composite = false;
        break;
      }
    }
    if (composite)
      return false;
  }
  return true;
}

// --- Pollard's rho 法による高速素因数分解 (O(N^{1/4})) ---
namespace internal_rho {
inline uint64_t gcd_u64(uint64_t a, uint64_t b) {
  while (b) {
    a %= b;
    swap(a, b);
  }
  return a;
}

inline uint64_t find_factor(uint64_t n) {
  if (n % 2 == 0)
    return 2;
  if (is_prime(n))
    return n;

  static mt19937_64 rng(
      chrono::steady_clock::now().time_since_epoch().count());
  uniform_int_distribution<uint64_t> dist_c(1, n - 1);
  uniform_int_distribution<uint64_t> dist_x(2, n - 1);

  while (true) {
    uint64_t c = dist_c(rng);
    uint64_t x = dist_x(rng);
    uint64_t y = x;
    uint64_t d = 1;

    auto f = [&](uint64_t v) { return (mul_mod(v, v, n) + c) % n; };

    while (d == 1) {
      x = f(x);
      y = f(f(y));
      d = gcd_u64((x > y ? x - y : y - x), n);
    }
    if (d != n)
      return d;
  }
}

inline void factorize_rec(uint64_t n, vector<uint64_t> &res) {
  if (n <= 1)
    return;
  if (is_prime(n)) {
    res.push_back(n);
    return;
  }
  uint64_t factor = find_factor(n);
  factorize_rec(factor, res);
  factorize_rec(n / factor, res);
}
} // namespace internal_rho

// 巨大整数 n (n <= 10^18) の素因数を昇順ソートして返す (O(n^{1/4}))
inline vector<uint64_t> factorize(uint64_t n) {
  vector<uint64_t> res;
  internal_rho::factorize_rec(n, res);
  sort(res.begin(), res.end());
  return res;
}

// 素因数分解を {素数, 指数} のペアで返す
inline vector<pair<uint64_t, int>> factorize_pairs(uint64_t n) {
  auto factors = factorize(n);
  vector<pair<uint64_t, int>> res;
  for (uint64_t p : factors) {
    if (res.empty() || res.back().first != p) {
      res.emplace_back(p, 1);
    } else {
      res.back().second++;
    }
  }
  return res;
}

// 整数 n (n <= 10^18) の全約数を昇順ソートして返す
inline vector<uint64_t> divisors(uint64_t n) {
  if (n == 0)
    return {};
  auto pairs = factorize_pairs(n);
  vector<uint64_t> res = {1};
  for (const auto &[p, exp] : pairs) {
    int sz = res.size();
    uint64_t mult = 1;
    for (int i = 0; i < exp; i++) {
      mult *= p;
      for (int j = 0; j < sz; j++) {
        res.push_back(res[j] * mult);
      }
    }
  }
  sort(res.begin(), res.end());
  return res;
}

// --- LinearSieve (osa_k法 / エラトステネスの線形篩) ---
// N <= 10^7 程度の前計算を行い、クエリ O(log x) で素因数分解するクラス
struct LinearSieve {
  int n;
  vector<int> min_factor;
  vector<int> primes;

  LinearSieve() : n(0) {}
  explicit LinearSieve(int n) : n(n), min_factor(n + 1, 0) {
    for (int i = 2; i <= n; i++) {
      if (min_factor[i] == 0) {
        min_factor[i] = i;
        primes.push_back(i);
      }
      for (int p : primes) {
        if (p > min_factor[i] || (long long)i * p > n)
          break;
        min_factor[i * p] = p;
      }
    }
  }

  bool is_prime(int x) const {
    assert(1 <= x && x <= n);
    return min_factor[x] == x;
  }

  // x の素因数列を昇順で返す (O(log x))
  vector<int> factorize(int x) const {
    assert(1 <= x && x <= n);
    vector<int> res;
    while (x > 1) {
      res.push_back(min_factor[x]);
      x /= min_factor[x];
    }
    return res;
  }

  // {素数, 指数} のペアで返す (O(log x))
  vector<pair<int, int>> factorize_pairs(int x) const {
    auto f = factorize(x);
    vector<pair<int, int>> res;
    for (int p : f) {
      if (res.empty() || res.back().first != p) {
        res.emplace_back(p, 1);
      } else {
        res.back().second++;
      }
    }
    return res;
  }

  // 約数列挙 (昇順ソート済み)
  vector<int> divisors(int x) const {
    assert(1 <= x && x <= n);
    auto pairs = factorize_pairs(x);
    vector<int> res = {1};
    for (const auto &[p, exp] : pairs) {
      int sz = res.size();
      int mult = 1;
      for (int i = 0; i < exp; i++) {
        mult *= p;
        for (int j = 0; j < sz; j++) {
          res.push_back(res[j] * mult);
        }
      }
    }
    sort(res.begin(), res.end());
    return res;
  }
};

// --- 中国剰余定理 (Extended Chinese Remainder Theorem) ---
// 連立合同式 x ≡ r_i (mod m_i) を解く
// 戻り値: {x, lcm(m_i)} (0 <= x < lcm, 解なしの場合は {0, 0})
inline pair<long long, long long> crt(const vector<long long> &r,
                                     const vector<long long> &m) {
  assert(r.size() == m.size());
  int n = r.size();
  long long r0 = 0, m0 = 1;
  for (int i = 0; i < n; i++) {
    assert(m[i] >= 1);
    long long r1 = (r[i] % m[i] + m[i]) % m[i];
    long long m1 = m[i];
    if (m0 < m1) {
      swap(r0, r1);
      swap(m0, m1);
    }
    if (m0 % m1 == 0) {
      if (r0 % m1 != r1)
        return {0, 0};
      continue;
    }
    long long im, ym;
    long long g = extgcd(m0, m1, im, ym);
    long long u1 = m1 / g;
    if ((r1 - r0) % g != 0)
      return {0, 0};
    long long x = (r1 - r0) / g % u1 * im % u1;
    r0 += x * m0;
    m0 *= u1;
    if (r0 < 0)
      r0 += m0;
  }
  return {r0, m0};
}

} // namespace NumberTheory
```

</details>

---

#### BiCoef (二項係数・順列・重複組合せ)
階乗およびその逆元テーブルを $O(N)$ で前計算し、順列 $nPr$、組合せ $nCr$、重複組合せ $nHr$ を $O(1)$ で算出します。ACL の `modint` と完全互換です。
- **ファイル**: `mylib/BiCoef.hpp`
- **計算量**: 初期化 $O(N)$、各計算クエリ $O(1)$

```cpp
#include <atcoder/modint>
#include "mylib/BiCoef.hpp"

using mint = atcoder::modint998244353;

BiCoef<mint> bc(2000000); // 2*10^6 までのテーブル構築
mint c = bc.C(100, 5);    // 100 C 5
mint p = bc.P(10, 3);     // 10 P 3
mint h = bc.H(5, 3);      // 5 H 3
mint f = bc.fact(10);     // 10!
```

<details>
<summary><b>ソースコードを展開 (BiCoef.hpp)</b></summary>

```cpp
#pragma once
#include <bits/stdc++.h>

using namespace std;

// 順列・組合せを計算するライブラリ
template <class mint> struct BiCoef {
  vector<mint> fact_, inv_fact_;

  // デフォルトコンストラクタ
  BiCoef() {}

  // コンストラクタで n までの階乗とその逆元を事前計算 (計算量: O(n))
  BiCoef(int n) { init(n); }

  // 初期化・再構築
  void init(int n) {
    if (n < 0) return;
    fact_.assign(n + 1, 1);
    inv_fact_.assign(n + 1, 1);
    for (int i = 1; i <= n; i++) {
      fact_[i] = fact_[i - 1] * i;
    }
    // ACLのmodintは .inv() メソッドを持つのでそのまま呼べる
    inv_fact_[n] = fact_[n].inv();
    // 逆元の計算は後ろからやると高速 (O(1)回の逆元計算で済む)
    for (int i = n; i > 0; i--) {
      inv_fact_[i - 1] = inv_fact_[i] * i;
    }
  }

  // 階乗 n!
  mint fact(int n) const {
    if (n < 0 || n >= (int)fact_.size())
      return 0;
    return fact_[n];
  }

  // 階乗の逆元 1 / n!
  mint inv_fact(int n) const {
    if (n < 0 || n >= (int)inv_fact_.size())
      return 0;
    return inv_fact_[n];
  }

  // 順列 nPr
  mint P(int n, int r) const {
    if (n < 0 || n < r || r < 0 || n >= (int)fact_.size())
      return 0;
    return fact_[n] * inv_fact_[n - r];
  }

  // 組合せ nCr
  mint C(int n, int r) const {
    if (n < 0 || n < r || r < 0 || n >= (int)fact_.size())
      return 0;
    return fact_[n] * inv_fact_[r] * inv_fact_[n - r];
  }

  // 重複組合せ nHr (n種類の中から重複を許してr個選ぶ)
  mint H(int n, int r) const {
    if (n < 0 || r < 0)
      return 0;
    if (n == 0 && r == 0)
      return 1;
    return C(n + r - 1, r);
  }
};
```

</details>

---

#### LinearDiophantine (一次不定方程式)
一次不定方程式 $ax + by = c$ の整数解の存在判定、特解の算出、一般解ステップの計算、および変数の有界区間内における解の個数カウントや目的関数の最小化を行います。
- **ファイル**: `mylib/LinearDiophantine.hpp`
- **計算量**: $O(\log(\min(a, b)))$

```cpp
#include "mylib/LinearDiophantine.hpp"

LinearDiophantine ld(a, b, c); // a*x + b*y = c
if (ld.has_sol) {
    long long x0 = ld.x0; // 特解
    long long y0 = ld.y0;
    // x = x0 + k * ld.dx,  y = y0 - k * ld.dy
    long long cnt = ld.count_solutions(x_min, x_max, y_min, y_max);
    auto [opt_x, opt_y] = ld.minimize_x_non_negative(); // x >= 0 かつ最小の解
}
```

<details>
<summary><b>ソースコードを展開 (LinearDiophantine.hpp)</b></summary>

```cpp
#pragma once
#include <bits/stdc++.h>

using namespace std;

// 一次不定方程式 a*x + b*y = c の整数解を扱う構造体
struct LinearDiophantine {
  long long a, b, c;
  long long g;
  long long x0, y0; // 特解
  long long dx, dy; // 一般解のステップ： x = x0 + k*dx, y = y0 - k*dy
  bool has_sol;

  static constexpr long long INF = 2e18; // 安全な無限大定数 (double精度落ちを防止)

  LinearDiophantine(long long a, long long b, long long c) : a(a), b(b), c(c) {
    solve();
  }

  // 負の数も正しく扱う切り捨て除算 (floor(n / d))
  long long floor_div(long long n, long long d) const {
    assert(d != 0);
    long long res = n / d;
    long long rem = n % d;
    if (rem != 0 && ((n < 0) ^ (d < 0))) {
      res--;
    }
    return res;
  }

  // 負の数も正しく扱う切り上げ除算 (ceil(n / d))
  long long ceil_div(long long n, long long d) const {
    assert(d != 0);
    long long res = n / d;
    long long rem = n % d;
    if (rem != 0 && !((n < 0) ^ (d < 0))) {
      res++;
    }
    return res;
  }

  // 解を k ステップ平行移動させる
  void shift(long long k) {
    x0 += k * dx;
    y0 -= k * dy;
  }

  // x >= 0 を満たす最小の x に現在の解 (x0, y0) を調整する
  // 解が存在しない、または dx == 0 で不可能な場合は false / x0 >= 0 を返す
  bool minimize_x_non_negative() {
    if (!has_sol)
      return false;
    if (dx == 0)
      return x0 >= 0;
    long long step = abs(dx);
    long long rem = (x0 % step + step) % step;
    long long k = (rem - x0) / dx;
    shift(k);
    return true;
  }

  // 指定範囲 x in [minx, maxx], y in [miny, maxy] に存在する解の個数を O(1) で数える
  long long count_solutions(long long minx, long long maxx, long long miny,
                            long long maxy) const {
    if (!has_sol || minx > maxx || miny > maxy)
      return 0;

    if (dx == 0 && dy == 0) {
      // a == 0 && b == 0 の場合
      if (a == 0 && b == 0 && c == 0) {
        return (maxx - minx + 1) * (maxy - miny + 1);
      }
      if (x0 >= minx && x0 <= maxx && y0 >= miny && y0 <= maxy)
        return 1;
      return 0;
    }

    long long k_min = -INF;
    long long k_max = INF;

    // x = x0 + k * dx からの制約
    if (dx > 0) {
      k_min = max(k_min, ceil_div(minx - x0, dx));
      k_max = min(k_max, floor_div(maxx - x0, dx));
    } else if (dx < 0) {
      k_min = max(k_min, ceil_div(maxx - x0, dx));
      k_max = min(k_max, floor_div(minx - x0, dx));
    } else { // dx == 0 (b == 0)
      if (x0 < minx || x0 > maxx)
        return 0;
    }

    // y = y0 - k * dy からの制約
    if (dy > 0) {
      k_min = max(k_min, ceil_div(y0 - maxy, dy));
      k_max = min(k_max, floor_div(y0 - miny, dy));
    } else if (dy < 0) {
      k_min = max(k_min, ceil_div(y0 - miny, dy));
      k_max = min(k_max, floor_div(y0 - maxy, dy));
    } else { // dy == 0 (a == 0)
      if (y0 < miny || y0 > maxy)
        return 0;
    }

    if (k_min > k_max)
      return 0;
    return k_max - k_min + 1;
  }

private:
  long long extGCD(long long a, long long b, long long &x, long long &y) {
    if (b == 0) {
      x = 1;
      y = 0;
      return a;
    }
    long long x1, y1;
    long long d = extGCD(b, a % b, x1, y1);
    x = y1;
    y = x1 - y1 * (a / b);
    return d;
  }

  void solve() {
    if (a == 0 && b == 0) {
      if (c == 0) {
        has_sol = true;
        x0 = 0;
        y0 = 0;
        g = 0;
        dx = 0;
        dy = 0;
      } else {
        has_sol = false;
        x0 = 0;
        y0 = 0;
        g = 0;
        dx = 0;
        dy = 0;
      }
      return;
    }
    long long x, y;
    g = extGCD(abs(a), abs(b), x, y);
    if (c % g != 0) {
      has_sol = false;
      x0 = 0;
      y0 = 0;
      dx = 0;
      dy = 0;
      return;
    }
    has_sol = true;
    if (a < 0)
      x = -x;
    if (b < 0)
      y = -y;
    x0 = x * (c / g);
    y0 = y * (c / g);
    dx = b / g;
    dy = a / g;
  }
};
```

</details>

---

#### Matrix (半環対応汎用行列累乗)
通常の体上の行列演算（四則演算）だけでなく、トロピカル半環（$(\min, +)$ 最短経路DPや $(\max, +)$ 最長経路DP）、ブール半環（推移閉包・到達可能性）、XOR-AND 半環に対応した汎用行列クラスです。
- **ファイル**: `mylib/Matrix.hpp`
- **計算量**: 行列積 $O(K^3)$、行列累乗 $O(K^3 \log N)$

```cpp
#include "mylib/Matrix.hpp"

// 1. 通常の行列累乗 (例: フィボナッチ数列、DPの遷移高速化)
Matrix<mint> mat(2, 2);
mat[0][0] = 1; mat[0][1] = 1;
mat[1][0] = 1; mat[1][1] = 0;
Matrix<mint> mat_n = mat.pow(N);

// 2. トロピカル半環 (min, +) : 頂点間最短路・辺固定数ステップDP
const long long INF = 2e18;
using Tropical = MinPlusSemiring<long long, INF>;
Matrix<long long, Tropical> dist_mat(n, n, INF);
dist_mat[u][v] = cost;
auto res_k_steps = dist_mat.pow(K);
```

<details>
<summary><b>ソースコードを展開 (Matrix.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

// --- 半環構造の定義 ---

// 通常の四則演算（modint や通常の整数・小数向け）
template <typename T> struct StandardSemiring {
  static T zero() { return T(0); }
  static T one() { return T(1); }
  static T add(const T &a, const T &b) { return a + b; }
  static T mul(const T &a, const T &b) { return a * b; }
  static bool is_zero(const T &a) { return a == zero(); }
};

// トロピカル半環 (min, +) : 最短経路・最小コストDP用
template <typename T, T INF> struct MinPlusSemiring {
  static T zero() { return INF; } // 加法の単位元
  static T one() { return T(0); } // 乗法の単位元
  static T add(const T &a, const T &b) { return min(a, b); }
  static T mul(const T &a, const T &b) {
    if (a == INF || b == INF)
      return INF;
    return a + b;
  }
  static bool is_zero(const T &a) { return a == INF; }
};

// トロピカル半環 (max, +) : 最長経路・最大スコアDP用
template <typename T, T NEG_INF> struct MaxPlusSemiring {
  static T zero() { return NEG_INF; }
  static T one() { return T(0); }
  static T add(const T &a, const T &b) { return max(a, b); }
  static T mul(const T &a, const T &b) {
    if (a == NEG_INF || b == NEG_INF)
      return NEG_INF;
    return a + b;
  }
  static bool is_zero(const T &a) { return a == NEG_INF; }
};

// ブール半環 (OR, AND) : グラフの到達可能性・推移閉包用
struct BooleanSemiring {
  static bool zero() { return false; }
  static bool one() { return true; }
  static bool add(const bool &a, const bool &b) { return a || b; }
  static bool mul(const bool &a, const bool &b) { return a && b; }
  static bool is_zero(const bool &a) { return !a; }
};

// 1. スカラー向け (各要素が 0 または 1 の GF(2) 上の線形代数)
template <typename T = int> struct XorAndSemiring {
  static T zero() { return T(0); }
  static T one() { return T(1); }
  static T add(const T &a, const T &b) { return a ^ b; }
  static T mul(const T &a, const T &b) { return a & b; }
  static bool is_zero(const T &a) { return a == T(0); }
};

// 2. ビット並列向け (unsigned long long 等で全ビット独立に並列計算)
template <typename T = unsigned long long> struct BitwiseXorAndSemiring {
  static T zero() { return T(0); }
  static T one() { return ~T(0); } // 全ビットが 1 (AND の単位元)
  static T add(const T &a, const T &b) { return a ^ b; }
  static T mul(const T &a, const T &b) { return a & b; }
  static bool is_zero(const T &a) { return a == T(0); }
};

// --- 行列クラス ---

template <typename T, typename Semiring = StandardSemiring<T>> struct Matrix {
  int h, w;
  vector<vector<T>> mat;

  Matrix() : h(0), w(0) {}
  Matrix(int h, int w, T val = Semiring::zero())
      : h(h), w(w), mat(h, vector<T>(w, val)) {}
  Matrix(const vector<vector<T>> &d)
      : h(d.size()), w(d.empty() ? 0 : d[0].size()), mat(d) {}

  static Matrix identity(int n) {
    Matrix res(n, n, Semiring::zero());
    for (int i = 0; i < n; ++i)
      res[i][i] = Semiring::one();
    return res;
  }

  const vector<T> &operator[](int i) const { return mat[i]; }
  vector<T> &operator[](int i) { return mat[i]; }

  // 行列積 (i -> k -> j 最適化 & ゼロ元スキップ)
  Matrix operator*(const Matrix &r) const {
    assert(w == r.h);
    Matrix res(h, r.w, Semiring::zero());
    for (int i = 0; i < h; ++i) {
      for (int k = 0; k < w; ++k) {
        if (Semiring::is_zero(mat[i][k]))
          continue;
        for (int j = 0; j < r.w; ++j) {
          res.mat[i][j] = Semiring::add(res.mat[i][j],
                                        Semiring::mul(mat[i][k], r.mat[k][j]));
        }
      }
    }
    return res;
  }

  Matrix &operator*=(const Matrix &r) { return *this = *this * r; }

  // 行列累乗 (繰り返し二乗法)
  Matrix pow(long long k) const {
    assert(h == w && k >= 0);
    Matrix res = Matrix::identity(h);
    Matrix a = *this;
    while (k > 0) {
      if (k & 1)
        res *= a;
      a *= a;
      k >>= 1;
    }
    return res;
  }

  // 行列 × ベクトル
  vector<T> operator*(const vector<T> &v) const {
    assert(w == (int)v.size());
    vector<T> res(h, Semiring::zero());
    for (int i = 0; i < h; ++i) {
      for (int j = 0; j < w; ++j) {
        if (Semiring::is_zero(mat[i][j]) || Semiring::is_zero(v[j]))
          continue;
        res[i] = Semiring::add(res[i], Semiring::mul(mat[i][j], v[j]));
      }
    }
    return res;
  }
};
```

</details>

---

### 5. 幾何 (Geometry)

#### Geometry (2次元計算幾何)
浮動小数点誤差を考慮した 2次元計算幾何ライブラリです。
- **ファイル**: `mylib/Geometry.hpp`
- **提供される主な機能**:
  - 点・ベクトル: `Point`（四則演算、内積 `dot`、外積 `cross`、絶対値 `abs`、単位ベクトル `unit_vector`）
  - 直線・線分: `Line`, `Segment`（射影 `project`、反射 `reflect`、直交・平行判定）
  - 交差・距離: 直線・線分の交差判定 (`intersect`)、交点計算 (`crosspoint`)、点と線分の距離
  - 多角形: 面積計算 (`area`)、凸性判定 (`is_convex`)、凸包構築 (`convex_hull`, Andrew's Monotone Chain法 $O(N \log N)$)、点の多角形包含判定 (`contains`)
  - 円: `Circle`（円と直線の交点、2円の交差状態判定・交点計算、点から円への接線）

```cpp
#include "mylib/Geometry.hpp"

Point a(0, 0), b(4, 0), c(0, 3);
Segment seg1(a, b), seg2(Point(2, -2), Point(2, 2));

// 線分交差判定
if (intersect(seg1, seg2)) {
    Point p = crosspoint(seg1, seg2); // 交点 (2, 0)
}

// 凸包 (Convex Hull)
vector<Point> pts = {a, b, c, Point(1, 1)};
Polygon ch = convex_hull(pts);
double s = area(ch); // 面積
```

<details>
<summary><b>ソースコードを展開 (Geometry.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

// ====================================================================
// 2次元計算幾何ライブラリ (2D Computational Geometry Library)
// ====================================================================

// --- 1. 基本設定と型定義 ---
using Real = double;
const Real EPS = 1e-9;
const Real PI = acos(-1.0);

// 浮動小数点数の微小な誤差(EPS)を考慮した符号判定
inline int sgn(Real a) {
  if (a < -EPS)
    return -1;
  if (a > EPS)
    return 1;
  return 0;
}

// 2つの実数がほぼ等しいか判定
inline bool equals(Real a, Real b) { return sgn(a - b) == 0; }

// 値を [low, high] の範囲に安全に収める (acos 等の NaN 防止)
inline Real clamp_val(Real x, Real low = -1.0, Real high = 1.0) {
  return max(low, min(high, x));
}

// --- 2. Point (点 / ベクトル) 構造体 ---
struct Point {
  Real x, y;

  Point(Real x = 0.0, Real y = 0.0) : x(x), y(y) {}

  // 単項演算子
  Point operator-() const { return Point(-x, -y); }

  // 複合代入演算子
  Point &operator+=(const Point &p) {
    x += p.x;
    y += p.y;
    return *this;
  }
  Point &operator-=(const Point &p) {
    x -= p.x;
    y -= p.y;
    return *this;
  }
  Point &operator*=(Real k) {
    x *= k;
    y *= k;
    return *this;
  }
  Point &operator/=(Real k) {
    assert(sgn(k) != 0);
    x /= k;
    y /= k;
    return *this;
  }

  // 二項演算子
  Point operator+(const Point &p) const { return Point(*this) += p; }
  Point operator-(const Point &p) const { return Point(*this) -= p; }
  Point operator*(Real k) const { return Point(*this) *= k; }
  Point operator/(Real k) const { return Point(*this) /= k; }

  // 比較演算子 (誤差を考慮)
  bool operator==(const Point &p) const {
    return sgn(x - p.x) == 0 && sgn(y - p.y) == 0;
  }
  bool operator!=(const Point &p) const { return !(*this == p); }

  // 辞書式順序 (X優先、同じならY)
  bool operator<(const Point &p) const {
    if (sgn(x - p.x) != 0)
      return sgn(x - p.x) < 0;
    return sgn(y - p.y) < 0;
  }
  bool operator>(const Point &p) const { return p < *this; }

  // 原点中心に反時計回りに theta ラジアン回転
  Point rotate(Real theta) const {
    return Point(x * cos(theta) - y * sin(theta),
                 x * sin(theta) + y * cos(theta));
  }

  // center を中心に反時計回りに theta ラジアン回転
  Point rotate(Real theta, const Point &center) const {
    return center + (*this - center).rotate(theta);
  }

  // 単位法線ベクトル (反時計回りに90度回転した単位ベクトル)
  Point normal_unit() const;
};

inline Point operator*(Real k, const Point &p) { return p * k; }

istream &operator>>(istream &is, Point &p) { return is >> p.x >> p.y; }
ostream &operator<<(ostream &os, const Point &p) {
  return os << "(" << p.x << ", " << p.y << ")";
}

// --- 3. 基本演算関数 ---

// 内積 (dot product)
inline Real dot(const Point &a, const Point &b) {
  return a.x * b.x + a.y * b.y;
}

// 外積 (cross product / 2D外積)
inline Real cross(const Point &a, const Point &b) {
  return a.x * b.y - a.y * b.x;
}

// ベクトルの長さの2乗 (norm)
inline Real norm(const Point &p) { return p.x * p.x + p.y * p.y; }

// ベクトルの長さ (abs)
inline Real abs(const Point &p) { return sqrt(max((Real)0.0, norm(p))); }

// 2点間のユークリッド距離
inline Real distance(const Point &a, const Point &b) { return abs(a - b); }

inline Point Point::normal_unit() const {
  Real len = abs(*this);
  assert(sgn(len) != 0);
  return Point(-y / len, x / len);
}

// --- 4. 直線 (Line) と 線分 (Segment) ---
struct Line {
  Point a, b;
  Line() : a(Point()), b(Point()) {}
  Line(Point a, Point b) : a(a), b(b) {}
};

struct Segment : Line {
  Segment() : Line() {}
  Segment(Point a, Point b) : Line(a, b) {}
};

// --- 5. 進行方向判定 (CCW) ---
const int CCW_COUNTER_CLOCKWISE = 1; // 反時計回り (左側)
const int CCW_CLOCKWISE = -1;        // 時計回り (右側)
const int CCW_ONLINE_BACK = 2;       // C - A - B の順で一直線上
const int CCW_ONLINE_FRONT = -2;     // A - B - C の順で一直線上
const int CCW_ON_SEGMENT = 0;        // A - C - B の順で線分上

// 線分 a-b を基準とした点 c の位置関係
inline int ccw(const Point &a, const Point &b, const Point &c) {
  Point ab = b - a;
  Point ac = c - a;

  int cross_sgn = sgn(cross(ab, ac));
  if (cross_sgn > 0)
    return CCW_COUNTER_CLOCKWISE;
  if (cross_sgn < 0)
    return CCW_CLOCKWISE;

  if (sgn(dot(ab, ac)) < 0)
    return CCW_ONLINE_BACK;
  if (sgn(norm(ab) - norm(ac)) < 0)
    return CCW_ONLINE_FRONT;

  return CCW_ON_SEGMENT;
}

// --- 6. 射影・反射 ---

// 直線 a-b に対して点 c から下ろした垂線の足 (正射影)
inline Point project(const Point &a, const Point &b, const Point &c) {
  Point ab = b - a;
  Point ac = c - a;
  Real t = dot(ac, ab) / norm(ab);
  return a + ab * t;
}
inline Point project(const Line &l, const Point &c) {
  return project(l.a, l.b, c);
}

// 直線 a-b を対称軸として点 c と線対称な位置にある点 (反射)
inline Point reflect(const Point &a, const Point &b, const Point &c) {
  return c + (project(a, b, c) - c) * 2.0;
}
inline Point reflect(const Line &l, const Point &c) {
  return reflect(l.a, l.b, c);
}

// --- 7. 交差判定 ---

// 線分 a-b と線分 c-d が交差するか判定 (端点共有も真)
inline bool intersect(const Point &a, const Point &b, const Point &c,
                      const Point &d) {
  return ccw(a, b, c) * ccw(a, b, d) <= 0 &&
         ccw(c, d, a) * ccw(c, d, b) <= 0;
}
inline bool intersect(const Segment &s1, const Segment &s2) {
  return intersect(s1.a, s1.b, s2.a, s2.b);
}

// 直線 a-b と点 c が平行/直線上か判定
inline bool intersect_line_point(const Point &a, const Point &b,
                                 const Point &c) {
  return sgn(cross(b - a, c - a)) == 0;
}

// 直線 a-b と直線 c-d が交差するか (平行でないか)
inline bool intersect_line_line(const Point &a, const Point &b, const Point &c,
                                const Point &d) {
  return sgn(cross(b - a, d - c)) != 0;
}

// --- 8. 距離 ---

// 直線 a-b と点 c の最短距離
inline Real distance_line_point(const Point &a, const Point &b,
                                const Point &c) {
  return abs(cross(b - a, c - a)) / distance(a, b);
}

// 線分 a-b と点 c の最短距離
inline Real distance_segment_point(const Point &a, const Point &b,
                                   const Point &c) {
  if (a == b)
    return distance(a, c);
  if (sgn(dot(b - a, c - a)) < 0)
    return distance(a, c);
  if (sgn(dot(a - b, c - b)) < 0)
    return distance(b, c);
  return distance_line_point(a, b, c);
}
inline Real distance_segment_point(const Segment &s, const Point &c) {
  return distance_segment_point(s.a, s.b, c);
}

// 線分 a-b と線分 c-d の最短距離 (交差時は 0.0)
inline Real distance_segment_segment(const Point &a, const Point &b,
                                     const Point &c, const Point &d) {
  if (intersect(a, b, c, d))
    return 0.0;
  return min({distance_segment_point(a, b, c),
              distance_segment_point(a, b, d),
              distance_segment_point(c, d, a),
              distance_segment_point(c, d, b)});
}
inline Real distance_segment_segment(const Segment &s1, const Segment &s2) {
  return distance_segment_segment(s1.a, s1.b, s2.a, s2.b);
}

// --- 9. 交点座標 ---

// 直線 a-b と直線 c-d の交点 (平行でないことを前提とする)
inline Point cross_point_line(const Point &a, const Point &b, const Point &c,
                              const Point &d) {
  Point ab = b - a;
  Point cd = d - c;
  Real d1 = cross(ab, cd);
  assert(sgn(d1) != 0); // 平行なら交点なし
  Real d2 = cross(ab, b - c);
  return c + cd * (d2 / d1);
}
inline Point cross_point_line(const Line &l1, const Line &l2) {
  return cross_point_line(l1.a, l1.b, l2.a, l2.b);
}

// 線分 a-b と線分 c-d の交点座標 (交差していることを前提とする)
inline Point cross_point_segment(const Point &a, const Point &b, const Point &c,
                                 const Point &d) {
  assert(intersect(a, b, c, d));
  return cross_point_line(a, b, c, d);
}
inline Point cross_point_segment(const Segment &s1, const Segment &s2) {
  return cross_point_segment(s1.a, s1.b, s2.a, s2.b);
}

// --- 10. 円 (Circle) ---
struct Circle {
  Point c;
  Real r;
  Circle(Point c = Point(0.0, 0.0), Real r = 0.0) : c(c), r(r) {}
};

// 2つの円の交点座標 (交点なし: 0個, 接する: 1個, 交わる: 2個)
inline vector<Point> cross_point_circles(const Circle &c1, const Circle &c2) {
  Real d = distance(c1.c, c2.c);
  // 同心円の場合
  if (sgn(d) == 0) {
    return {};
  }
  // 離れすぎ、または一方が他方に完全内包されている
  if (sgn(d - (c1.r + c2.r)) > 0 || sgn(d - abs(c1.r - c2.r)) < 0) {
    return {};
  }

  // 余弦定理で角度 a を求める (浮動小数点誤差による NaN を clamp で防止)
  Real cos_a = (c1.r * c1.r + d * d - c2.r * c2.r) / (2.0 * c1.r * d);
  Real a = acos(clamp_val(cos_a, -1.0, 1.0));
  Real t = atan2(c2.c.y - c1.c.y, c2.c.x - c1.c.x);

  Point p1 = c1.c + Point(cos(t + a) * c1.r, sin(t + a) * c1.r);
  Point p2 = c1.c + Point(cos(t - a) * c1.r, sin(t - a) * c1.r);

  if (p1 == p2)
    return {p1};
  return {p1, p2};
}

// 円と直線 a-b の交点座標 (0, 1, または 2 個)
inline vector<Point> cross_point_circle_line(const Circle &c, const Point &a,
                                             const Point &b) {
  Real d = distance_line_point(a, b, c.c);
  if (sgn(d - c.r) > 0)
    return {};

  Point pr = project(a, b, c.c);
  if (sgn(d - c.r) == 0)
    return {pr};

  Point v = (b - a) / distance(a, b);
  Real len = sqrt(max((Real)0.0, c.r * c.r - d * d));
  return {pr - v * len, pr + v * len};
}

// --- 11. 多角形 (Polygon) ---
using Polygon = vector<Point>;

// 多角形の面積 (頂点の並び順によらず正の値を返す)
inline Real area(const Polygon &P) {
  int sz = P.size();
  if (sz < 3)
    return 0.0;
  Real res = 0.0;
  for (int i = 0; i < sz; i++) {
    res += cross(P[i], P[(i + 1) % sz]);
  }
  return abs(res) * 0.5;
}

// 点 p と多角形 P の包含判定 (0: 外部, 1: 辺上, 2: 内部)
inline int contains(const Polygon &P, const Point &p) {
  int sz = P.size();
  bool in = false;
  for (int i = 0; i < sz; i++) {
    Point a = P[i] - p;
    Point b = P[(i + 1) % sz] - p;
    if (a.y > b.y)
      swap(a, b);

    // 点pからX軸正方向への半直線と辺が交差するか
    if (sgn(a.y) <= 0 && sgn(b.y) > 0 && sgn(cross(a, b)) < 0)
      in = !in;
    // 点pが辺上にあるか
    if (sgn(cross(a, b)) == 0 && sgn(dot(a, b)) <= 0)
      return 1;
  }
  return in ? 2 : 0;
}

// 凸多角形判定 (反時計回りに与えられた多角形が凸かどうか)
inline bool is_convex(const Polygon &P) {
  int sz = P.size();
  if (sz < 3)
    return true;
  for (int i = 0; i < sz; i++) {
    if (ccw(P[i], P[(i + 1) % sz], P[(i + 2) % sz]) == CCW_CLOCKWISE) {
      return false;
    }
  }
  return true;
}

// 凸包 (Convex Hull: Andrew's Monotone Chain) O(N log N)
// strict = true: 一直線上の余分な点を除去 (厳密な頂点のみ)
// strict = false: 辺上の点も含める
inline Polygon convex_hull(vector<Point> P, bool strict = true) {
  int n = P.size();
  if (n <= 2)
    return P;
  sort(P.begin(), P.end());

  Polygon ch(2 * n);
  int k = 0;

  Real threshold = strict ? 0.0 : -EPS;

  // 下側凸包
  for (int i = 0; i < n; i++) {
    while (k >= 2 && sgn(cross(ch[k - 1] - ch[k - 2], P[i] - ch[k - 1])) <=
                         threshold) {
      k--;
    }
    ch[k++] = P[i];
  }
  // 上側凸包
  for (int i = n - 2, t = k + 1; i >= 0; i--) {
    while (k >= t && sgn(cross(ch[k - 1] - ch[k - 2], P[i] - ch[k - 1])) <=
                         threshold) {
      k--;
    }
    ch[k++] = P[i];
  }

  ch.resize(k - 1);
  return ch;
}

// 凸多角形の直径 (最遠点対間距離) - キャリパー法 O(N)
inline Real convex_diameter(const Polygon &P) {
  int n = P.size();
  if (n <= 1)
    return 0.0;
  if (n == 2)
    return distance(P[0], P[1]);

  int is = 0, js = 0;
  for (int i = 1; i < n; i++) {
    if (P[i].y > P[is].y)
      is = i;
    if (P[i].y < P[js].y)
      js = i;
  }

  Real max_d = distance(P[is], P[js]);
  int i = is, j = js;
  do {
    int ni = (i + 1) % n;
    int nj = (j + 1) % n;
    if (sgn(cross(P[ni] - P[i], P[nj] - P[j])) >= 0) {
      j = nj;
    } else {
      i = ni;
    }
    max_d = max(max_d, distance(P[i], P[j]));
  } while (i != is || j != js);

  return max_d;
}
```

</details>

---

## 一括インクルード (All.hpp)

全ライブラリを一括でインクルードするためのヘッダーファイルです。

<details>
<summary><b>ソースコードを展開 (All.hpp)</b></summary>

```cpp
#pragma once

#include "BiCoef.hpp"
#include "CoordinateCompression.hpp"
#include "Dijkstra.hpp"
#include "DisjointSparseTable.hpp"
#include "DoubleRollingHash.hpp"
#include "DynamicSegTree.hpp"
#include "DynamicStringTreap.hpp"
#include "Geometry.hpp"
#include "LazySqrtDecomposition.hpp"
#include "LinearDiophantine.hpp"
#include "Matrix.hpp"
#include "NumberTheory.hpp"
#include "OrderStatisticSet.hpp"
#include "PersistentSegTree.hpp"
#include "Rerooting.hpp"
#include "RollingHash.hpp"
#include "SegTree2D.hpp"
#include "SparseTable.hpp"
#include "SqrtDecomposition.hpp"
#include "TopologicalSort.hpp"
#include "Trie.hpp"
#include "UnionFind.hpp"
#include "WeightedUnionFind.hpp"
```

</details>

---

## 使い方と環境設定

### 1. 全ライブラリの一括インクルード
コンテスト環境で手軽にすべてのライブラリを読み込むには、プロジェクトルートの `mylib.hpp` または `mylib/All.hpp` をインクルードしてください。

```cpp
#include "mylib/All.hpp"
// または
#include "mylib.hpp"
```

### 2. 個別インクルード
ライブラリの依存関係は極小に設計されているため、必要なファイルのみを単体でインクルードして提出コードを軽量に保つことができます。

```cpp
#include "mylib/DisjointSparseTable.hpp"
#include "mylib/NumberTheory.hpp"
```

### 3. コンパイル要件
- **C++ 標準**: C++17 以上
- **依存関係**: Standard Template Library (STL)。AtCoder Library (ACL) と併用可能。
