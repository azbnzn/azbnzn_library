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
    - [BIT2D (動的2次元BIT・双対BIT・領域加算和)](#bit2d-動的2次元bit双対bit領域加算和)
    - [DynamicSegTree (動的セグ木)](#dynamicsegtree-動的セグ木)
    - [PersistentSegTree (永続セグ木)](#persistentsegtree-永続セグ木)
    - [SqrtDecomposition (平方分割)](#sqrtdecomposition-平方分割)
    - [LazySqrtDecomposition (遅延平方分割)](#lazysqrtdecomposition-遅延平方分割)
    - [OrderStatisticSet (順序付き集合)](#orderstatisticset-順序付き集合)
    - [CoordinateCompression (座標圧縮)](#coordinatecompression-座標圧縮)
    - [MosAlgorithm (Mo's algorithm)](#mosalgorithm)
    - [TwoPointers (尺取り法)](#twopointers-尺取り法)
    - [ZobristHash (多重集合・区間一致・順列判定・動的ハッシュ)](#zobristhash-多重集合区間一致順列判定動的ハッシュ)
    - [IntervalSet / IntervalMap (区間を set で管理するテク・mex・Chtholly Tree)](#intervalset--intervalmap-区間を-set-で管理するテクmexchtholly-tree)
    - [MultiDimensionalCumulativeSum (3次元以上の多次元累積和・多次元いもす法)](#multidimensionalcumulativesum-3次元以上の多次元累積和多次元いもす法)
  - [2. 文字列 (Strings)](#2-文字列-strings)
    - [DoubleRollingHash (安全なダブルローリングハッシュ・点更新対応)](#doublerollinghash-安全なダブルローリングハッシュ点更新対応)
    - [DynamicStringTreap (動的文字列平衡二分木)](#dynamicstringtreap-動的文字列平衡二分木)
    - [Trie / MapTrie](#trie--maptrie)
  - [3. グラフ・木 (Graphs & Trees)](#3-グラフ木-graphs--trees)
    - [Dijkstra (単一始点最短経路)](#dijkstra-単一始点最短経路)
    - [TopologicalSort (トポロジカルソート)](#topologicalsort-トポロジカルソート)
    - [Rerooting (全方位木DP)](#rerooting-全方位木dp)
    - [EulerTour (オイラーツアー・LCA)](#eulertour)
    - [LowestCommonAncestor (LCA・木上の重み付き最短距離・パスJump)](#lowestcommonancestor)
  - [4. 数学・数論 (Math & Number Theory)](#4-数学数論-math--number-theory)
    - [NumberTheory (高速数論・素因数分解・CRT)](#numbertheory-高速数論素因数分解crt)
    - [Prime (巨大素数判定・素因数分解・線形篩・区間篩)](#prime)
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
| 動的2次元BIT・双対2次元BIT・領域加算和 (RAQ) | `BIT2D` / `DualBIT2D` / `RangeAddBIT2D` | クエリ $O(\log H \log W)$ | `BIT2D.hpp` |
| 座標が $10^9$ や $10^{18}$ と巨大なセグ木 | `DynamicSegTree<S, op, e>` | クエリ $O(\log C)$ | `DynamicSegTree.hpp` |
| 過去の状態へのクエリ・ロールバック・復元 | `PersistentSegTree<S, op, e>` | 点更新 $O(\log N)$<br>クエリ $O(\log N)$ | `PersistentSegTree.hpp` |
| ブロック単位の平方分割（ACL segtree 互換） | `SqrtDecomposition<S, op, e>` | クエリ $O(\sqrt{N})$ | `SqrtDecomposition.hpp` |
| 区間加算・区間変更・区間クエリ（遅延評価） | `LazySqrtDecomposition<...>` | クエリ $O(\sqrt{N})$ | `LazySqrtDecomposition.hpp` |
| $k$ 番目に小さい値 / 値 $x$ 未満の要素数 | `OrderStatisticSet<T>` (`OSS`) | $O(\log N)$ | `OrderStatisticSet.hpp` |
| 座標圧縮・添字変換・復元 | `CoordinateCompression<T>` | 構築 $O(N \log N)$<br>取得 $O(\log N)$ / $O(1)$ | `CoordinateCompression.hpp` |
| オフライン区間クエリ・伸縮更新 (Mo's algorithm) | `MosAlgorithm` (`Mo`) | $O(N \sqrt{Q})$ | `MosAlgorithm.hpp` |
| 尺取り法（単調性のある区間条件の極大区間列挙） | `two_pointers` | $O(N)$ | `TwoPointers.hpp` |
| 区間多重集合一致・区間順列判定・状態ハッシュ | `ZobristHash` / `DynamicZobristHash` | 構築 $O(N)$<br>クエリ $O(1)$ / $O(\log N)$ | `ZobristHash.hpp` |
| 区間の集合管理・マージ・被覆長・mex・区間色塗り | `IntervalSet` / `IntervalMap` | ならし $O(\log N)$ / クエリ $O(\log N)$ | `IntervalSet.hpp` |
| 3次元以上の多次元累積和・多次元いもす法 | `MultiDimensionalPrefixSum` / `MultiDimensionalImos` / `PrefixSum3D` | 構築 $O(D \cdot \prod N_k)$<br>クエリ $O(2^D)$ / $O(1)$ | `MultiDimensionalCumulativeSum.hpp` |
| 文字列一致判定・点更新クエリ・回文判定 | `DoubleRollingHash` | ハッシュ取得 $O(1)$ / 更新 $O(\log N)$<br>LCP $O(\log N)$ | `DoubleRollingHash.hpp` |
| 文字列の任意位置挿入・削除・区間文字種数 | `DynamicStringTreap` | 各種操作 $O(\log N)$ | `DynamicStringTreap.hpp` |
| 複数文字列のプレフィックス検索・辞書 | `Trie` / `MapTrie<T>` | 挿入・検索 $O(|S|)$ | `Trie.hpp` |
| 単一始点最短経路 (重み付き正辺グラフ) | `dijkstra` | $O((V + E) \log V)$ | `Dijkstra.hpp` |
| 依存関係の順序付け・閉路検出・DAG判定 | `topological_sort` | $O(V + E)$ | `TopologicalSort.hpp` |
| 木のすべての頂点を根としたときの木DP | `Rerooting<...>` | $O(N)$ | `Rerooting.hpp` |
| 木の部分木クエリ・LCA・頂点間距離 | `EulerTour` | 構築 $O(N \log N)$<br>LCA $O(1)$ | `EulerTour.hpp` |
| 木の2点間LCA・重み付き距離・パスJump | `LowestCommonAncestor<T>` (`LCA`) | 構築 $O(N \log N)$<br>クエリ $O(\log N)$ | `LowestCommonAncestor.hpp` |
| 高速素数判定 ($N \le 10^{18}$) | `miller_rabin(n)` | $O(k \log^3 n)$ | `NumberTheory.hpp` |
| 素数判定・素因数分解・線形篩・区間篩 | `Prime` / `PrimeSieve` | クエリ $O(\log X)$ / $O(N^{1/4})$ | `Prime.hpp` |
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

#### BIT2D (動的2次元BIT・双対BIT・領域加算和)
2次元グリッド $H \times W$ に対し、オンライン（動的）に更新クエリと区間和クエリを $O(\log H \log W)$ で高速処理するデータ構造です。
「点加算・矩形和」の通常版、いもす法の動的版にあたる「矩形加算・一点取得」の双対版、そして「矩形加算・矩形和取得（RAQ）」の完全対応版の3クラスを提供します。

- **ファイル**: `mylib/BIT2D.hpp`
- **提供クラス**:
  - `BIT2D<T>`: 点加算 `add(r, c, v)`・矩形和取得 `sum(r1, c1, r2, c2)`
  - `DualBIT2D<T>`: 矩形加算 `add(r1, c1, r2, c2, v)`・1点取得 `get(r, c)`
  - `RangeAddBIT2D<T>`: 矩形加算 `add(r1, c1, r2, c2, v)`・矩形和取得 `sum(r1, c1, r2, c2)`（RAQ）
- **計算量**:
  - 各種クエリ: $O(\log H \log W)$
  - 空間計算量: $O(HW)$

```cpp
#include "mylib/BIT2D.hpp"

// 1. 通常の 2D BIT (点加算・矩形和取得)
BIT2D<long long> bit(5, 6);
bit.add(1, 2, 10);
bit.add(3, 4, 20);
assert(bit.sum(0, 0, 5, 6) == 30);
assert(bit.sum(1, 2, 2, 3) == 10);

// 2. 双対 2D BIT (矩形加算・一点取得)
DualBIT2D<long long> dbit(5, 6);
dbit.add(1, 1, 4, 5, 7); // [1, 4) x [1, 5) に +7
assert(dbit.get(1, 1) == 7);
assert(dbit.get(0, 0) == 0);

// 3. 領域加算・領域和取得 (Range Add Range Sum Query)
RangeAddBIT2D<long long> raq(6, 6);
raq.add(1, 1, 4, 4, 5); // 3x3 領域に +5
assert(raq.sum(1, 1, 4, 4) == 45); // 9マス * 5 = 45
assert(raq.get(1, 1) == 5);
```

<details>
<summary><b>ソースコードを展開 (BIT2D.hpp)</b></summary>

```cpp
#pragma once
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

// ============================================================================
// 動的2次元 BIT (2D Binary Indexed Tree / Fenwick Tree)
// ============================================================================
// 【概要】
// 2次元グリッド H x W に対し、オンライン（動的）に更新クエリと区間和クエリを
// 高速に処理するデータ構造です。
//
// 【提供クラス】
// 1. BIT2D<T>:
//    - 点加算 add(r, c, val) (O(log H log W))
//    - 矩形和 sum(r1, c1, r2, c2) (O(log H log W))
//    - プレフィックス和 prefix_sum(r, c) (O(log H log W))
//    - 1点取得 get(r, c) (O(log H log W))
//
// 2. DualBIT2D<T>: (双対2次元 BIT)
//    - 矩形加算 add(r1, c1, r2, c2, val) (O(log H log W))
//    - 1点取得 get(r, c) (O(log H log W))
//
// 3. RangeAddBIT2D<T>: (領域加算・領域和取得 RAQ - 4本の BIT を使用)
//    - 矩形加算 add(r1, c1, r2, c2, val) (O(log H log W))
//    - 矩形和 sum(r1, c1, r2, c2) (O(log H log W))
//
// ※すべての区間は 0-indexed 半開区間 [r1, r2) x [c1, c2) です。
// ============================================================================

// ----------------------------------------------------------------------------
// 1. BIT2D (点加算・矩形和取得)
// ----------------------------------------------------------------------------
template <typename T = long long>
class BIT2D {
private:
  int H = 0, W = 0;
  std::vector<T> tree;

  inline int id(int r, int c) const {
    return r * (W + 1) + c;
  }

public:
  BIT2D() {}
  BIT2D(int h, int w) : H(h), W(w), tree((h + 1) * (w + 1), T(0)) {}

  // 点 (r, c) に val を加算 (O(log H log W))
  void add(int r, int c, T val) {
    assert(0 <= r && r < H && 0 <= c && c < W);
    for (int i = r + 1; i <= H; i += i & -i) {
      for (int j = c + 1; j <= W; j += j & -j) {
        tree[id(i, j)] += val;
      }
    }
  }

  // 半開区間 [0, r) x [0, c) のプレフィックス総和 (O(log H log W))
  T prefix_sum(int r, int c) const {
    r = std::max(0, std::min(r, H));
    c = std::max(0, std::min(c, W));
    T res = 0;
    for (int i = r; i > 0; i -= i & -i) {
      for (int j = c; j > 0; j -= j & -j) {
        res += tree[id(i, j)];
      }
    }
    return res;
  }

  // 矩形区間 [r1, r2) x [c1, c2) の総和 (O(log H log W))
  T sum(int r1, int c1, int r2, int c2) const {
    if (r1 >= r2 || c1 >= c2) return T(0);
    assert(0 <= r1 && r2 <= H && 0 <= c1 && c2 <= W);
    return prefix_sum(r2, c2) - prefix_sum(r1, c2) - prefix_sum(r2, c1) + prefix_sum(r1, c1);
  }

  // 点 (r, c) の現在の値を取得 (O(log H log W))
  T get(int r, int c) const {
    return sum(r, c, r + 1, c + 1);
  }

  int height() const { return H; }
  int width() const { return W; }
};

// ----------------------------------------------------------------------------
// 2. DualBIT2D (矩形加算・一点取得 / 双対 2D BIT)
// ----------------------------------------------------------------------------
template <typename T = long long>
class DualBIT2D {
private:
  int H = 0, W = 0;
  BIT2D<T> bit;

public:
  DualBIT2D() {}
  DualBIT2D(int h, int w) : H(h), W(w), bit(h + 1, w + 1) {}

  // 矩形区間 [r1, r2) x [c1, c2) に val を加算 (O(log H log W))
  void add(int r1, int c1, int r2, int c2, T val) {
    if (r1 >= r2 || c1 >= c2) return;
    assert(0 <= r1 && r2 <= H && 0 <= c1 && c2 <= W);
    bit.add(r1, c1, val);
    bit.add(r1, c2, -val);
    bit.add(r2, c1, -val);
    bit.add(r2, c2, val);
  }

  // 点 (r, c) の現在の値を取得 (O(log H log W))
  T get(int r, int c) const {
    assert(0 <= r && r < H && 0 <= c && c < W);
    return bit.prefix_sum(r + 1, c + 1);
  }

  int height() const { return H; }
  int width() const { return W; }
};

// ----------------------------------------------------------------------------
// 3. RangeAddBIT2D (矩形加算・矩形和取得 RAQ / 4本の BIT を使用)
// ----------------------------------------------------------------------------
template <typename T = long long>
class RangeAddBIT2D {
private:
  int H = 0, W = 0;
  BIT2D<T> bit_xy, bit_x, bit_y, bit_0;

  void add_point(int r, int c, T val) {
    bit_xy.add(r, c, val);
    bit_x.add(r, c, -val * c);
    bit_y.add(r, c, -val * r);
    bit_0.add(r, c, val * r * c);
  }

public:
  RangeAddBIT2D() {}
  RangeAddBIT2D(int h, int w)
      : H(h), W(w),
        bit_xy(h + 1, w + 1), bit_x(h + 1, w + 1),
        bit_y(h + 1, w + 1), bit_0(h + 1, w + 1) {}

  // 矩形区間 [r1, r2) x [c1, c2) に val を一括加算 (O(log H log W))
  void add(int r1, int c1, int r2, int c2, T val) {
    if (r1 >= r2 || c1 >= c2) return;
    assert(0 <= r1 && r2 <= H && 0 <= c1 && c2 <= W);
    add_point(r1, c1, val);
    add_point(r1, c2, -val);
    add_point(r2, c1, -val);
    add_point(r2, c2, val);
  }

  // 半開区間 [0, r) x [0, c) のプレフィックス総和 (O(log H log W))
  T prefix_sum(int r, int c) const {
    r = std::max(0, std::min(r, H));
    c = std::max(0, std::min(c, W));
    return bit_xy.prefix_sum(r, c) * r * c +
           bit_x.prefix_sum(r, c) * r +
           bit_y.prefix_sum(r, c) * c +
           bit_0.prefix_sum(r, c);
  }

  // 矩形区間 [r1, r2) x [c1, c2) の総和 (O(log H log W))
  T sum(int r1, int c1, int r2, int c2) const {
    if (r1 >= r2 || c1 >= c2) return T(0);
    assert(0 <= r1 && r2 <= H && 0 <= c1 && c2 <= W);
    return prefix_sum(r2, c2) - prefix_sum(r1, c2) - prefix_sum(r2, c1) + prefix_sum(r1, c1);
  }

  // 点 (r, c) の現在の値を取得 (O(log H log W))
  T get(int r, int c) const {
    return sum(r, c, r + 1, c + 1);
  }

  int height() const { return H; }
  int width() const { return W; }
};

template <typename T = long long>
using FenwickTree2D = BIT2D<T>;

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
Treap による平衡二分探索木です。重複要素を持たない `set`（一意な集合）として動作し、値からインデックス、インデックスから値を高速に取得できます。
- **ファイル**: `mylib/OrderStatisticSet.hpp`（短縮エイリアス: `OSS<T>`）
- **計算量**: 挿入・削除・検索・$k$ 番目取得 いずれも $O(\log N)$

```cpp
#include "mylib/OrderStatisticSet.hpp"

OrderStatisticSet<long long> oss; // または短縮形: OSS<long long> oss;
oss.insert(10);
oss.insert(20);
oss.insert(30);
oss.insert(10); // 既に存在するため挿入されず false を返す (set と同様)

int cnt = oss.get_index(15);                   // 15 未満の要素数 (ここでは 10 の 1 個)
const long long *val = oss.get_from_bottom(0); // 昇順 0 番目 (最小値: 10) のポインタ
const long long *top = oss.get_from_top(0);    // 降順 0 番目 (最大値: 30) のポインタ
oss.erase(10);                                 // 10 を削除 (存在すれば true)

// multiset (重複要素) として使いたい場合は pair<T, int> {値, 一意なID} を使用
OSS<pair<long long, int>> ms;
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

#### MosAlgorithm (Mo's algorithm)
オフライン区間クエリを平方分割と伸縮操作により効率的に処理するアルゴリズムです。偶奇ソートによる定数倍最適化を内蔵しています。
- **ファイル**: `mylib/MosAlgorithm.hpp` (または `mylib/Mo.hpp`)
- **計算量**: $O(N \sqrt{Q})$（伸縮操作が $O(1)$ の場合）

```cpp
#include "mylib/MosAlgorithm.hpp" // または "mylib/Mo.hpp"

int n = 8;
MosAlgorithm mo(n); // using Mo = MosAlgorithm; も利用可能

// クエリ [l, r) を追加 (0-indexed 半開区間)
mo.add_query(0, 4);
mo.add_query(2, 6);

vector<int> ans(2);
vector<int> cnt(100, 0);
int distinct_cnt = 0;

auto add = [&](int idx) {
    if (cnt[a[idx]] == 0) distinct_cnt++;
    cnt[a[idx]]++;
};
auto del = [&](int idx) {
    cnt[a[idx]]--;
    if (cnt[a[idx]] == 0) distinct_cnt--;
};
auto out = [&](int q_id) {
    ans[q_id] = distinct_cnt;
};

mo.run(add, del, out);
```

<details>
<summary><b>ソースコードを展開 (MosAlgorithm.hpp)</b></summary>

```cpp
#pragma once
#include <bits/stdc++.h>

using namespace std;

struct MosAlgorithm {
  int n;
  struct Query {
    int l, r, id;
  };
  vector<Query> queries;
  int bucket_size;

  MosAlgorithm(int n = 0, int bucket_size = -1)
      : n(n), bucket_size(bucket_size) {}

  void add_query(int l, int r) {
    queries.push_back({l, r, (int)queries.size()});
  }

  template <typename Add, typename Del, typename Out>
  void run(const Add &add, const Del &del, const Out &out) {
    int q = queries.size();
    if (q == 0)
      return;

    int bs = bucket_size;
    if (bs <= 0) {
      bs = max(1, (int)(n / max(1.0, sqrt(q * 2.0 / 3.0))));
    }

    sort(queries.begin(), queries.end(), [&](const Query &a, const Query &b) {
      int ablock = a.l / bs, bblock = b.l / bs;
      if (ablock != bblock)
        return ablock < bblock;
      return (ablock & 1) ? (a.r > b.r) : (a.r < b.r);
    });

    int cur_l = 0, cur_r = 0;
    for (const auto &qry : queries) {
      while (cur_l > qry.l) {
        cur_l--;
        add(cur_l);
      }
      while (cur_r < qry.r) {
        add(cur_r);
        cur_r++;
      }
      while (cur_l < qry.l) {
        del(cur_l);
        cur_l++;
      }
      while (cur_r > qry.r) {
        cur_r--;
        del(cur_r);
      }
      out(qry.id);
    }
  }

  template <typename AddL, typename AddR, typename DelL, typename DelR,
            typename Out>
  void run(const AddL &add_l, const AddR &add_r, const DelL &del_l,
           const DelR &del_r, const Out &out) {
    int q = queries.size();
    if (q == 0)
      return;

    int bs = bucket_size;
    if (bs <= 0) {
      bs = max(1, (int)(n / max(1.0, sqrt(q * 2.0 / 3.0))));
    }

    sort(queries.begin(), queries.end(), [&](const Query &a, const Query &b) {
      int ablock = a.l / bs, bblock = b.l / bs;
      if (ablock != bblock)
        return ablock < bblock;
      return (ablock & 1) ? (a.r > b.r) : (a.r < b.r);
    });

    int cur_l = 0, cur_r = 0;
    for (const auto &qry : queries) {
      while (cur_l > qry.l) {
        cur_l--;
        add_l(cur_l);
      }
      while (cur_r < qry.r) {
        add_r(cur_r);
        cur_r++;
      }
      while (cur_l < qry.l) {
        del_l(cur_l);
        cur_l++;
      }
      while (cur_r > qry.r) {
        cur_r--;
        del_r(cur_r);
      }
      out(qry.id);
    }
  }
};

using Mo = MosAlgorithm;
```

</details>

#### TwoPointers (尺取り法)
単調性のある区間条件に対し、各左端 `l` に対する極大な右端 `r`（半開区間 `[l, r)`）を $O(N)$ で走査するヘルパー関数です。`l == r` の追い越し防止や境界チェックを自動処理します。
- **ファイル**: `mylib/TwoPointers.hpp`
- **計算量**: $O(N)$（状態更新が $O(1)$ の場合）

```cpp
#include "mylib/TwoPointers.hpp"

long long sum = 0, ans = 0;

// 使い方 1: eval(l, r) で極大区間 [l, r) ごとに処理
two_pointers(
    n,
    [&](int r) { return sum + a[r] <= k; }, // can_add(r): 追加可能か
    [&](int r) { sum += a[r]; },            // add(r): 右端を追加
    [&](int l) { sum -= a[l]; },            // del(l): 左端を削除
    [&](int l, int r) { ans += r - l; }     // eval(l, r): 極大区間 [l, r) の評価
);

// 使い方 2: eval を省略して各 l に対する極大右端 max_r[l] の配列を取得
auto max_r = two_pointers(
    n,
    [&](int r) { return sum + a[r] <= k; },
    [&](int r) { sum += a[r]; },
    [&](int l) { sum -= a[l]; }
);
```

<details>
<summary><b>ソースコードを展開 (TwoPointers.hpp)</b></summary>

```cpp
#pragma once
#include <bits/stdc++.h>

using namespace std;

template <typename CanAdd, typename Add, typename Del, typename Eval>
void two_pointers(int n, const CanAdd &can_add, const Add &add, const Del &del,
                  const Eval &eval) {
  int r = 0;
  for (int l = 0; l < n; l++) {
    while (r < n && can_add(r)) {
      add(r);
      r++;
    }
    eval(l, r);
    if (l == r) {
      r++;
    } else {
      del(l);
    }
  }
}

template <typename CanAdd, typename Add, typename Del>
vector<int> two_pointers(int n, const CanAdd &can_add, const Add &add,
                         const Del &del) {
  vector<int> max_r(n);
  two_pointers(
      n, can_add, add, del, [&](int l, int r) { max_r[l] = r; });
  return max_r;
}
```

</details>

#### ZobristHash (多重集合・区間一致・順列判定・動的ハッシュ)
64ビット乱数2組を用いた安全な128ビットハッシュ値構造体（`Hash128`）による、衝突確率 $\approx 2^{-128}$（実質ゼロ）のゾブリストハッシュライブラリです。
加算による**多重集合（Multiset）ハッシュ**と、XORによる**集合（Set）ハッシュ・奇偶判定**の両方に対応し、静的配列から点更新、動的集合、2次元盤面探索まで幅広くカバーしています。

- **ファイル**: `mylib/ZobristHash.hpp`
- **主要クラス**:
  - `ZobristHash`: 静的配列に対する区間多重集合一致判定・区間順列判定・全要素偶数出現判定（クエリ $O(1)$）
  - `DynamicZobristHash`: 点更新クエリ $O(\log N)$、区間多重集合ハッシュ $O(\log N)$（Fenwick Tree基盤）
  - `ZobristSet`: 動的集合の状態ハッシュ（挿入・削除・反転 $O(1)$）
  - `ZobristMultiset`: 要素カウント付き動的多重集合の状態ハッシュ（挿入・削除 $O(1)$）
  - `ZobristGrid`: $H \times W$ 盤面・ゲーム木探索用状態ハッシュ（マス更新・スワップ $O(1)$）
- **計算量**:
  - 構築: $O(N)$（`ZobristGrid` は $O(HW \cdot C)$）
  - 区間多重集合一致 `same_multiset`: $O(1)$（動的版は $O(\log N)$）
  - 区間順列判定 `is_permutation`: $O(1)$（動的版は $O(\log N)$）
  - 全要素偶数出現判定 `is_all_even`: $O(1)$（動的版は $O(\log N)$）
  - 集合・盤面更新: $O(1)$

```cpp
#include "mylib/ZobristHash.hpp"

// 1. 区間多重集合の一致判定 (ABC367 F - Rearrange Query 型)
vector<int> a = {1, 2, 3, 2, 1};
vector<int> b = {2, 1, 1, 3, 2};
ZobristHash ha(a), hb(b);

// a[0..5) と b[0..5) は同一の多重集合か？ -> true (O(1))
assert(ha.same_multiset(0, 5, hb, 0, 5));

// 2. 区間順列判定 (1..L の並び替えになっているか？)
vector<int> p = {4, 2, 1, 3, 5};
ZobristHash hp(p);
assert(hp.is_permutation(0, 4, 1)); // p[0..4) は 1..4 の順列 -> true (O(1))
assert(hp.is_permutation(1, 4, 1)); // p[1..4) は {2, 1, 3} で 1..3 の順列 -> true (O(1))

// 3. 奇偶判定 (区間内のすべての要素が偶数回現れるか？)
vector<int> ev = {5, 9, 5, 9};
ZobristHash hev(ev);
assert(hev.is_all_even(0, 4)); // true (O(1))

// 4. 点更新クエリ対応 (DynamicZobristHash)
DynamicZobristHash<int> dh(a);
dh.set(2, 5); // a[2] を 5 に変更 (O(log N))
assert(dh.get_multiset_hash(0, 5) != ha.get_multiset_hash(0, 5));

// 5. 動的集合・多重集合・盤面探索
ZobristSet<int> zs;
zs.insert(10);
zs.toggle(20);
Hash128 set_hash = zs.get_hash();

ZobristGrid grid(3, 3, 10); // 3x3 盤面
grid.set(1, 1, 5);
grid.swap_cells(0, 0, 1, 1);
Hash128 grid_hash = grid.get_hash();
```

<details>
<summary><b>ソースコードを展開 (ZobristHash.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// 128ビット Zobrist Hash (ゾブリストハッシュ) ライブラリ
// ============================================================================
// 【主な機能】
// 1. Hash128: 64ビット×2の独立した乱数による強固な128ビットハッシュ値構造体
//    ・衝突確率: ~2^-128 (事実上衝突不可能)
//    ・加算 (+, -) による「多重集合 (multiset) ハッシュ」対応
//    ・排他的論理和 (^) による「集合 (set) ハッシュ / 奇偶判定」対応
// 2. ZobristHash: 静的配列に対する O(1) 区間多重集合一致判定・区間順列判定
//    ・same_multiset(l1, r1, l2, r2): 2つの区間の多重集合が同一か O(1) で判定 (ABC367F 等)
//    ・is_permutation(l, r, start): 区間が 1..L の順列か O(1) で判定
//    ・is_all_even(l, r): 区間内の全要素が偶数回出現するか O(1) で判定
// 3. DynamicZobristHash: 点更新 (1点変更) に O(log N) で対応する区間ハッシュ (Fenwick Tree基盤)
// 4. ZobristSet: 動的集合の状態ハッシュ (挿入・削除・反転 O(1))
// 5. ZobristMultiset: 動的多重集合の状態ハッシュ (挿入・削除 O(1))
// 6. ZobristGrid: 2次元盤面・ゲーム木探索用状態ハッシュ (マス更新・スワップ O(1))
// ============================================================================

namespace zobrist_internal {
  static inline uint64_t splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
  }

  // 実行時シード (クロック + アドレスランダム化によるハック攻撃防止)
  static inline uint64_t get_random_seed1() {
    static uint64_t seed = []() {
      uint64_t t = std::chrono::high_resolution_clock::now().time_since_epoch().count();
      uint64_t addr = reinterpret_cast<uint64_t>(&seed);
      return splitmix64(t ^ splitmix64(addr));
    }();
    return seed;
  }

  static inline uint64_t get_random_seed2() {
    static uint64_t seed = splitmix64(get_random_seed1() ^ 0xabcdef0123456789ULL);
    return seed;
  }
} // namespace zobrist_internal

// 128ビットハッシュ値構造体
struct Hash128 {
  uint64_t h1, h2;

  constexpr Hash128() : h1(0), h2(0) {}
  constexpr Hash128(uint64_t v1, uint64_t v2) : h1(v1), h2(v2) {}
  constexpr Hash128(uint64_t v) : h1(v), h2(zobrist_internal::splitmix64(v)) {}

  Hash128 operator^(const Hash128 &o) const { return {h1 ^ o.h1, h2 ^ o.h2}; }
  Hash128 operator+(const Hash128 &o) const { return {h1 + o.h1, h2 + o.h2}; }
  Hash128 operator-(const Hash128 &o) const { return {h1 - o.h1, h2 - o.h2}; }
  Hash128 operator*(uint64_t k) const { return {h1 * k, h2 * k}; }

  Hash128 &operator^=(const Hash128 &o) { h1 ^= o.h1; h2 ^= o.h2; return *this; }
  Hash128 &operator+=(const Hash128 &o) { h1 += o.h1; h2 += o.h2; return *this; }
  Hash128 &operator-=(const Hash128 &o) { h1 -= o.h1; h2 -= o.h2; return *this; }
  Hash128 &operator*=(uint64_t k) { h1 *= k; h2 *= k; return *this; }

  bool operator==(const Hash128 &o) const { return h1 == o.h1 && h2 == o.h2; }
  bool operator!=(const Hash128 &o) const { return !(*this == o); }
  bool operator<(const Hash128 &o) const { return h1 < o.h1 || (h1 == o.h1 && h2 < o.h2); }

  uint64_t get64() const { return h1; }
  explicit operator uint64_t() const { return h1; }
  bool is_zero() const { return h1 == 0 && h2 == 0; }
};

inline std::ostream &operator<<(std::ostream &os, const Hash128 &h) {
  return os << "Hash128(" << h.h1 << ", " << h.h2 << ")";
}

namespace std {
  template <>
  struct hash<Hash128> {
    size_t operator()(const Hash128 &h) const noexcept {
      return h.h1 ^ (h.h2 + 0x9e3779b97f4a7c15ULL + (h.h1 << 6) + (h.h1 >> 2));
    }
  };
} // namespace std

// 任意の値に対する擬似乱数ハッシュ値生成関数 (SplitMix64ベース)
template <typename T>
inline typename enable_if<is_integral<T>::value, Hash128>::type zobrist_hash_value(T x) {
  uint64_t u = static_cast<uint64_t>(x);
  uint64_t v1 = zobrist_internal::splitmix64(u ^ zobrist_internal::get_random_seed1());
  uint64_t v2 = zobrist_internal::splitmix64(u ^ zobrist_internal::get_random_seed2());
  return Hash128(v1, v2);
}

inline Hash128 zobrist_hash_value(const string &s) {
  uint64_t u1 = 0, u2 = 0;
  for (char c : s) {
    u1 = zobrist_internal::splitmix64(u1 ^ (uint8_t)c);
    u2 = zobrist_internal::splitmix64(u2 + (uint8_t)c);
  }
  return Hash128(u1, u2);
}

// 順列ハッシュの事前計算用テーブル
struct ZobristPermutationTable {
  vector<Hash128> pref_add;

  ZobristPermutationTable() {
    pref_add.push_back(Hash128(0, 0));
  }

  void extend(int n) {
    while ((int)pref_add.size() <= n) {
      int idx = pref_add.size();
      Hash128 next_h = pref_add.back() + zobrist_hash_value(idx);
      pref_add.push_back(next_h);
    }
  }

  Hash128 get(long long start, int len) {
    if (len <= 0) return Hash128(0, 0);
    if (start >= 1 && start + len - 1 <= 2000000) {
      extend(start + len - 1);
      return pref_add[start + len - 1] - pref_add[start - 1];
    }
    // start が 1..2000000 の範囲外の場合はループ計算
    Hash128 res(0, 0);
    for (int i = 0; i < len; ++i) {
      res += zobrist_hash_value(start + i);
    }
    return res;
  }
};

inline ZobristPermutationTable &get_global_perm_table() {
  static ZobristPermutationTable table;
  return table;
}

template <typename T> struct DynamicZobristHash;

// ============================================================================
// 1. ZobristHash (静的配列に対する区間ハッシュ・多重集合一致・順列判定)
// ============================================================================
struct ZobristHash {
  int n;
  vector<Hash128> pref_add; // 累積和 (多重集合用)
  vector<Hash128> pref_xor; // 累積XOR (奇偶判定・集合用)

  ZobristHash() : n(0) {}

  template <typename T>
  ZobristHash(const vector<T> &a) {
    build(a);
  }

  ZobristHash(const string &s) {
    vector<int> a(s.begin(), s.end());
    build(a);
  }

  template <typename T>
  void build(const vector<T> &a) {
    n = a.size();
    pref_add.assign(n + 1, Hash128(0, 0));
    pref_xor.assign(n + 1, Hash128(0, 0));
    for (int i = 0; i < n; ++i) {
      Hash128 h = zobrist_hash_value(a[i]);
      pref_add[i + 1] = pref_add[i] + h;
      pref_xor[i + 1] = pref_xor[i] ^ h;
    }
  }

  int size() const { return n; }

  // [l, r) の多重集合ハッシュ (O(1))
  Hash128 get_multiset_hash(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    return pref_add[r] - pref_add[l];
  }

  // [l, r) の XOR ハッシュ (O(1))
  Hash128 get_xor_hash(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    return pref_xor[r] ^ pref_xor[l];
  }

  // 同一配列内の [l1, r1) と [l2, r2) の多重集合が一致するか (O(1))
  bool same_multiset(int l1, int r1, int l2, int r2) const {
    if (r1 - l1 != r2 - l2) return false;
    return get_multiset_hash(l1, r1) == get_multiset_hash(l2, r2);
  }

  // 別配列 other の [l2, r2) との多重集合が一致するか (O(1))
  bool same_multiset(int l1, int r1, const ZobristHash &other, int l2, int r2) const {
    if (r1 - l1 != r2 - l2) return false;
    return get_multiset_hash(l1, r1) == other.get_multiset_hash(l2, r2);
  }

  // DynamicZobristHash との多重集合が一致するか (O(log N))
  template <typename U>
  bool same_multiset(int l1, int r1, const DynamicZobristHash<U> &other, int l2, int r2) const;

  // 同一配列内の [l1, r1) と [l2, r2) の XOR ハッシュが一致するか (O(1))
  bool same_xor(int l1, int r1, int l2, int r2) const {
    return get_xor_hash(l1, r1) == get_xor_hash(l2, r2);
  }

  // 別配列 other の [l2, r2) との XOR ハッシュが一致するか (O(1))
  bool same_xor(int l1, int r1, const ZobristHash &other, int l2, int r2) const {
    return get_xor_hash(l1, r1) == other.get_xor_hash(l2, r2);
  }

  // DynamicZobristHash との XOR ハッシュが一致するか (O(log N))
  template <typename U>
  bool same_xor(int l1, int r1, const DynamicZobristHash<U> &other, int l2, int r2) const;

  // [l, r) 内のすべての要素が偶数回出現するか (O(1))
  bool is_all_even(int l, int r) const {
    return get_xor_hash(l, r).is_zero();
  }

  // [l, r) が {start, start+1, ..., start+(r-l)-1} の順列になっているか (O(1))
  bool is_permutation(int l, int r, long long start = 1) const {
    int len = r - l;
    if (len <= 0) return true;
    Hash128 target = get_global_perm_table().get(start, len);
    return get_multiset_hash(l, r) == target;
  }
};

// ============================================================================
// 2. DynamicZobristHash (点更新クエリ対応の区間ハッシュ・Fenwick Tree基盤)
// ============================================================================
template <typename T = long long>
struct DynamicZobristHash {
  int n;
  vector<T> data;
  vector<Hash128> bit_add;
  vector<Hash128> bit_xor;

  DynamicZobristHash(int n, T default_val = T())
      : n(n), data(n, default_val), bit_add(n + 1, Hash128(0, 0)), bit_xor(n + 1, Hash128(0, 0)) {
    if (default_val != T()) {
      build(data);
    }
  }

  DynamicZobristHash(const vector<T> &a)
      : n(a.size()), data(a), bit_add(n + 1, Hash128(0, 0)), bit_xor(n + 1, Hash128(0, 0)) {
    build(a);
  }

  void build(const vector<T> &a) {
    data = a;
    for (int i = 0; i < n; ++i) {
      Hash128 h = zobrist_hash_value(a[i]);
      bit_add[i + 1] = h;
      bit_xor[i + 1] = h;
    }
    for (int i = 1; i <= n; ++i) {
      int p = i + (i & -i);
      if (p <= n) {
        bit_add[p] += bit_add[i];
        bit_xor[p] ^= bit_xor[i];
      }
    }
  }

  // a[idx] = new_val に更新 (O(log N))
  void set(int idx, T new_val) {
    assert(0 <= idx && idx < n);
    Hash128 old_h = zobrist_hash_value(data[idx]);
    Hash128 new_h = zobrist_hash_value(new_val);
    data[idx] = new_val;
    Hash128 diff_add = new_h - old_h;
    Hash128 diff_xor = new_h ^ old_h;
    for (int i = idx + 1; i <= n; i += i & -i) {
      bit_add[i] += diff_add;
      bit_xor[i] ^= diff_xor;
    }
  }

  T get(int idx) const {
    assert(0 <= idx && idx < n);
    return data[idx];
  }

  int size() const { return n; }

  // [l, r) の多重集合ハッシュ (O(log N))
  Hash128 get_multiset_hash(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    Hash128 res_r(0, 0);
    for (int i = r; i > 0; i -= i & -i) res_r += bit_add[i];
    Hash128 res_l(0, 0);
    for (int i = l; i > 0; i -= i & -i) res_l += bit_add[i];
    return res_r - res_l;
  }

  // [l, r) の XOR ハッシュ (O(log N))
  Hash128 get_xor_hash(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    Hash128 res_r(0, 0);
    for (int i = r; i > 0; i -= i & -i) res_r ^= bit_xor[i];
    Hash128 res_l(0, 0);
    for (int i = l; i > 0; i -= i & -i) res_l ^= bit_xor[i];
    return res_r ^ res_l;
  }

  // 同一配列内の [l1, r1) と [l2, r2) の多重集合が一致するか (O(log N))
  bool same_multiset(int l1, int r1, int l2, int r2) const {
    if (r1 - l1 != r2 - l2) return false;
    return get_multiset_hash(l1, r1) == get_multiset_hash(l2, r2);
  }

  // 別配列 other の [l2, r2) との多重集合が一致するか (O(log N))
  bool same_multiset(int l1, int r1, const DynamicZobristHash<T> &other, int l2, int r2) const {
    if (r1 - l1 != r2 - l2) return false;
    return get_multiset_hash(l1, r1) == other.get_multiset_hash(l2, r2);
  }

  // 静的 ZobristHash other の [l2, r2) との多重集合が一致するか (O(log N))
  bool same_multiset(int l1, int r1, const ZobristHash &other, int l2, int r2) const {
    if (r1 - l1 != r2 - l2) return false;
    return get_multiset_hash(l1, r1) == other.get_multiset_hash(l2, r2);
  }

  // 同一配列内の [l1, r1) と [l2, r2) の XOR ハッシュが一致するか (O(log N))
  bool same_xor(int l1, int r1, int l2, int r2) const {
    return get_xor_hash(l1, r1) == get_xor_hash(l2, r2);
  }

  // 別配列 other の [l2, r2) との XOR ハッシュが一致するか (O(log N))
  bool same_xor(int l1, int r1, const DynamicZobristHash<T> &other, int l2, int r2) const {
    return get_xor_hash(l1, r1) == other.get_xor_hash(l2, r2);
  }

  // 静的 ZobristHash other の [l2, r2) との XOR ハッシュが一致するか (O(log N))
  bool same_xor(int l1, int r1, const ZobristHash &other, int l2, int r2) const {
    return get_xor_hash(l1, r1) == other.get_xor_hash(l2, r2);
  }

  // [l, r) 内のすべての要素が偶数回出現するか (O(log N))
  bool is_all_even(int l, int r) const {
    return get_xor_hash(l, r).is_zero();
  }

  // [l, r) が {start, start+1, ..., start+(r-l)-1} の順列になっているか (O(log N))
  bool is_permutation(int l, int r, long long start = 1) const {
    int len = r - l;
    if (len <= 0) return true;
    Hash128 target = get_global_perm_table().get(start, len);
    return get_multiset_hash(l, r) == target;
  }
};

template <typename U>
inline bool ZobristHash::same_multiset(int l1, int r1, const DynamicZobristHash<U> &other, int l2, int r2) const {
  if (r1 - l1 != r2 - l2) return false;
  return get_multiset_hash(l1, r1) == other.get_multiset_hash(l2, r2);
}

template <typename U>
inline bool ZobristHash::same_xor(int l1, int r1, const DynamicZobristHash<U> &other, int l2, int r2) const {
  return get_xor_hash(l1, r1) == other.get_xor_hash(l2, r2);
}

// ============================================================================
// 3. ZobristSet (動的集合の状態ハッシュ)
// ============================================================================
template <typename T = long long>
struct ZobristSet {
  unordered_set<T> st;
  Hash128 current_hash;

  ZobristSet() : current_hash(0, 0) {}

  // 要素 x を追加 (既に存在すれば false を返す) (O(1))
  bool insert(const T &x) {
    if (st.insert(x).second) {
      current_hash ^= zobrist_hash_value(x);
      return true;
    }
    return false;
  }

  // 要素 x を削除 (存在しなかったら false を返す) (O(1))
  bool erase(const T &x) {
    if (st.erase(x)) {
      current_hash ^= zobrist_hash_value(x);
      return true;
    }
    return false;
  }

  // 要素 x の有無を反転 (O(1))
  void toggle(const T &x) {
    if (st.count(x)) {
      st.erase(x);
    } else {
      st.insert(x);
    }
    current_hash ^= zobrist_hash_value(x);
  }

  bool contains(const T &x) const {
    return st.count(x);
  }

  int size() const {
    return st.size();
  }

  bool empty() const {
    return st.empty();
  }

  Hash128 get_hash() const {
    return current_hash;
  }

  uint64_t get64() const {
    return current_hash.get64();
  }

  void clear() {
    st.clear();
    current_hash = Hash128(0, 0);
  }
};

// ============================================================================
// 4. ZobristMultiset (動的多重集合の状態ハッシュ)
// ============================================================================
template <typename T = long long>
struct ZobristMultiset {
  unordered_map<T, long long> counts;
  Hash128 current_hash;
  long long total_count;

  ZobristMultiset() : current_hash(0, 0), total_count(0) {}

  // 要素 x を k 個追加 (O(1))
  void insert(const T &x, long long k = 1) {
    if (k <= 0) return;
    counts[x] += k;
    total_count += k;
    current_hash += zobrist_hash_value(x) * (uint64_t)k;
  }

  // 要素 x を最大 k 個削除 (O(1))
  bool erase(const T &x, long long k = 1) {
    if (k <= 0) return false;
    auto it = counts.find(x);
    if (it == counts.end() || it->second == 0) return false;
    long long actual = min(it->second, k);
    it->second -= actual;
    if (it->second == 0) counts.erase(it);
    total_count -= actual;
    current_hash -= zobrist_hash_value(x) * (uint64_t)actual;
    return true;
  }

  long long count(const T &x) const {
    auto it = counts.find(x);
    return it == counts.end() ? 0 : it->second;
  }

  long long size() const {
    return total_count;
  }

  bool empty() const {
    return total_count == 0;
  }

  Hash128 get_hash() const {
    return current_hash;
  }

  uint64_t get64() const {
    return current_hash.get64();
  }

  void clear() {
    counts.clear();
    current_hash = Hash128(0, 0);
    total_count = 0;
  }
};

// ============================================================================
// 5. ZobristGrid (2次元盤面・ゲーム木・グリッド探索用状態ハッシュ)
// ============================================================================
struct ZobristGrid {
  int h, w, num_states;
  vector<vector<int>> grid;
  vector<vector<vector<Hash128>>> table;
  Hash128 current_hash;

  ZobristGrid(int h, int w, int num_states = 2)
      : h(h), w(w), num_states(num_states), grid(h, vector<int>(w, 0)), current_hash(0, 0) {
    table.resize(h, vector<vector<Hash128>>(w, vector<Hash128>(num_states)));
    for (int r = 0; r < h; ++r) {
      for (int c = 0; c < w; ++c) {
        for (int v = 0; v < num_states; ++v) {
          uint64_t key = ((uint64_t)r << 32) ^ ((uint64_t)c << 16) ^ (uint64_t)v;
          table[r][c][v] = zobrist_hash_value(key);
        }
        current_hash ^= table[r][c][0];
      }
    }
  }

  // (r, c) の値を new_val に更新 (O(1))
  void set(int r, int c, int new_val) {
    assert(0 <= r && r < h && 0 <= c && c < w);
    assert(0 <= new_val && new_val < num_states);
    int old_val = grid[r][c];
    if (old_val == new_val) return;
    grid[r][c] = new_val;
    current_hash ^= table[r][c][old_val];
    current_hash ^= table[r][c][new_val];
  }

  // 2つのマス (r1, c1) と (r2, c2) の値をスワップ (15パズル等) (O(1))
  void swap_cells(int r1, int c1, int r2, int c2) {
    if (r1 == r2 && c1 == c2) return;
    int v1 = grid[r1][c1];
    int v2 = grid[r2][c2];
    set(r1, c1, v2);
    set(r2, c2, v1);
  }

  int get(int r, int c) const {
    assert(0 <= r && r < h && 0 <= c && c < w);
    return grid[r][c];
  }

  Hash128 get_hash() const {
    return current_hash;
  }

  uint64_t get64() const {
    return current_hash.get64();
  }
};
```

</details>

#### IntervalSet / IntervalMap (区間を set で管理するテク・mex・Chtholly Tree)
互いに交差しない閉区間 $[l, r]$ の集合を `std::set` で管理し、区間の追加・削除・重複統合（マージ）・被覆判定・要素数集計・mex（最小未被覆値検索）を高速に行うデータ構造です。
また、区間に値を割り当てて区間更新（区間代入）と走査を行う **Chtholly Tree (Old Driver Tree)** としての `IntervalMap` も併せて提供しています。

- **ファイル**: `mylib/IntervalSet.hpp`
- **主要クラス**:
  - `IntervalSet<T>`: 互いに素な区間集合の管理（デフォルト型: `long long`）
  - `IntervalMap<T, V>`: 区間に値・色を割り当てる Chtholly Tree（デフォルト型: `long long, long long`）
- **主な機能**:
  - `insert(l, r)`: 区間 $[l, r]$ を追加し、交差・隣接区間を自動結合。新たに被覆された要素数の差分を返却（ならし $O(\log N)$）
  - `erase(l, r)`: 区間 $[l, r]$ を削除し、必要に応じて既存区間を切り分け。削除された要素数の差分を返却（ならし $O(\log N)$）
  - `mex(x)`: $x$ 以上の最小の「どの区間にも含まれていない（未被覆の）」整数を取得（$O(\log N)$）
  - `prev_absent(x)`: $x$ 以下の最大の「どの区間にも含まれていない」整数を取得（$O(\log N)$）
  - `contains(x)` / `contains(l, r)`: 点または区間が完全に被覆されているか判定（$O(\log N)$）
  - `intersects(l, r)`: 区間が既存の区間と一部でも交差するか判定（$O(\log N)$）
  - `get(x)`: 点 $x$ を含む区間 $[l, r]$ を `std::optional` で取得（$O(\log N)$）
  - `covered_length()`: 被覆されている整数の総数（$O(1)$）
  - **コールバック機能 (drken式)**:
    - 可逆操作用: `insert(l, r, on_add, on_erase)` / `erase(l, r, on_add, on_erase)`
    - 新規・削除差分のみ用: `insert_strictly_new(l, r, on_new)` / `erase_strictly_removed(l, r, on_removed)`
  - `IntervalMap`: `split(x)`, `assign(l, r, val)`（ならし $O(\log N)$）, `get(x)`, `for_each(l, r, fn)`
- **計算量**:
  - 挿入・削除・区間代入: ならし $O(\log N)$（$N$ は管理されている区間数）
  - 被覆長・要素数取得: $O(1)$
  - 判定・検索・mex: $O(\log N)$

```cpp
#include "mylib/IntervalSet.hpp"

// 1. 基本的な区間管理とマージ
IntervalSet<long long> is;
is.insert(1, 3);
is.insert(5, 7);
is.insert(3, 5); // [1, 3], [3, 5], [5, 7] が [1, 7] に自動統合される
assert(is.size() == 1);
assert(is.covered_length() == 7);

// 2. 高速 mex 取得 (ABC228 D など)
IntervalSet<long long> used;
used.insert(0, 4);
assert(used.mex(0) == 5); // 0以上で未被覆の最小整数 -> 5
used.insert(5);
assert(used.mex(0) == 6);

// 3. 区間の削除
is.erase(2, 4); // [1, 7] から [2, 4] を削り、[1, 1] と [5, 7] に分割
assert(is.size() == 2);
assert(!is.contains(3));

// 4. コールバック (新規追加された部分のみ処理)
IntervalSet<long long> tracker;
tracker.insert(10, 20);
tracker.insert_strictly_new(15, 25, [](long long l, long long r) {
    // 実際に新しく被覆された [21, 25] のみ呼ばれる
    cout << "New interval: [" << l << ", " << r << "]\n";
});

// 5. IntervalMap (Chtholly Tree / 区間カラーリング)
IntervalMap<long long, int> cmap(0, 100, 0); // 初期区間 [0, 100] を色 0 で初期化
cmap.assign(10, 20, 1); // [10, 20] を色 1 に塗る
cmap.assign(15, 25, 2); // [15, 25] を色 2 に塗る
assert(cmap.get(12) == 1);
assert(cmap.get(18) == 2);
assert(cmap.get(26) == 0);
```

<details>
<summary><b>ソースコードを展開 (IntervalSet.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// 区間を set で管理するテク (Interval Set & Interval Map)
// ============================================================================
// 【概要】
// 互いに交差しない閉区間 [l, r] の集合を std::set で管理し、区間の追加・削除・
// 統合（マージ）・被覆判定・mex（最小未被覆値）検索を高速に行うデータ構造です。
//
// 【提供クラス】
// 1. IntervalSet<T>:
//    - 閉区間 [l, r] の集合を管理 (整数型 T, デフォルト long long)
//    - insert(l, r): 区間の追加。重複・隣接区間をマージし、被覆長の変化量を返す
//    - erase(l, r): 区間の削除。被覆部分を削り、必要に応じて区間を分割
//    - contains(x) / contains(l, r): 点または区間が完全に被覆されているか判定
//    - intersects(l, r): 区間と一部でも交差するか判定
//    - mex(x): x 以上の最小の「被覆されていない」整数を O(log N) で取得 (ABC228D等)
//    - prev_absent(x): x 以下の最大の「被覆されていない」整数を取得
//    - get(x): 点 x を含む区間 [l, r] を取得 (存在しない場合は nullopt)
//    - covered_length(): 被覆されている整数の総数 (O(1))
//    - size(): 独立した区間の個数 (O(1))
//    - コールバック付き insert / erase (drken式):
//        - del と add が逆操作である場合: on_add, on_erase を指定してマージ前後の区間を処理
//        - 新規追加/削除部分のみ処理したい場合: insert_strictly_new, erase_strictly_removed
//    - 隣接区間の自動結合 (merge_adjacent): true なら [1, 3] と [4, 6] を [1, 6] に統合
//
// 2. IntervalMap<T, V>: (Chtholly Tree / Old Driver Tree / 区間カラーリング)
//    - 区間 [l, r] に値（色や数値）を割り当てて管理
//    - assign(l, r, val): 区間 [l, r] の値を val に一括上書き (ならし O(log N))
//    - get(x): 点 x の値を取得
//    - for_each(l, r, fn): 区間 [l, r] 内の各連続成分に対して処理を実行
//
// 【計算量】
// - insert / erase / assign: ならし O(log N) (N は区間の個数)
// - contains / intersects / mex / get: O(log N)
// - covered_length / size / empty: O(1)
// ============================================================================

template <typename T = long long>
class IntervalSet {
public:
  using Interval = std::pair<T, T>;

private:
  std::set<Interval> st;
  T total_len = 0;
  bool merge_adj = true;

  static constexpr T INF = std::numeric_limits<T>::max();

public:
  // コンストラクタ
  // merge_adjacent: 隣接する区間 (例: [1, 2] と [3, 4]) をマージするかどうか (デフォルト: true)
  explicit IntervalSet(bool merge_adjacent = true) : merge_adj(merge_adjacent) {}

  // 状態取得
  bool empty() const { return st.empty(); }
  size_t size() const { return st.size(); }
  T covered_length() const { return total_len; }
  void clear() {
    st.clear();
    total_len = 0;
  }

  // イテレータ (区間 [l, r] の昇順)
  auto begin() const { return st.begin(); }
  auto end() const { return st.end(); }
  auto rbegin() const { return st.rbegin(); }
  auto rend() const { return st.rend(); }

  // 全区間のリストを取得
  std::vector<Interval> get_intervals() const {
    return std::vector<Interval>(st.begin(), st.end());
  }

  // --------------------------------------------------------------------------
  // 基本操作: insert (区間の追加)
  // --------------------------------------------------------------------------
  // [l, r] を追加し、交差または隣接する区間をマージする。
  // 戻り値: pair<最終的なマージ後区間 [L, R], 新たに被覆された要素数 (差分)>
  std::pair<Interval, T> insert(T l, T r) {
    if (l > r) return {{l, r}, 0};

    auto it = st.upper_bound({l, INF});
    if (it != st.begin()) {
      auto pit = std::prev(it);
      if (pit->second + (merge_adj ? 1 : 0) >= l) {
        it = pit;
      }
    }

    T nl = l, nr = r;
    T removed_len = 0;

    while (it != st.end() && it->first <= r + (merge_adj ? 1 : 0)) {
      nl = std::min(nl, it->first);
      nr = std::max(nr, it->second);
      removed_len += (it->second - it->first + 1);
      it = st.erase(it);
    }

    st.insert({nl, nr});
    T added_len = (nr - nl + 1) - removed_len;
    total_len += added_len;
    return {{nl, nr}, added_len};
  }

  // 単一の点 x を追加
  std::pair<Interval, T> insert(T x) {
    return insert(x, x);
  }

  // --------------------------------------------------------------------------
  // 基本操作: erase (区間の削除)
  // --------------------------------------------------------------------------
  // [l, r] を集合から削除する（被覆部分を削り、必要に応じて区間を分割）。
  // 戻り値: 実際に削除された要素数
  T erase(T l, T r) {
    if (l > r || st.empty()) return 0;

    auto it = st.upper_bound({l, INF});
    if (it != st.begin()) {
      auto pit = std::prev(it);
      if (pit->second >= l) {
        it = pit;
      }
    }

    T prev_total = total_len;
    std::vector<Interval> to_readd;

    while (it != st.end() && it->first <= r) {
      T il = it->first;
      T ir = it->second;
      total_len -= (ir - il + 1);
      it = st.erase(it);

      if (il < l) {
        to_readd.push_back({il, l - 1});
      }
      if (ir > r) {
        to_readd.push_back({r + 1, ir});
      }
    }

    for (const auto &iv : to_readd) {
      st.insert(iv);
      total_len += (iv.second - iv.first + 1);
    }

    return prev_total - total_len;
  }

  // 単一の点 x を削除
  T erase(T x) {
    return erase(x, x);
  }

  // --------------------------------------------------------------------------
  // 判定・クエリ操作
  // --------------------------------------------------------------------------
  // 点 x がいずれかの区間に含まれているか
  bool contains(T x) const {
    auto it = st.upper_bound({x, INF});
    if (it == st.begin()) return false;
    --it;
    return it->first <= x && x <= it->second;
  }

  // 区間 [l, r] 全体が単一の区間に完全に含まれているか
  bool contains(T l, T r) const {
    if (l > r) return true;
    auto it = st.upper_bound({l, INF});
    if (it == st.begin()) return false;
    --it;
    return it->first <= l && r <= it->second;
  }

  // 区間 [l, r] が既存のいずれかの区間と一部でも交差しているか
  bool intersects(T l, T r) const {
    if (l > r || st.empty()) return false;
    auto it = st.upper_bound({l, INF});
    if (it != st.begin()) {
      auto pit = std::prev(it);
      if (pit->second >= l) return true;
    }
    return (it != st.end() && it->first <= r);
  }

  // 点 x を含む区間 [l, r] を取得 (含まれていなければ nullopt)
  std::optional<Interval> get(T x) const {
    auto it = st.upper_bound({x, INF});
    if (it == st.begin()) return std::nullopt;
    --it;
    if (it->first <= x && x <= it->second) return *it;
    return std::nullopt;
  }

  // 点 x 以上の最小の「被覆されていない」整数を取得 (mex)
  // 【用途】ABC228D (Linear Probing), ゲーム木の mex 遷移, 重複なし採番など
  T mex(T x = 0) const {
    auto it = st.upper_bound({x, INF});
    if (it == st.begin()) return x;
    --it;
    if (it->first <= x && x <= it->second) {
      return it->second + 1;
    }
    return x;
  }

  // 点 x 以下の最大の「被覆されていない」整数を取得
  T prev_absent(T x) const {
    auto it = st.upper_bound({x, INF});
    if (it == st.begin()) return x;
    --it;
    if (it->first <= x && x <= it->second) {
      return it->first - 1;
    }
    return x;
  }

  // --------------------------------------------------------------------------
  // コールバック対応 insert / erase (drken 式テクニック 1)
  // --------------------------------------------------------------------------
  // 区間の追加と削除が互いに可逆（逆操作可能）な場合に、
  // 削除された各区間に対して on_erase(l, r)、
  // 最終的に追加された区間に on_add(L, R) を呼び出します。
  template <typename OnAdd, typename OnErase>
  Interval insert(T l, T r, OnAdd on_add, OnErase on_erase) {
    if (l > r) return {l, r};

    auto it = st.upper_bound({l, INF});
    if (it != st.begin()) {
      auto pit = std::prev(it);
      if (pit->second + (merge_adj ? 1 : 0) >= l) {
        it = pit;
      }
    }

    T nl = l, nr = r;
    T removed_len = 0;

    while (it != st.end() && it->first <= r + (merge_adj ? 1 : 0)) {
      nl = std::min(nl, it->first);
      nr = std::max(nr, it->second);
      removed_len += (it->second - it->first + 1);
      on_erase(it->first, it->second);
      it = st.erase(it);
    }

    st.insert({nl, nr});
    total_len += ((nr - nl + 1) - removed_len);
    on_add(nl, nr);
    return {nl, nr};
  }

  template <typename OnAdd, typename OnErase>
  void erase(T l, T r, OnAdd on_add, OnErase on_erase) {
    if (l > r || st.empty()) return;

    auto it = st.upper_bound({l, INF});
    if (it != st.begin()) {
      auto pit = std::prev(it);
      if (pit->second >= l) {
        it = pit;
      }
    }

    std::vector<Interval> to_readd;

    while (it != st.end() && it->first <= r) {
      T il = it->first;
      T ir = it->second;
      total_len -= (ir - il + 1);
      on_erase(il, ir);
      it = st.erase(it);

      if (il < l) {
        to_readd.push_back({il, l - 1});
      }
      if (ir > r) {
        to_readd.push_back({r + 1, ir});
      }
    }

    for (const auto &iv : to_readd) {
      st.insert(iv);
      total_len += (iv.second - iv.first + 1);
      on_add(iv.first, iv.second);
    }
  }

  // --------------------------------------------------------------------------
  // 新規追加 / 削除差分コールバック (drken 式テクニック 2)
  // --------------------------------------------------------------------------
  // 「純粋に新しく被覆された区間」のみに対して on_new_interval(sub_l, sub_r) を実行
  template <typename OnNew>
  Interval insert_strictly_new(T l, T r, OnNew on_new_interval) {
    if (l > r) return {l, r};

    // [l, r] 内で未被覆だった部分を探索
    T cur = l;
    while (cur <= r) {
      auto it = st.upper_bound({cur, INF});
      if (it != st.begin()) {
        auto pit = std::prev(it);
        if (pit->second >= cur) {
          cur = pit->second + 1;
          continue;
        }
      }
      // cur は未被覆
      T nxt = r;
      if (it != st.end()) {
        nxt = std::min(r, it->first - 1);
      }
      if (cur <= nxt) {
        on_new_interval(cur, nxt);
        cur = nxt + 1;
      }
    }

    // 集合へのマージ追加
    return insert(l, r).first;
  }

  // 「純粋に削除された区間」のみに対して on_removed_interval(sub_l, sub_r) を実行
  template <typename OnRemoved>
  void erase_strictly_removed(T l, T r, OnRemoved on_removed_interval) {
    if (l > r || st.empty()) return;

    auto it = st.upper_bound({l, INF});
    if (it != st.begin()) {
      auto pit = std::prev(it);
      if (pit->second >= l) {
        it = pit;
      }
    }

    while (it != st.end() && it->first <= r) {
      T subl = std::max(l, it->first);
      T subr = std::min(r, it->second);
      if (subl <= subr) {
        on_removed_interval(subl, subr);
      }
      ++it;
    }

    erase(l, r);
  }
};

// ============================================================================
// IntervalMap (Chtholly Tree / Old Driver Tree)
// ============================================================================
// 区間 [l, r] に任意の値（色や数値）を割り当てて管理するデータ構造
template <typename T = long long, typename V = long long>
class IntervalMap {
public:
  struct Node {
    T l, r;
    mutable V val;
    bool operator<(const Node &rhs) const {
      return l < rhs.l;
    }
  };

  using SetIterator = typename std::set<Node>::iterator;

private:
  std::set<Node> st;

public:
  IntervalMap() {}

  // 初期区間 [l, r] を初期値 val で構築
  IntervalMap(T l, T r, V val) {
    st.insert({l, r, val});
  }

  bool empty() const { return st.empty(); }
  size_t size() const { return st.size(); }
  void clear() { st.clear(); }

  auto begin() const { return st.begin(); }
  auto end() const { return st.end(); }

  // 点 x で区間を分割し、[l, x-1] と [x, r] に切り分ける
  // 戻り値: [x, r] を指すイテレータ
  SetIterator split(T x) {
    auto it = st.upper_bound({x, 0, V()});
    if (it != st.begin()) {
      --it;
      if (it->l <= x && x <= it->r) {
        if (it->l == x) return it;
        T l = it->l, r = it->r;
        V v = it->val;
        st.erase(it);
        st.insert({l, x - 1, v});
        return st.insert({x, r, v}).first;
      }
    }
    return st.end();
  }

  // 区間 [l, r] の値を一括して val に更新する (ならし O(log N))
  void assign(T l, T r, V val) {
    if (l > r) return;
    auto itr = split(r + 1);
    auto itl = split(l);
    st.erase(itl, itr);
    st.insert({l, r, val});
  }

  // 点 x の値を取得
  V get(T x) const {
    auto it = st.upper_bound({x, 0, V()});
    assert(it != st.begin());
    --it;
    assert(it->l <= x && x <= it->r);
    return it->val;
  }

  // 区間 [l, r] に含まれる各連続部分区間に対して fn(sub_l, sub_r, val) を実行
  template <typename Func>
  void for_each(T l, T r, Func fn) {
    if (l > r) return;
    auto itr = split(r + 1);
    auto itl = split(l);
    for (auto it = itl; it != itr; ++it) {
      fn(it->l, it->r, it->val);
    }
  }
};

using IntervalSetLL = IntervalSet<long long>;
using IntervalMapLL = IntervalMap<long long, long long>;

```

</details>

#### MultiDimensionalCumulativeSum (3次元以上の多次元累積和・多次元いもす法)
3次元以上の高次元空間（$D \ge 3$）に対する**累積和（Prefix Sum）**および**いもす法（Imos Method）**の総合ライブラリです。
2次元以下の累積和・いもす法と異なり、高次元では手動の多重ループや包除原理の符号管理が極めて複雑でバグの温床となるため、任意の $D \ge 3$ 次元に対して軸ごとの走査構築と $O(2^D)$ 包除原理クエリを安全かつ高速に実行できます。

- **ファイル**: `mylib/MultiDimensionalCumulativeSum.hpp` (または `mylib/CumulativeSum.hpp`, `mylib/PrefixSum.hpp`)
- **提供クラス**:
  1. **静的一点加算・超直方体区間和取得 (Prefix Sum)**:
     - `MultiDimensionalPrefixSum<T, D = 3>`: テンプレート固定次元 $D$（$D \ge 3$）
     - `DynamicPrefixSumND<T>`: 実行時に動的に次元数を指定できるクラス（$D \ge 3$）
     - `PrefixSum3D<T>`: 座標配列を作らず `(x, y, z)` で直感的に呼べる 3次元専用ラッパー
  2. **静的区間更新・一点取得 (Imos Method)**:
     - `MultiDimensionalImos<T, D = 3>`: 超直方体への一括加算 $\to$ `build()` $\to$ 各マス最終値取得（$D \ge 3$）
     - `DynamicImosND<T>`: 実行時次元指定版（$D \ge 3$）
     - `Imos3D<T>`: 3次元専用いもす法ラッパー
- **計算量**:
  - **多次元累積和**: 構築 $O(D \cdot \prod N_k)$、超直方体区間和クエリ $O(2^D)$、1点取得 $O(1)$
  - **多次元いもす法**: 超直方体加算 $O(2^D)$、構築 $O(D \cdot \prod N_k)$、1点取得 $O(1)$

```cpp
#include "mylib/MultiDimensionalCumulativeSum.hpp"

// -------------------------------------------------------------
// 1. 3次元累積和 (PrefixSum3D) - 直方体領域の区間和
// -------------------------------------------------------------
PrefixSum3D<long long> ps3(3, 4, 5);
ps3.add(1, 2, 3, 100);
ps3.build(); // 各軸ごとに O(XYZ) で一括累積
assert(ps3.sum(0, 0, 0, 3, 4, 5) == 100);
assert(ps3.sum(1, 2, 3, 2, 3, 4) == 100);
assert(ps3.get(1, 2, 3) == 100);

// -------------------------------------------------------------
// 2. 3次元いもす法 (Imos3D) - 直方体領域への一括加算
// -------------------------------------------------------------
Imos3D<long long> imos3(4, 4, 4);
imos3.add(1, 1, 1, 3, 3, 3, 42); // [1, 3) x [1, 3) x [1, 3) に +42
imos3.build();
assert(imos3.get(1, 1, 1) == 42);
assert(imos3.get(2, 2, 2) == 42);
assert(imos3.get(0, 0, 0) == 0);

// -------------------------------------------------------------
// 3. 4次元以上の超空間累積和 (MultiDimensionalPrefixSum)
// -------------------------------------------------------------
MultiDimensionalPrefixSum<long long, 4> ps4({2, 3, 2, 3});
ps4.add({0, 1, 0, 1}, 55);
ps4.build();
assert(ps4.sum({0, 0, 0, 0}, {2, 3, 2, 3}) == 55);
assert(ps4.sum({0, 1, 0, 1}, {1, 2, 1, 2}) == 55);

// -------------------------------------------------------------
// 4. 実行時任意多次元いもす法 (DynamicImosND)
// -------------------------------------------------------------
DynamicImosND<long long> dimos({3, 3, 3});
dimos.add({1, 1, 1}, {3, 3, 3}, 99);
dimos.build();
assert(dimos.get({1, 1, 1}) == 99);
```

<details>
<summary><b>ソースコードを展開 (MultiDimensionalCumulativeSum.hpp)</b></summary>

```cpp
#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

// ============================================================================
// 多次元累積和 & 多次元いもす法 (3次元以上 / Multi-Dimensional Prefix Sum & Imos)
// ============================================================================
// 【概要】
// 3次元以上の空間（D >= 3）に対する累積和（Prefix Sum）およびいもす法（Imos Method）
// を提供します。
//
// 【提供クラス】
// 1. 静的一点加算・超直方体区間和取得 (Prefix Sum):
//    - MultiDimensionalPrefixSum<T, D = 3>: テンプレート次元 D (D >= 3)
//      各軸ごとの走査構築 O(D * prod N_k) -> 包除原理による超直方体区間和取得 O(2^D)
//    - DynamicPrefixSumND<T>: 実行時に次元数を指定できる動的次元版 (D >= 3)
//    - PrefixSum3D<T>: 座標配列を作らず (x, y, z) で直接呼べる 3次元専用ラッパー
//
// 2. 静的区間更新・一点取得 (Imos Method):
//    - MultiDimensionalImos<T, D = 3>: テンプレート次元 D (D >= 3)
//      超直方体加算 O(2^D) -> 各軸ごとの走査構築 O(D * prod N_k) -> 各マス最終値取得 O(1)
//    - DynamicImosND<T>: 実行時次元指定版 (D >= 3)
//    - Imos3D<T>: 座標配列を作らず (x, y, z) で直接呼べる 3次元専用ラッパー
//
// ※すべての区間は 0-indexed 半開区間 [l, r) です。
//   超直方体: [l_0, r_0) x [l_1, r_1) x ... x [l_{D-1}, r_{D-1})
// ============================================================================

// ----------------------------------------------------------------------------
// 1. MultiDimensionalPrefixSum<T, D> (静的一点加算・超直方体区間和取得, D >= 3)
// ----------------------------------------------------------------------------
template <typename T = long long, size_t D = 3>
class MultiDimensionalPrefixSum {
  static_assert(D >= 3, "MultiDimensionalPrefixSum is intended for D >= 3.");

public:
  using Point = std::array<int, D>;

private:
  Point shape{};
  std::array<int, D + 1> stride{};
  std::vector<T> data;
  bool built = false;

  int flat_idx(const Point &p) const {
    int idx = 0;
    for (size_t k = 0; k < D; ++k) {
      idx += p[k] * stride[k + 1];
    }
    return idx;
  }

public:
  MultiDimensionalPrefixSum() {}

  explicit MultiDimensionalPrefixSum(const Point &shp) : shape(shp) {
    stride[D] = 1;
    for (int k = (int)D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    data.assign(stride[0], T(0));
  }

  MultiDimensionalPrefixSum(std::initializer_list<int> shp_list) {
    assert(shp_list.size() == D);
    size_t idx = 0;
    for (int s : shp_list) shape[idx++] = s;
    stride[D] = 1;
    for (int k = (int)D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    data.assign(stride[0], T(0));
  }

  // 点 p の値を val に設定 (build 前)
  void set(const Point &p, T val) {
    assert(!built);
    Point shifted;
    for (size_t k = 0; k < D; ++k) {
      assert(0 <= p[k] && p[k] < shape[k]);
      shifted[k] = p[k] + 1;
    }
    data[flat_idx(shifted)] = val;
  }

  // 点 p の値に val を加算 (build 前)
  void add(const Point &p, T val) {
    assert(!built);
    Point shifted;
    for (size_t k = 0; k < D; ++k) {
      assert(0 <= p[k] && p[k] < shape[k]);
      shifted[k] = p[k] + 1;
    }
    data[flat_idx(shifted)] += val;
  }

  // 各軸ごとに累積和を走査計算 (O(D * prod N_k))
  void build() {
    assert(!built);
    built = true;
    for (size_t dim = 0; dim < D; ++dim) {
      int step = stride[dim + 1];
      int outer_count = stride[0] / stride[dim];
      int dim_len = shape[dim] + 1;
      for (int outer = 0; outer < outer_count; ++outer) {
        int base = outer * stride[dim];
        for (int coord = 1; coord < dim_len; ++coord) {
          int src = base + (coord - 1) * step;
          int dst = base + coord * step;
          for (int inner = 0; inner < step; ++inner) {
            data[dst + inner] += data[src + inner];
          }
        }
      }
    }
  }

  // 原点から点 r までの半開区間 [0, r_0) x ... x [0, r_{D-1}) の総和 (O(1))
  T prefix_sum(const Point &r) const {
    assert(built);
    Point shifted;
    for (size_t k = 0; k < D; ++k) {
      assert(0 <= r[k] && r[k] <= shape[k]);
      shifted[k] = r[k];
    }
    return data[flat_idx(shifted)];
  }

  // 超直方体 [l_0, r_0) x ... x [l_{D-1}, r_{D-1}) の区間和 (包除原理により O(2^D))
  T sum(const Point &l, const Point &r) const {
    assert(built);
    for (size_t k = 0; k < D; ++k) {
      if (l[k] >= r[k]) return T(0);
      assert(0 <= l[k] && r[k] <= shape[k]);
    }
    T res = 0;
    for (int mask = 0; mask < (1 << D); ++mask) {
      Point coords;
      int popcount = 0;
      for (size_t k = 0; k < D; ++k) {
        if (mask & (1 << k)) {
          coords[k] = l[k];
          popcount++;
        } else {
          coords[k] = r[k];
        }
      }
      T val = data[flat_idx(coords)];
      if (popcount & 1) res -= val;
      else res += val;
    }
    return res;
  }

  // 点 p の元の値を取得 (O(2^D))
  T get(const Point &p) const {
    Point r = p;
    for (size_t k = 0; k < D; ++k) r[k]++;
    return sum(p, r);
  }

  // 全領域の総和 (O(1))
  T all_sum() const {
    return prefix_sum(shape);
  }

  const Point &get_shape() const { return shape; }
};

// ----------------------------------------------------------------------------
// 2. MultiDimensionalImos<T, D> (静的区間更新・一点取得, D >= 3)
// ----------------------------------------------------------------------------
template <typename T = long long, size_t D = 3>
class MultiDimensionalImos {
  static_assert(D >= 3, "MultiDimensionalImos is intended for D >= 3.");

public:
  using Point = std::array<int, D>;

private:
  Point shape{};
  std::array<int, D + 1> stride{};
  std::vector<T> diff;
  bool built = false;

  int flat_idx(const Point &p) const {
    int idx = 0;
    for (size_t k = 0; k < D; ++k) {
      idx += p[k] * stride[k + 1];
    }
    return idx;
  }

public:
  MultiDimensionalImos() {}

  explicit MultiDimensionalImos(const Point &shp) : shape(shp) {
    stride[D] = 1;
    for (int k = (int)D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    diff.assign(stride[0], T(0));
  }

  MultiDimensionalImos(std::initializer_list<int> shp_list) {
    assert(shp_list.size() == D);
    size_t idx = 0;
    for (int s : shp_list) shape[idx++] = s;
    stride[D] = 1;
    for (int k = (int)D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    diff.assign(stride[0], T(0));
  }

  // 超直方体 [l_0, r_0) x ... x [l_{D-1}, r_{D-1}) に val を加算 (O(2^D))
  void add(const Point &l, const Point &r, T val) {
    assert(!built);
    for (size_t k = 0; k < D; ++k) {
      if (l[k] >= r[k]) return;
      assert(0 <= l[k] && r[k] <= shape[k]);
    }
    for (int mask = 0; mask < (1 << D); ++mask) {
      Point coords;
      int popcount = 0;
      for (size_t k = 0; k < D; ++k) {
        if (mask & (1 << k)) {
          coords[k] = r[k];
          popcount++;
        } else {
          coords[k] = l[k];
        }
      }
      T delta = (popcount & 1) ? -val : val;
      diff[flat_idx(coords)] += delta;
    }
  }

  // 各軸ごとに累積和を走査計算して各点の最終値を確定 (O(D * prod N_k))
  void build() {
    assert(!built);
    built = true;
    for (size_t dim = 0; dim < D; ++dim) {
      int step = stride[dim + 1];
      int outer_count = stride[0] / stride[dim];
      int dim_len = shape[dim] + 1;
      for (int outer = 0; outer < outer_count; ++outer) {
        int base = outer * stride[dim];
        for (int coord = 1; coord < dim_len; ++coord) {
          int src = base + (coord - 1) * step;
          int dst = base + coord * step;
          for (int inner = 0; inner < step; ++inner) {
            diff[dst + inner] += diff[src + inner];
          }
        }
      }
    }
  }

  // 点 p の最終的な加算結果を取得 (O(1))
  T get(const Point &p) const {
    assert(built);
    for (size_t k = 0; k < D; ++k) {
      assert(0 <= p[k] && p[k] < shape[k]);
    }
    return diff[flat_idx(p)];
  }

  const Point &get_shape() const { return shape; }
};

// ----------------------------------------------------------------------------
// 3. DynamicPrefixSumND<T> & DynamicImosND<T> (実行時任意多次元版, D >= 3)
// ----------------------------------------------------------------------------
template <typename T = long long>
class DynamicPrefixSumND {
private:
  int D = 0;
  std::vector<int> shape;
  std::vector<int> stride;
  std::vector<T> data;
  bool built = false;

  int flat_idx(const std::vector<int> &p) const {
    int idx = 0;
    for (int k = 0; k < D; ++k) {
      idx += p[k] * stride[k + 1];
    }
    return idx;
  }

public:
  DynamicPrefixSumND() {}
  explicit DynamicPrefixSumND(const std::vector<int> &shp) : shape(shp) {
    D = (int)shape.size();
    assert(D >= 3 && "DynamicPrefixSumND is intended for D >= 3.");
    stride.assign(D + 1, 1);
    for (int k = D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    data.assign(stride[0], T(0));
  }

  void add(const std::vector<int> &p, T val) {
    assert(!built);
    assert((int)p.size() == D);
    std::vector<int> shifted(D);
    for (int k = 0; k < D; ++k) {
      assert(0 <= p[k] && p[k] < shape[k]);
      shifted[k] = p[k] + 1;
    }
    data[flat_idx(shifted)] += val;
  }

  void build() {
    assert(!built);
    built = true;
    for (int dim = 0; dim < D; ++dim) {
      int step = stride[dim + 1];
      int outer_count = stride[0] / stride[dim];
      int dim_len = shape[dim] + 1;
      for (int outer = 0; outer < outer_count; ++outer) {
        int base = outer * stride[dim];
        for (int coord = 1; coord < dim_len; ++coord) {
          int src = base + (coord - 1) * step;
          int dst = base + coord * step;
          for (int inner = 0; inner < step; ++inner) {
            data[dst + inner] += data[src + inner];
          }
        }
      }
    }
  }

  T sum(const std::vector<int> &l, const std::vector<int> &r) const {
    assert(built);
    assert((int)l.size() == D && (int)r.size() == D);
    for (int k = 0; k < D; ++k) {
      if (l[k] >= r[k]) return T(0);
      assert(0 <= l[k] && r[k] <= shape[k]);
    }
    T res = 0;
    for (int mask = 0; mask < (1 << D); ++mask) {
      std::vector<int> coords(D);
      int popcount = 0;
      for (int k = 0; k < D; ++k) {
        if (mask & (1 << k)) {
          coords[k] = l[k];
          popcount++;
        } else {
          coords[k] = r[k];
        }
      }
      T val = data[flat_idx(coords)];
      if (popcount & 1) res -= val;
      else res += val;
    }
    return res;
  }
};

template <typename T = long long>
class DynamicImosND {
private:
  int D = 0;
  std::vector<int> shape;
  std::vector<int> stride;
  std::vector<T> diff;
  bool built = false;

  int flat_idx(const std::vector<int> &p) const {
    int idx = 0;
    for (int k = 0; k < D; ++k) {
      idx += p[k] * stride[k + 1];
    }
    return idx;
  }

public:
  DynamicImosND() {}
  explicit DynamicImosND(const std::vector<int> &shp) : shape(shp) {
    D = (int)shape.size();
    assert(D >= 3 && "DynamicImosND is intended for D >= 3.");
    stride.assign(D + 1, 1);
    for (int k = D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    diff.assign(stride[0], T(0));
  }

  void add(const std::vector<int> &l, const std::vector<int> &r, T val) {
    assert(!built);
    assert((int)l.size() == D && (int)r.size() == D);
    for (int k = 0; k < D; ++k) {
      if (l[k] >= r[k]) return;
      assert(0 <= l[k] && r[k] <= shape[k]);
    }
    for (int mask = 0; mask < (1 << D); ++mask) {
      std::vector<int> coords(D);
      int popcount = 0;
      for (int k = 0; k < D; ++k) {
        if (mask & (1 << k)) {
          coords[k] = r[k];
          popcount++;
        } else {
          coords[k] = l[k];
        }
      }
      T delta = (popcount & 1) ? -val : val;
      diff[flat_idx(coords)] += delta;
    }
  }

  void build() {
    assert(!built);
    built = true;
    for (int dim = 0; dim < D; ++dim) {
      int step = stride[dim + 1];
      int outer_count = stride[0] / stride[dim];
      int dim_len = shape[dim] + 1;
      for (int outer = 0; outer < outer_count; ++outer) {
        int base = outer * stride[dim];
        for (int coord = 1; coord < dim_len; ++coord) {
          int src = base + (coord - 1) * step;
          int dst = base + coord * step;
          for (int inner = 0; inner < step; ++inner) {
            diff[dst + inner] += diff[src + inner];
          }
        }
      }
    }
  }

  T get(const std::vector<int> &p) const {
    assert(built);
    assert((int)p.size() == D);
    for (int k = 0; k < D; ++k) {
      assert(0 <= p[k] && p[k] < shape[k]);
    }
    return diff[flat_idx(p)];
  }
};

// ============================================================================
// 4. 3次元コンビニエンスラッパー (3D Wrappers)
// ============================================================================
template <typename T = long long>
class PrefixSum3D {
private:
  MultiDimensionalPrefixSum<T, 3> ps;
  int X = 0, Y = 0, Z = 0;

public:
  PrefixSum3D() {}
  PrefixSum3D(int x, int y, int z) : ps({x, y, z}), X(x), Y(y), Z(z) {}

  void set(int x, int y, int z, T val) { ps.set({x, y, z}, val); }
  void add(int x, int y, int z, T val) { ps.add({x, y, z}, val); }
  void build() { ps.build(); }

  // 直方体 [x1, x2) x [y1, y2) x [z1, z2) の総和 (O(1))
  T sum(int x1, int y1, int z1, int x2, int y2, int z2) const {
    return ps.sum({x1, y1, z1}, {x2, y2, z2});
  }

  // プレフィックス [0, x) x [0, y) x [0, z) の総和 (O(1))
  T prefix_sum(int x, int y, int z) const {
    return ps.prefix_sum({x, y, z});
  }

  // 点 (x, y, z) の値を取得 (O(1))
  T get(int x, int y, int z) const {
    return ps.get({x, y, z});
  }

  T all_sum() const { return ps.all_sum(); }
  int size_x() const { return X; }
  int size_y() const { return Y; }
  int size_z() const { return Z; }
};

template <typename T = long long>
class Imos3D {
private:
  MultiDimensionalImos<T, 3> imos;
  int X = 0, Y = 0, Z = 0;

public:
  Imos3D() {}
  Imos3D(int x, int y, int z) : imos({x, y, z}), X(x), Y(y), Z(z) {}

  // 直方体 [x1, x2) x [y1, y2) x [z1, z2) に val を加算 (O(1))
  void add(int x1, int y1, int z1, int x2, int y2, int z2, T val) {
    imos.add({x1, y1, z1}, {x2, y2, z2}, val);
  }

  // 累積和を計算して各マスの値を確定 (O(XYZ))
  void build() { imos.build(); }

  // 点 (x, y, z) の値を取得 (O(1))
  T get(int x, int y, int z) const {
    return imos.get({x, y, z});
  }

  int size_x() const { return X; }
  int size_y() const { return Y; }
  int size_z() const { return Z; }
};

// 汎用エイリアス
template <typename T = long long, size_t D = 3>
using PrefixSumND = MultiDimensionalPrefixSum<T, D>;

template <typename T = long long, size_t D = 3>
using ImosND = MultiDimensionalImos<T, D>;

```

</details>
---

### 2. 文字列 (Strings)

#### DoubleRollingHash (安全なダブルローリングハッシュ・点更新対応)
$2^{61}-1$（メルセンヌ素数）を法とし、実行時に安全な乱数基数を生成する耐撃墜性（Anti-Hack）の高いローリングハッシュです。
静的文字列に対する $O(1)$ ハッシュ取得・LCP計算に加え、セグメント木連携による**点更新クエリ ($O(\log N)$)** や**回文判定**、**1文字変更仮定ハッシュ計算 ($O(1)$)** をサポートしています。
- **ファイル**: `mylib/DoubleRollingHash.hpp`
- **計算量**: 
  - 構築: $O(N)$
  - 区間ハッシュ `get(l, r)`: 更新前 $O(1)$、点更新後 $O(\log N)$
  - 点更新 `set(idx, c)` / `update(idx, c)`: $O(\log N)$
  - 1文字変更仮定 `get_with_change(l, r, idx, c)`: $O(1)$
  - 回文判定 `is_palindrome(l, r)` / 反転ハッシュ `get_reversed(l, r)`: $O(\log N)$
  - 最長共通接頭辞 (LCP): $O(\log N)$

```cpp
#include "mylib/DoubleRollingHash.hpp"

string s = "abracadabra";
DoubleRollingHash rh(s); // using RollingHash = DoubleRollingHash; も利用可能

// [l, r) のハッシュ値を取得 (更新前は O(1))
auto h1 = rh.get(0, 4); // "abra"
auto h2 = rh.get(7, 11); // "abra"
assert(h1 == h2);

// 1. 点更新クエリ: 位置 idx の文字を変更 (O(log N))
rh.set(0, 'o'); // "obracadabra"
assert(rh.get(0, 4) != h1);

// 2. 変更仮定クエリ: 実際には変更せず、位置 idx を変更した場合のハッシュ値を O(1) で取得
auto h_hypo = rh.get_with_change(0, 4, 0, 'a'); // 位置 0 を 'a' に戻したと仮定
assert(h_hypo == h1);

// 3. 回文判定クエリ: 区間 [l, r) が回文か判定 (O(log N))
DoubleRollingHash pal("abacaba");
assert(pal.is_palindrome(0, 7)); // true
pal.set(3, 'x'); // "abaxaba"
assert(pal.is_palindrome(0, 7)); // true
pal.set(1, 'z'); // "azaxaba"
assert(!pal.is_palindrome(0, 7)); // false

// 4. LCP (最長共通接頭辞) の長さを二分探索で取得
int len = rh.lcp(rh, 1, 8); // s[1...] と s[8...] の LCP
```

<details>
<summary><b>ソースコードを展開 (DoubleRollingHash.hpp)</b></summary>

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

struct DoubleRollingHash {
  static const uint64_t mod = (1ULL << 61) - 1;
  uint64_t base1, base2;
  vector<uint64_t> hash1, power1;
  vector<uint64_t> hash2, power2;
  vector<uint64_t> origin;
  int n;

  struct SegNode {
    uint64_t h1, h2;
    uint64_t rh1, rh2;
    int len;
  };
  vector<SegNode> seg;
  int seg_sz;
  bool seg_built;

  static inline uint64_t mul(uint64_t a, uint64_t b) {
    __uint128_t t = (__uint128_t)a * b;
    t = (t >> 61) + (t & mod);
    if (t >= mod)
      t -= mod;
    return (uint64_t)t;
  }

  static inline uint64_t generate_base() {
    static mt19937_64 mt(
        chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<uint64_t> rand(2, mod - 2);
    return rand(mt);
  }

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

  DoubleRollingHash()
      : base1(0), base2(0), n(0), seg_sz(0), seg_built(false) {}

  DoubleRollingHash(const string &s) {
    auto [b1, b2] = get_default_bases();
    init(s, b1, b2);
  }

  DoubleRollingHash(const string &s, uint64_t b1, uint64_t b2) {
    init(s, b1, b2);
  }

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
    n = v.size();
    origin.resize(n);
    for (int i = 0; i < n; i++) {
      origin[i] = (uint64_t)v[i] % mod;
    }

    hash1.assign(n + 1, 0);
    power1.assign(n + 1, 1);
    hash2.assign(n + 1, 0);
    power2.assign(n + 1, 1);

    for (int i = 0; i < n; i++) {
      uint64_t val = origin[i];
      hash1[i + 1] = mul(hash1[i], base1) + val;
      if (hash1[i + 1] >= mod)
        hash1[i + 1] -= mod;
      power1[i + 1] = mul(power1[i], base1);

      hash2[i + 1] = mul(hash2[i], base2) + val;
      if (hash2[i + 1] >= mod)
        hash2[i + 1] -= mod;
      power2[i + 1] = mul(power2[i], base2);
    }
    seg_built = false;
  }

  pair<uint64_t, uint64_t> get(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    if (l == r)
      return {0, 0};
    if (!seg_built) {
      uint64_t res1 = hash1[r] + mod - mul(hash1[l], power1[r - l]);
      if (res1 >= mod)
        res1 -= mod;

      uint64_t res2 = hash2[r] + mod - mul(hash2[l], power2[r - l]);
      if (res2 >= mod)
        res2 -= mod;

      return {res1, res2};
    }

    SegNode L_res{0, 0, 0, 0, 0}, R_res{0, 0, 0, 0, 0};
    int L = l + seg_sz, R = r + seg_sz;
    while (L < R) {
      if (L & 1)
        L_res = merge_node(L_res, seg[L++]);
      if (R & 1)
        R_res = merge_node(seg[--R], R_res);
      L >>= 1;
      R >>= 1;
    }
    SegNode total = merge_node(L_res, R_res);
    return {total.h1, total.h2};
  }

  template <typename T> void set(int idx, T val) {
    assert(0 <= idx && idx < n);
    ensure_seg_tree();
    uint64_t v = (uint64_t)val % mod;
    origin[idx] = v;
    int k = seg_sz + idx;
    seg[k] = make_leaf(v);
    k >>= 1;
    while (k > 0) {
      seg[k] = merge_node(seg[2 * k], seg[2 * k + 1]);
      k >>= 1;
    }
  }

  template <typename T> void update(int idx, T val) { set(idx, val); }

  template <typename T>
  pair<uint64_t, uint64_t> get_with_change(int l, int r, int idx,
                                          T new_val) const {
    assert(0 <= l && l <= r && r <= n);
    auto [h1, h2] = get(l, r);
    if (idx < l || idx >= r)
      return {h1, h2};

    uint64_t old_v = origin[idx];
    uint64_t new_v = (uint64_t)new_val % mod;
    int shift = r - 1 - idx;

    uint64_t diff1 = mul((new_v + mod - old_v) % mod, power1[shift]);
    h1 += diff1;
    if (h1 >= mod)
      h1 -= mod;

    uint64_t diff2 = mul((new_v + mod - old_v) % mod, power2[shift]);
    h2 += diff2;
    if (h2 >= mod)
      h2 -= mod;

    return {h1, h2};
  }

  pair<uint64_t, uint64_t> get_reversed(int l, int r) {
    assert(0 <= l && l <= r && r <= n);
    if (l == r)
      return {0, 0};
    ensure_seg_tree();
    SegNode L_res{0, 0, 0, 0, 0}, R_res{0, 0, 0, 0, 0};
    int L = l + seg_sz, R = r + seg_sz;
    while (L < R) {
      if (L & 1)
        L_res = merge_node(L_res, seg[L++]);
      if (R & 1)
        R_res = merge_node(seg[--R], R_res);
      L >>= 1;
      R >>= 1;
    }
    SegNode total = merge_node(L_res, R_res);
    return {total.rh1, total.rh2};
  }

  bool is_palindrome(int l, int r) {
    if (l >= r)
      return true;
    return get(l, r) == get_reversed(l, r);
  }

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

  int lcp(const DoubleRollingHash &other, int l1 = 0, int l2 = 0) const {
    int max_len = min(n - l1, other.n - l2);
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

private:
  SegNode make_leaf(uint64_t val) const {
    val %= mod;
    return {val, val, val, val, 1};
  }

  SegNode merge_node(const SegNode &L, const SegNode &R) const {
    if (L.len == 0)
      return R;
    if (R.len == 0)
      return L;

    uint64_t h1 = mul(L.h1, power1[R.len]) + R.h1;
    if (h1 >= mod)
      h1 -= mod;
    uint64_t h2 = mul(L.h2, power2[R.len]) + R.h2;
    if (h2 >= mod)
      h2 -= mod;

    uint64_t rh1 = mul(R.rh1, power1[L.len]) + L.rh1;
    if (rh1 >= mod)
      rh1 -= mod;
    uint64_t rh2 = mul(R.rh2, power2[L.len]) + L.rh2;
    if (rh2 >= mod)
      rh2 -= mod;

    return {h1, h2, rh1, rh2, L.len + R.len};
  }

  void ensure_seg_tree() {
    if (seg_built)
      return;
    seg_sz = 1;
    while (seg_sz < n)
      seg_sz <<= 1;
    seg.assign(2 * seg_sz, {0, 0, 0, 0, 0});
    for (int i = 0; i < n; i++) {
      seg[seg_sz + i] = make_leaf(origin[i]);
    }
    for (int i = seg_sz - 1; i >= 1; i--) {
      seg[i] = merge_node(seg[2 * i], seg[2 * i + 1]);
    }
    seg_built = true;
  }
};

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

#### EulerTour (オイラーツアー・LCA)
木構造を平坦化し、部分木クエリを区間クエリに変換する構造体です。内蔵の Sparse Table により、LCA（最小共通祖先）や頂点間距離を $O(1)$ で取得可能です。
- **ファイル**: `mylib/EulerTour.hpp`
- **計算量**: 構築 $O(N \log N)$、部分木範囲 $O(1)$、LCA・距離取得 $O(1)$、祖先判定 $O(1)$

```cpp
#include "mylib/EulerTour.hpp"

int n = 5;
EulerTour et(n);
et.add_edge(0, 1);
et.add_edge(0, 2);
et.add_edge(1, 3);
et.add_edge(1, 4);

et.build(0); // 根を 0 として構築

// 部分木 u に対応する半開区間 [l, r)
auto [l, r] = et.subtree_range(1); // BIT やセグ木に乗せて部分木更新・取得

// 部分木のサイズ
int sz = et.subtree_size(1); // 3

// LCA (最小共通祖先)
int anc = et.lca(3, 4); // 1

// 頂点間距離 (辺数)
int d = et.dist(3, 2); // 3

// 祖先判定
bool ok = et.is_ancestor(0, 3); // true
```

<details>
<summary><b>ソースコードを展開 (EulerTour.hpp)</b></summary>

```cpp
#pragma once
#include <bits/stdc++.h>

using namespace std;

struct EulerTour {
  int n;
  vector<vector<int>> g;
  vector<int> in, out;     // 部分木 u に対応する区間 [in[u], out[u]) (サイズ n)
  vector<int> tour;        // 訪問時刻順の頂点列 (サイズ n)
  vector<int> depth;       // 各頂点の深さ (根は 0)
  vector<int> parent;      // 親頂点 (根は -1)

  vector<int> node_visit;  // 頂点訪問履歴 (サイズ 2*n - 1)
  vector<int> first_visit; // 各頂点が node_visit に最初に現れるインデックス
  vector<vector<int>> st;  // Sparse Table

  EulerTour(int n = 0)
      : n(n), g(n), in(n), out(n), tour(n), depth(n, 0), parent(n, -1),
        first_visit(n, 0) {}

  void add_edge(int u, int v) {
    g[u].push_back(v);
    g[v].push_back(u);
  }

  void build(int root = 0) {
    int timer = 0;
    node_visit.clear();
    node_visit.reserve(2 * n);
    dfs(root, -1, 0, timer);
    build_lca();
  }

  pair<int, int> subtree_range(int u) const {
    return {in[u], out[u]};
  }

  int subtree_size(int u) const {
    return out[u] - in[u];
  }

  bool is_ancestor(int u, int v) const {
    return in[u] <= in[v] && out[v] <= out[u];
  }

  int lca(int u, int v) const {
    int l = first_visit[u], r = first_visit[v];
    if (l > r)
      swap(l, r);
    int d = r - l + 1;
    int k = 31 - __builtin_clz(d);
    int idx1 = st[k][l];
    int idx2 = st[k][r - (1 << k) + 1];
    return depth[node_visit[idx1]] < depth[node_visit[idx2]] ? node_visit[idx1]
                                                             : node_visit[idx2];
  }

  int dist(int u, int v) const {
    return depth[u] + depth[v] - 2 * depth[lca(u, v)];
  }

private:
  void dfs(int u, int p, int d, int &timer) {
    parent[u] = p;
    depth[u] = d;
    in[u] = timer;
    tour[timer] = u;
    timer++;

    first_visit[u] = node_visit.size();
    node_visit.push_back(u);

    for (int v : g[u]) {
      if (v == p)
        continue;
      dfs(v, u, d + 1, timer);
      node_visit.push_back(u);
    }
    out[u] = timer;
  }

  void build_lca() {
    int m = node_visit.size();
    if (m == 0)
      return;
    int log_m = 32 - __builtin_clz(m);
    st.assign(log_m, vector<int>(m));
    for (int i = 0; i < m; i++)
      st[0][i] = i;
    for (int k = 1; k < log_m; k++) {
      int span = 1 << (k - 1);
      for (int i = 0; i + (1 << k) <= m; i++) {
        int idx1 = st[k - 1][i];
        int idx2 = st[k - 1][i + span];
        st[k][i] = (depth[node_visit[idx1]] < depth[node_visit[idx2]]) ? idx1
                                                                       : idx2;
      }
    }
  }
};
```

</details>

#### LowestCommonAncestor (LCA・木上の重み付き最短距離・パスJump)
ダブリング（Binary Lifting）により、木の2頂点間の最小共通祖先（LCA）や最短距離（重み付き対応）、$k$ 個親の取得、パス上の判定・探索を高速に行う構造体です。
- **ファイル**: `mylib/LowestCommonAncestor.hpp`（エイリアス: `mylib/LCA.hpp`）
- **計算量**: 構築 $O(N \log N)$、クエリ $O(\log N)$（LCA・距離・jump・パス判定など）

```cpp
#include "mylib/LowestCommonAncestor.hpp" // または "mylib/LCA.hpp"

int n = 5;
LowestCommonAncestor<long long> lca(n); // using LCA = LowestCommonAncestor<long long>; も利用可能

// 辺の追加 (u, v, cost = 1)
lca.add_edge(0, 1, 2);
lca.add_edge(0, 2, 10);
lca.add_edge(1, 3, 4);
lca.add_edge(1, 4, 5);

lca.build(0); // 根を 0 として構築

// LCA (最小共通祖先)
int a = lca.lca(3, 4); // 1

// 2頂点間の重み付き最短距離
long long d = lca.dist(3, 4); // 4 + 5 = 9
long long d2 = lca.dist(3, 2); // 4 + 2 + 10 = 16

// 2頂点間の辺数 (重みなし距離)
int edges = lca.dist_edges(3, 4); // 2

// 根方向に k 個登った頂点 (存在しなければ -1)
int p = lca.jump(3, 1); // 1 (親)
int root = lca.jump(3, 2); // 0 (根)

// パス u-v 上で u から k ステップ進んだ頂点
int mid = lca.jump_on_path(3, 4, 1); // 1

// 頂点 x が パス u-v 上にあるか判定
bool on_path = lca.is_on_path(3, 4, 1); // true
```

<details>
<summary><b>ソースコードを展開 (LowestCommonAncestor.hpp)</b></summary>

```cpp
#pragma once
#include <bits/stdc++.h>

using namespace std;

template <typename T = long long>
struct LowestCommonAncestor {
  struct Edge {
    int to;
    T cost;
  };

  int n;
  int log_n;
  vector<vector<Edge>> g;
  vector<vector<int>> parent;
  vector<int> depth;
  vector<T> dist_from_root;

  LowestCommonAncestor(int n = 0) : n(n), log_n(1) {
    while ((1 << log_n) <= max(1, n)) log_n++;
    g.resize(n);
  }

  void add_edge(int u, int v, T cost = 1) {
    assert(0 <= u && u < n);
    assert(0 <= v && v < n);
    g[u].push_back({v, cost});
    g[v].push_back({u, cost});
  }

  void build(int root = 0) {
    assert(0 <= root && root < n);
    parent.assign(log_n, vector<int>(n, -1));
    depth.assign(n, 0);
    dist_from_root.assign(n, 0);

    auto dfs = [&](auto self, int u, int p, int d, T c) -> void {
      parent[0][u] = p;
      depth[u] = d;
      dist_from_root[u] = c;
      for (const auto &e : g[u]) {
        if (e.to == p) continue;
        self(self, e.to, u, d + 1, c + e.cost);
      }
    };
    dfs(dfs, root, -1, 0, 0);

    for (int k = 0; k + 1 < log_n; k++) {
      for (int v = 0; v < n; v++) {
        if (parent[k][v] < 0) {
          parent[k + 1][v] = -1;
        } else {
          parent[k + 1][v] = parent[k][parent[k][v]];
        }
      }
    }
  }

  int lca(int u, int v) const {
    assert(0 <= u && u < n && 0 <= v && v < n);
    if (depth[u] > depth[v]) swap(u, v);
    for (int k = 0; k < log_n; k++) {
      if ((depth[v] - depth[u]) >> k & 1) {
        v = parent[k][v];
      }
    }
    if (u == v) return u;
    for (int k = log_n - 1; k >= 0; k--) {
      if (parent[k][u] != parent[k][v]) {
        u = parent[k][u];
        v = parent[k][v];
      }
    }
    return parent[0][u];
  }

  T dist(int u, int v) const {
    int a = lca(u, v);
    return dist_from_root[u] + dist_from_root[v] - 2 * dist_from_root[a];
  }

  int dist_edges(int u, int v) const {
    int a = lca(u, v);
    return depth[u] + depth[v] - 2 * depth[a];
  }

  int jump(int u, int k) const {
    assert(0 <= u && u < n);
    if (k < 0 || k > depth[u]) return -1;
    for (int i = 0; i < log_n; i++) {
      if (k >> i & 1) {
        u = parent[i][u];
        if (u < 0) return -1;
      }
    }
    return u;
  }

  int kth_ancestor(int u, int k) const { return jump(u, k); }

  bool is_on_path(int u, int v, int x) const {
    assert(0 <= x && x < n);
    return dist_edges(u, x) + dist_edges(x, v) == dist_edges(u, v);
  }

  int jump_on_path(int u, int v, int k) const {
    int a = lca(u, v);
    int d1 = depth[u] - depth[a];
    int d2 = depth[v] - depth[a];
    if (k < 0 || k > d1 + d2) return -1;
    if (k <= d1) {
      return jump(u, k);
    } else {
      return jump(v, d1 + d2 - k);
    }
  }
};

using LCA = LowestCommonAncestor<long long>;
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

#### Prime (巨大素数判定・素因数分解・線形篩・区間篩)
素数判定・素因数分解・約数列挙・エラトステネス線形篩（osa_k法）・区間篩を網羅した素数特化ライブラリです。
- **ファイル**: `mylib/Prime.hpp`
- **計算量**:
  - 巨大数素数判定 `is_prime(n)`: $O(\log N)$（$N \le 10^{18}$、Miller-Rabin）
  - 高速素因数分解 `prime_factorize(n)`: $O(N^{1/4})$（$N \le 10^{18}$、Pollard's rho）
  - 全約数列挙 `divisors(n)`: 約数個数に応じた計算量
  - 線形篩 `PrimeSieve(MAX_N)`: 構築 $O(MAX_N)$、クエリ素因数分解 $O(\log X)$、素数判定 $O(1)$
  - 区間篩 `segmented_sieve(L, R)`: $O((R - L) + \sqrt{R} \log \log \sqrt{R})$

```cpp
#include "mylib/Prime.hpp"

// 1. 巨大数の素数判定 (Miller-Rabin, 10^18 まで高速)
bool ok = is_prime(1000000000000037LL); // true

// 2. 巨大数の素因数分解 (Pollard's rho)
auto fac = prime_factorize(60LL); // {{2, 2}, {3, 1}, {5, 1}}
auto divs = divisors(60LL);        // {1, 2, 3, 4, 5, 6, 10, 12, 15, 20, 30, 60}
long long d_cnt = count_divisors(60LL); // 12
long long d_sum = sum_divisors(60LL);   // 168
long long phi = euler_phi(60LL);        // 16 (互いに素な正整数の個数)

// 3. 線形篩 / osa_k法 (クエリが多数ある場合に前計算 O(N))
PrimeSieve sieve(1000000);
bool is_p = sieve.is_prime(97); // O(1)
int spf = sieve.min_factor(91); // 7 (最小素因数)
auto f = sieve.prime_factorize(60); // O(log X) -> {{2, 2}, {3, 1}, {5, 1}}
auto div_fast = sieve.divisors(60); // 約数列挙
int mu = sieve.moebius(30);         // -1 (メビウス関数)

// 4. 区間篩 (10^12 以下の区間 [L, R) の素数列挙)
auto range_primes = segmented_sieve(10, 30); // {11, 13, 17, 19, 23, 29}
```

<details>
<summary><b>ソースコードを展開 (Prime.hpp)</b></summary>

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

namespace prime_internal {

inline uint64_t mul_mod(uint64_t a, uint64_t b, uint64_t mod) {
  return (uint64_t)((__uint128_t)a * b % mod);
}

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

inline uint64_t gcd_u64(uint64_t a, uint64_t b) {
  while (b) {
    a %= b;
    swap(a, b);
  }
  return a;
}

inline uint64_t find_factor(uint64_t n);
inline void factorize_rec(uint64_t n, vector<uint64_t> &res);

} // namespace prime_internal

inline bool is_prime(uint64_t n) {
  if (n <= 1) return false;
  if (n == 2 || n == 3 || n == 5 || n == 7) return true;
  if (n % 2 == 0 || n % 3 == 0 || n % 5 == 0 || n % 7 == 0) return false;
  if (n < 121) return true;

  uint64_t d = n - 1;
  int s = 0;
  while ((d & 1) == 0) {
    d >>= 1;
    s++;
  }

  static const uint64_t bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
  for (uint64_t a : bases) {
    if (n <= a) break;
    uint64_t x = prime_internal::pow_mod(a, d, n);
    if (x == 1 || x == n - 1) continue;
    bool composite = true;
    for (int r = 1; r < s; r++) {
      x = prime_internal::mul_mod(x, x, n);
      if (x == n - 1) {
        composite = false;
        break;
      }
    }
    if (composite) return false;
  }
  return true;
}

inline bool is_prime(long long n) {
  if (n <= 1) return false;
  return is_prime((uint64_t)n);
}

namespace prime_internal {

inline uint64_t find_factor(uint64_t n) {
  if (n % 2 == 0) return 2;
  if (is_prime(n)) return n;

  static mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
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
    if (d != n) return d;
  }
}

inline void factorize_rec(uint64_t n, vector<uint64_t> &res) {
  if (n <= 1) return;
  if (is_prime(n)) {
    res.push_back(n);
    return;
  }
  uint64_t factor = find_factor(n);
  factorize_rec(factor, res);
  factorize_rec(n / factor, res);
}

} // namespace prime_internal

inline vector<long long> factorize(long long n) {
  if (n <= 1) return {};
  vector<uint64_t> res_u64;
  prime_internal::factorize_rec((uint64_t)n, res_u64);
  sort(res_u64.begin(), res_u64.end());
  return vector<long long>(res_u64.begin(), res_u64.end());
}

inline vector<pair<long long, int>> prime_factorize(long long n) {
  auto factors = factorize(n);
  vector<pair<long long, int>> res;
  for (long long p : factors) {
    if (res.empty() || res.back().first != p) {
      res.emplace_back(p, 1);
    } else {
      res.back().second++;
    }
  }
  return res;
}

inline vector<long long> divisors(long long n) {
  if (n <= 0) return {};
  auto pairs = prime_factorize(n);
  vector<long long> res = {1};
  for (const auto &[p, exp] : pairs) {
    int sz = res.size();
    long long mult = 1;
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

inline long long count_divisors(long long n) {
  if (n <= 0) return 0;
  auto pairs = prime_factorize(n);
  long long ans = 1;
  for (const auto &[p, exp] : pairs) ans *= (exp + 1);
  return ans;
}

inline long long sum_divisors(long long n) {
  if (n <= 0) return 0;
  auto pairs = prime_factorize(n);
  long long ans = 1;
  for (const auto &[p, exp] : pairs) {
    long long cur = 1, mult = 1;
    for (int i = 0; i < exp; i++) {
      mult *= p;
      cur += mult;
    }
    ans *= cur;
  }
  return ans;
}

inline long long euler_phi(long long n) {
  if (n <= 0) return 0;
  auto pairs = prime_factorize(n);
  long long ans = n;
  for (const auto &[p, exp] : pairs) ans -= ans / p;
  return ans;
}

struct PrimeSieve {
  int n;
  vector<int> min_factor_table;
  vector<int> primes;

  PrimeSieve() : n(0) {}
  explicit PrimeSieve(int n) : n(n), min_factor_table(n + 1, 0) {
    for (int i = 2; i <= n; i++) {
      if (min_factor_table[i] == 0) {
        min_factor_table[i] = i;
        primes.push_back(i);
      }
      for (int p : primes) {
        if (p > min_factor_table[i] || (long long)i * p > n) break;
        min_factor_table[i * p] = p;
      }
    }
  }

  bool is_prime(int x) const {
    assert(1 <= x && x <= n);
    return min_factor_table[x] == x;
  }

  int min_factor(int x) const {
    assert(1 <= x && x <= n);
    return (x == 1 ? 1 : min_factor_table[x]);
  }

  vector<int> factorize(int x) const {
    assert(1 <= x && x <= n);
    vector<int> res;
    while (x > 1) {
      res.push_back(min_factor_table[x]);
      x /= min_factor_table[x];
    }
    return res;
  }

  vector<pair<int, int>> prime_factorize(int x) const {
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

  vector<int> divisors(int x) const {
    assert(1 <= x && x <= n);
    auto pairs = prime_factorize(x);
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

  int count_divisors(int x) const {
    auto pairs = prime_factorize(x);
    int ans = 1;
    for (const auto &[p, exp] : pairs) ans *= (exp + 1);
    return ans;
  }

  long long sum_divisors(int x) const {
    auto pairs = prime_factorize(x);
    long long ans = 1;
    for (const auto &[p, exp] : pairs) {
      long long cur = 1, mult = 1;
      for (int i = 0; i < exp; i++) {
        mult *= p;
        cur += mult;
      }
      ans *= cur;
    }
    return ans;
  }

  int euler_phi(int x) const {
    assert(1 <= x && x <= n);
    auto pairs = prime_factorize(x);
    int ans = x;
    for (const auto &[p, exp] : pairs) ans -= ans / p;
    return ans;
  }

  int moebius(int x) const {
    assert(1 <= x && x <= n);
    int mu = 1;
    while (x > 1) {
      int p = min_factor_table[x];
      x /= p;
      if (x % p == 0) return 0;
      mu = -mu;
    }
    return mu;
  }
};

inline vector<long long> segmented_sieve(long long L, long long R) {
  if (L < 2) L = 2;
  if (L >= R) return {};

  long long sqrt_R = 1;
  while ((sqrt_R + 1) * (sqrt_R + 1) < R) sqrt_R++;

  vector<bool> is_prime_small(sqrt_R + 1, true);
  vector<long long> small_primes;
  for (long long i = 2; i <= sqrt_R; i++) {
    if (is_prime_small[i]) {
      small_primes.push_back(i);
      for (long long j = i * i; j <= sqrt_R; j += i) {
        is_prime_small[j] = false;
      }
    }
  }

  vector<bool> is_prime_range(R - L, true);
  for (long long p : small_primes) {
    long long start = ((L + p - 1) / p) * p;
    if (start < p * p) start = p * p;
    for (long long j = start; j < R; j += p) {
      is_prime_range[j - L] = false;
    }
  }

  vector<long long> primes;
  for (long long i = 0; i < R - L; i++) {
    if (is_prime_range[i]) primes.push_back(L + i);
  }
  return primes;
}
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
#include "BIT2D.hpp"
#include "CoordinateCompression.hpp"
#include "Dijkstra.hpp"
#include "DisjointSparseTable.hpp"
#include "DoubleRollingHash.hpp"
#include "DynamicSegTree.hpp"
#include "DynamicStringTreap.hpp"
#include "EulerTour.hpp"
#include "Geometry.hpp"
#include "IntervalSet.hpp"
#include "LazySqrtDecomposition.hpp"
#include "LinearDiophantine.hpp"
#include "LowestCommonAncestor.hpp"
#include "Matrix.hpp"
#include "MosAlgorithm.hpp"
#include "MultiDimensionalCumulativeSum.hpp"
#include "NumberTheory.hpp"
#include "OrderStatisticSet.hpp"
#include "PersistentSegTree.hpp"
#include "Prime.hpp"
#include "Rerooting.hpp"
#include "SegTree2D.hpp"
#include "SparseTable.hpp"
#include "SqrtDecomposition.hpp"
#include "TopologicalSort.hpp"
#include "Trie.hpp"
#include "TwoPointers.hpp"
#include "UnionFind.hpp"
#include "WeightedUnionFind.hpp"
#include "ZobristHash.hpp"
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
