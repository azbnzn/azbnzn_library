#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// 木の直径 (Tree Diameter - 任意非負重み対応・パス復元・中心・最遠点距離)
// ============================================================================
// ・2 回の BFS で木の最遠点対 (直径) を O(N) で求める。
// ・辺の重みが非負の整数/実数 (省略時は重み 1) に対応。
// ・直径の長さ、両端点 u, v、直径パス path (u -> ... -> v)、木の中心 (1〜2頂点)、
//   任意頂点 x の最遠点距離 eccentricity(x) を取得可能。
// ・負の重みを含む木の最長単純パス長が必要な場合は build_allow_negative() を使用。
//
// 使用例:
//   TreeDiameter<long long> td(n);
//   td.add_edge(u, v, cost);
//   td.build();
//   long long d = td.diameter;
//   auto centers = td.get_centers();
//   long long far_x = td.eccentricity(x); // 頂点 x から最も遠い頂点までの距離
// ============================================================================

template <typename T = long long> struct TreeDiameter {
  struct Edge {
    int to;
    T cost;
  };

  int n;
  vector<vector<Edge>> g;
  T diameter;
  int u, v;             // 直径の両端点
  vector<int> path;     // 直径を構成する頂点列 (u -> ... -> v)
  vector<T> dist_u;     // u から各頂点への距離
  vector<T> dist_v;     // v から各頂点への距離

  explicit TreeDiameter(int n = 0)
      : n(n), g(n), diameter(0), u(0), v(0) {
    assert(n >= 0);
  }

  explicit TreeDiameter(const vector<vector<pair<int, T>>> &adj)
      : n((int)adj.size()), g(adj.size()), diameter(0), u(0), v(0) {
    for (int from = 0; from < n; ++from) {
      for (const auto &p : adj[from]) {
        g[from].push_back({p.first, p.second});
      }
    }
  }

  // 無向辺の追加 (重みなしの場合は cost = 1)
  void add_edge(int from, int to, T cost = 1) {
    assert(0 <= from && from < n && 0 <= to && to < n);
    g[from].push_back({to, cost});
    g[to].push_back({from, cost});
  }

  // 2回の探索で直径・両端点・直径パス・端点からの距離配列を計算 (非負重み) O(N)
  T build(int root = 0) {
    if (n == 0) {
      diameter = 0;
      u = v = -1;
      path.clear();
      dist_u.clear();
      dist_v.clear();
      return 0;
    }
    assert(0 <= root && root < n);

    auto [far_u, d1, p1, dist_dummy] = farthest(root);
    (void)d1;
    (void)p1;
    (void)dist_dummy;
    u = far_u;

    auto [far_v, max_d, parent_v, du] = farthest(u);
    v = far_v;
    diameter = max_d;
    dist_u = std::move(du);

    auto [far_back, max_d2, parent_back, dv] = farthest(v);
    (void)far_back;
    (void)max_d2;
    (void)parent_back;
    dist_v = std::move(dv);

    path.clear();
    for (int cur = v; cur != -1; cur = parent_v[cur]) {
      path.push_back(cur);
    }
    reverse(path.begin(), path.end());
    return diameter;
  }

  T run(int root = 0) { return build(root); }

  // 木の中心 (直径パスの中央に位置する 1 個または 2 個の頂点)
  vector<int> get_centers() {
    if (n == 0)
      return {};
    if (path.empty())
      build();
    int sz = (int)path.size();
    if (sz % 2 == 1) {
      return {path[sz / 2]};
    } else {
      return {path[sz / 2 - 1], path[sz / 2]};
    }
  }

  // 任意の頂点 x から最も遠い頂点までの距離 O(1) (全頂点の木の高さ)
  T eccentricity(int x) const {
    assert(0 <= x && x < n);
    assert(!dist_u.empty() && !dist_v.empty());
    return max(dist_u[x], dist_v[x]);
  }

  // 任意の頂点 x から最も遠い端点 (u または v) O(1)
  int farthest_vertex(int x) const {
    assert(0 <= x && x < n);
    assert(!dist_u.empty() && !dist_v.empty());
    return (dist_u[x] >= dist_v[x]) ? u : v;
  }

  // 負の重みを含む木の最長単純パス長を計算し diameter, u, v, path を更新する O(N)
  T build_allow_negative(int root = 0) {
    if (n == 0) {
      diameter = 0;
      u = v = -1;
      path.clear();
      return 0;
    }
    assert(0 <= root && root < n);
    diameter = 0;
    u = v = root;

    // {深さ最大値, 端点}
    auto dfs = [&](auto &self, int cur, int par) -> pair<T, int> {
      T best1_d = 0, best2_d = 0;
      int best1_v = cur, best2_v = cur;
      for (const auto &e : g[cur]) {
        if (e.to == par)
          continue;
        auto [cd, cv] = self(self, e.to, cur);
        T cand_d = cd + e.cost;
        if (cand_d > best1_d) {
          best2_d = best1_d;
          best2_v = best1_v;
          best1_d = cand_d;
          best1_v = cv;
        } else if (cand_d > best2_d) {
          best2_d = cand_d;
          best2_v = cv;
        }
      }
      if (best1_d + best2_d >= diameter) {
        diameter = best1_d + best2_d;
        u = best1_v;
        v = best2_v;
      }
      return {best1_d, best1_v};
    };
    dfs(dfs, root, -1);

    // u -> v パス復元 (u から親を記録)
    vector<int> par_u(n, -1);
    vector<uint8_t> vis(n, 0);
    queue<int> q;
    vis[u] = 1;
    q.push(u);
    while (!q.empty()) {
      int cur = q.front();
      q.pop();
      if (cur == v)
        break;
      for (const auto &e : g[cur]) {
        if (!vis[e.to]) {
          vis[e.to] = 1;
          par_u[e.to] = cur;
          q.push(e.to);
        }
      }
    }
    path.clear();
    for (int cur = v; cur != -1; cur = par_u[cur]) {
      path.push_back(cur);
    }
    reverse(path.begin(), path.end());
    return diameter;
  }

private:
  tuple<int, T, vector<int>, vector<T>> farthest(int start) const {
    vector<T> dist(n, 0);
    vector<uint8_t> vis(n, 0);
    vector<int> parent(n, -1);
    queue<int> que;

    vis[start] = 1;
    dist[start] = 0;
    que.push(start);

    int far_node = start;
    T max_dist = 0;

    while (!que.empty()) {
      int cur = que.front();
      que.pop();

      if (dist[cur] > max_dist) {
        max_dist = dist[cur];
        far_node = cur;
      }

      for (const auto &e : g[cur]) {
        if (!vis[e.to]) {
          vis[e.to] = 1;
          dist[e.to] = dist[cur] + e.cost;
          parent[e.to] = cur;
          que.push(e.to);
        }
      }
    }
    return {far_node, max_dist, std::move(parent), std::move(dist)};
  }
};
