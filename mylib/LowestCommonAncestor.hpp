#pragma once
#include <bits/stdc++.h>

using namespace std;

// ============================================================================
// 最小共通祖先 (Lowest Common Ancestor / LCA) & 木上の2点間距離
// ============================================================================
// ・ダブリング (Binary Lifting) により、木の 2 頂点間の LCA や最短距離を取得する。
// ・辺の重み (非負/実数/整数、デフォルト long long) に対応。重みなしなら cost = 1。
// ・主な機能:
//   - lca(u, v)            : 最小共通祖先 (LCA) を O(log N) で取得
//   - dist(u, v)           : 2頂点間の重み付き最短距離を O(log N) で取得
//   - dist_edges(u, v)     : 2頂点間の辺数 (重みなし距離) を O(log N) で取得
//   - jump(u, k)           : u から根方向に k 個登った頂点を O(log N) で取得
//   - jump_on_path(u, v, k): パス u-v 上で u から k ステップ進んだ頂点を O(log N) で取得
//   - is_on_path(u, v, x)  : 頂点 x が パス u-v 上にあるかを O(log N) で判定
// ============================================================================

template <typename T = long long>
struct LowestCommonAncestor {
  struct Edge {
    int to;
    T cost;
  };

  int n;
  int log_n;
  vector<vector<Edge>> g;
  vector<vector<int>> parent; // parent[k][u]: u の 2^k 個上の親
  vector<int> depth;          // 根からの辺数 (重みなし深さ)
  vector<T> dist_from_root;   // 根からの重み付き距離

  LowestCommonAncestor(int n = 0) : n(n), log_n(1) {
    while ((1 << log_n) <= max(1, n)) log_n++;
    g.resize(n);
  }

  // 無向辺の追加 (重みなしの場合は cost = 1)
  void add_edge(int u, int v, T cost = 1) {
    assert(0 <= u && u < n);
    assert(0 <= v && v < n);
    g[u].push_back({v, cost});
    g[v].push_back({u, cost});
  }

  // 根 root を指定してテーブル構築 (O(N log N))
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

  // 頂点 u と v の最小共通祖先 (LCA) を取得 (O(log N))
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

  // 2頂点 u, v 間の重み付き最短距離を取得 (O(log N))
  T dist(int u, int v) const {
    int a = lca(u, v);
    return dist_from_root[u] + dist_from_root[v] - 2 * dist_from_root[a];
  }

  // 2頂点 u, v 間の辺数 (重みなし最短距離) を取得 (O(log N))
  int dist_edges(int u, int v) const {
    int a = lca(u, v);
    return depth[u] + depth[v] - 2 * depth[a];
  }

  // 頂点 u から根方向に k 個登った頂点を取得 (存在しない場合は -1) (O(log N))
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

  // jump の別名
  int kth_ancestor(int u, int k) const { return jump(u, k); }

  // 頂点 x が u と v の単純パス上にあるかを判定 (O(log N))
  bool is_on_path(int u, int v, int x) const {
    assert(0 <= x && x < n);
    return dist_edges(u, x) + dist_edges(x, v) == dist_edges(u, v);
  }

  // パス u-v 上で、頂点 u から v 方向に k ステップ進んだ頂点を取得 (O(log N))
  // k = 0 ならば u、k = dist_edges(u, v) ならば v を返す。パス範囲外なら -1。
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
