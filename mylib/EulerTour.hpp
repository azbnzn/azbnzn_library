#pragma once
#include <bits/stdc++.h>

using namespace std;

// ============================================================================
// 木のオイラーツアー (Euler Tour)
// ============================================================================
// ・木構造を DFS で巡回し、訪問時刻の配列に平坦化する。
// ・部分木 u の頂点は訪問時刻の半開区間 [in[u], out[u]) に一対一対応する (サイズ n)。
// ・BIT やセグ木と組み合わせることで、部分木加算や部分木和取得が O(log N) で可能。
// ・内蔵の Sparse Table により、LCA (最小共通祖先) や頂点間距離を O(1) で取得可能。
//
// 使用例:
//   EulerTour et(n);
//   et.add_edge(u, v);
//   et.build(root); // デフォルト root = 0
//   auto [l, r] = et.subtree_range(u); // 部分木 u の半開区間 [l, r)
//   int a = et.lca(u, v);              // LCA (最小共通祖先)
//   int d = et.dist(u, v);             // 2頂点間の最短距離 (辺数)
// ============================================================================

struct EulerTour {
  int n;
  vector<vector<int>> g;
  vector<int> in, out;     // 部分木 u に対応する区間 [in[u], out[u]) (サイズ n)
  vector<int> tour;        // 訪問時刻順の頂点列 (サイズ n)
  vector<int> depth;       // 各頂点の深さ (根は 0)
  vector<int> parent;      // 親頂点 (根は -1)

  // LCA 用データ (Sparse Table on Euler Tour of size 2*N-1)
  vector<int> node_visit;  // 頂点訪問履歴 (サイズ 2*n - 1)
  vector<int> first_visit; // 各頂点が node_visit に最初に現れるインデックス
  vector<vector<int>> st;  // Sparse Table (深さが最小となる node_visit のインデックスを保持)

  EulerTour(int n = 0)
      : n(n), g(n), in(n), out(n), tour(n), depth(n, 0), parent(n, -1),
        first_visit(n, 0) {}

  // 無向辺を追加 (0-indexed)
  void add_edge(int u, int v) {
    g[u].push_back(v);
    g[v].push_back(u);
  }

  // 根を指定してオイラーツアーおよび LCA 構造を構築 (デフォルト root = 0)
  void build(int root = 0) {
    int timer = 0;
    node_visit.clear();
    node_visit.reserve(2 * n);
    dfs(root, -1, 0, timer);
    build_lca();
  }

  // 部分木 u に対応する半開区間 [l, r) を取得
  pair<int, int> subtree_range(int u) const {
    return {in[u], out[u]};
  }

  // 部分木 u のサイズを取得
  int subtree_size(int u) const {
    return out[u] - in[u];
  }

  // u が v の祖先 (u == v 含む) かどうか判定
  bool is_ancestor(int u, int v) const {
    return in[u] <= in[v] && out[v] <= out[u];
  }

  // u と v の最小共通祖先 (LCA) を O(1) で取得
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

  // 頂点 u と v の最短距離 (辺数) を O(1) で取得
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
