#pragma once
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// 木のオイラーツアー (Euler Tour)
// ============================================================================
// ・木構造を DFS で巡回し、訪問時刻の配列に平坦化する。
// ・部分木 u の頂点は訪問時刻の半開区間 [in[u], out[u]) に一対一対応する (サイズ n)。
// ・行きがけと帰りがけの両方で進める時刻 in2, out2 (範囲 [0, 2*n)) も同時に記録する。
//   - 部分木クエリ: subtree_range(u) = {in[u], out[u]} (サイズ n)
//   - 根からのパス和クエリ: path_indices(u) = {in2[u], out2[u]} (サイズ 2*n の BIT / セグ木用)
// ・BIT やセグ木と組み合わせることで、部分木加算/和・1点更新+パス和取得が O(log N) で可能。
// ・内蔵の Sparse Table により、LCA (最小共通祖先) や頂点間距離を O(1) で取得可能。
//
// 使用例:
//   EulerTour et(n);
//   et.add_edge(u, v);
//   et.build(root); // デフォルト root = 0
//
//   // 1. 部分木クエリ (サイズ n)
//   auto [l, r] = et.subtree_range(u); // 部分木 u の半開区間 [in[u], out[u])
//
//   // 2. 根からのパス和クエリ (1点更新 + パス和取得, BIT サイズ 2*n)
//   // atcoder::fenwick_tree<long long> bit(2 * n);
//   auto [in_i, out_i] = et.path_indices(u);          // {in2[u], out2[u]}
//   bit.add(in_i, val[u]); bit.add(out_i, -val[u]);   // 頂点 u に +val[u] を寄与させる (更新も +delta, -delta)
//   long long path_sum = bit.sum(0, et.in2[v] + 1);   // 根から頂点 v までのパス上の総和
//
//   // 3. LCA・距離 (O(1))
//   int a = et.lca(u, v);              // LCA (最小共通祖先)
//   int d = et.dist(u, v);             // 2頂点間の最短距離 (辺数)
// ============================================================================

struct EulerTour {
  int n;
  vector<vector<int>> g;
  vector<int> in, out;     // 部分木 u に対応する区間 [in[u], out[u]) (サイズ n)
  vector<int> tour;        // 訪問時刻順の頂点列 (サイズ n)
  vector<int> in2, out2;   // 行きがけ・帰りがけ両方で +1 する時刻 (値の範囲 [0, 2*n))
  vector<int> depth;       // 各頂点の深さ (根は 0)
  vector<int> parent;      // 親頂点 (根は -1)

  // LCA 用データ (Sparse Table on Euler Tour of size 2*N-1)
  vector<int> node_visit;  // 頂点訪問履歴 (サイズ 2*n - 1)
  vector<int> first_visit; // 各頂点が node_visit に最初に現れるインデックス
  vector<vector<int>> st;  // Sparse Table (深さが最小となる node_visit のインデックスを保持)

  EulerTour(int n = 0)
      : n(n), g(n), in(n), out(n), tour(n), in2(n), out2(n), depth(n, 0),
        parent(n, -1), first_visit(n, 0) {}

  // 無向辺を追加 (0-indexed)
  void add_edge(int u, int v) {
    g[u].push_back(v);
    g[v].push_back(u);
  }

  // 根を指定してオイラーツアーおよび LCA 構造を構築 (デフォルト root = 0)
  void build(int root = 0) {
    if (n == 0)
      return;
    assert(0 <= root && root < n);
    int timer = 0, timer2 = 0;
    in.assign(n, 0);
    out.assign(n, 0);
    tour.assign(n, 0);
    in2.assign(n, 0);
    out2.assign(n, 0);
    depth.assign(n, 0);
    parent.assign(n, -1);
    node_visit.clear();
    node_visit.reserve(2 * n);
    dfs(root, -1, 0, timer, timer2);
    build_lca();
  }

  // 部分木 u に対応する半開区間 [l, r) を取得
  pair<int, int> subtree_range(int u) const {
    return {in[u], out[u]};
  }

  // 頂点 u のパス寄与更新用インデックス {in2[u], out2[u]} (値の範囲 [0, 2*N)) を取得
  pair<int, int> path_indices(int u) const {
    return {in2[u], out2[u]};
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
  void dfs(int u, int p, int d, int &timer, int &timer2) {
    parent[u] = p;
    depth[u] = d;
    in[u] = timer;
    tour[timer] = u;
    timer++;

    in2[u] = timer2;
    timer2++;

    first_visit[u] = node_visit.size();
    node_visit.push_back(u);

    for (int v : g[u]) {
      if (v == p)
        continue;
      dfs(v, u, d + 1, timer, timer2);
      node_visit.push_back(u);
    }
    out[u] = timer;
    out2[u] = timer2;
    timer2++;
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
