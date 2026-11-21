#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <deque>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// 01-BFS (辺の重みが 0 または 1 のみのグラフに対する最短経路 O(V + E))
// ============================================================================
// 【主な機能】
// 1. ZeroOneBFS<T> (デフォルト T = long long):
//    - add_edge(from, to, w) / add_undirected_edge(u, v, w) (w は 0 または 1)
//    - run(start) / run(starts): 単一始点・多始点 01-BFS (到達不可は dist == -1)
//    - is_reachable(v), dist(v), get_dist_optional(v), dists()
//    - get_path(target), get_path_edges(target): 最短経路復元
//
// 2. 簡易関数:
//    - zero_one_bfs(g, start) / zero_one_bfs(g, starts)
//    - zero_one_bfs_with_path(g, start): {dist, prev_node} を返す
//    - zero_one_bfs_grid(...): グリッド上の 01-BFS (壁破壊や方向コスト 0/1 等)
//
// 使用例:
//   ZeroOneBFS<> bfs(n);
//   bfs.add_edge(u, v, w); // w in {0, 1}
//   bfs.run(0);
//   auto d = bfs.dist(goal);       // 未到達は -1
//   auto path = bfs.get_path(goal); // {0, ..., goal}
//
//   // 簡易関数版 (dijkstra 互換)
//   auto dist = zero_one_bfs(g, 0);
// ============================================================================

template <typename T = long long> struct ZeroOneBFS {
  struct Edge {
    int from;
    int to;
    int cost;
    int id;
  };

private:
  int n_;
  vector<Edge> edges_;
  vector<vector<int>> adj_;
  vector<T> dist_;
  vector<uint8_t> reached_;
  vector<int> parent_;
  vector<int> parent_edge_;

public:
  explicit ZeroOneBFS(int n = 0) : n_(n), adj_(n) { assert(n >= 0); }

  template <typename W>
  explicit ZeroOneBFS(const vector<vector<pair<int, W>>> &g)
      : n_((int)g.size()), adj_(g.size()) {
    for (int u = 0; u < n_; ++u) {
      for (const auto &p : g[u]) {
        assert(p.second == 0 || p.second == 1);
        add_edge(u, p.first, (int)p.second);
      }
    }
  }

  int add_edge(int from, int to, int cost) {
    assert(0 <= from && from < n_);
    assert(0 <= to && to < n_);
    assert(cost == 0 || cost == 1);
    int id = (int)edges_.size();
    edges_.push_back({from, to, cost, id});
    adj_[from].push_back(id);
    return id;
  }

  void add_undirected_edge(int u, int v, int cost) {
    add_edge(u, v, cost);
    add_edge(v, u, cost);
  }

  void clear(int new_n = -1) {
    if (new_n >= 0) {
      assert(new_n >= 0);
      n_ = new_n;
    }
    edges_.clear();
    adj_.assign(n_, {});
    dist_.clear();
    reached_.clear();
    parent_.clear();
    parent_edge_.clear();
  }

  int num_vertices() const { return n_; }
  int num_edges() const { return (int)edges_.size(); }

  const vector<T> &run(int start = 0) {
    if (n_ == 0)
      return run(vector<int>{});
    assert(0 <= start && start < n_);
    return run(vector<int>{start});
  }

  const vector<T> &run(const vector<int> &starts) {
    dist_.assign(n_, (T)-1);
    reached_.assign(n_, 0);
    parent_.assign(n_, -1);
    parent_edge_.assign(n_, -1);

    deque<pair<T, int>> todo;
    for (int s : starts) {
      assert(0 <= s && s < n_);
      if (!reached_[s]) {
        reached_[s] = 1;
        dist_[s] = 0;
        todo.push_back({(T)0, s});
      }
    }

    while (!todo.empty()) {
      auto [c, u] = todo.front();
      todo.pop_front();
      if (c != dist_[u])
        continue;

      for (int eid : adj_[u]) {
        const auto &e = edges_[eid];
        int v = e.to;
        T nc = c + (T)e.cost;
        if (!reached_[v] || nc < dist_[v]) {
          reached_[v] = 1;
          dist_[v] = nc;
          parent_[v] = u;
          parent_edge_[v] = e.id;
          if (e.cost == 0) {
            todo.push_front({nc, v});
          } else {
            todo.push_back({nc, v});
          }
        }
      }
    }
    return dist_;
  }

  bool is_reachable(int v) const {
    assert(0 <= v && v < n_);
    return !reached_.empty() && reached_[v] != 0;
  }

  T dist(int v) const {
    assert(0 <= v && v < n_);
    assert(!dist_.empty());
    return dist_[v];
  }

  T operator[](int v) const { return dist(v); }
  const vector<T> &dists() const { return dist_; }

  optional<T> get_dist_optional(int v) const {
    if (!is_reachable(v))
      return nullopt;
    return dist_[v];
  }

  vector<int> get_path(int target) const {
    if (!is_reachable(target))
      return {};
    vector<int> path;
    for (int cur = target; cur != -1; cur = parent_[cur]) {
      path.push_back(cur);
    }
    reverse(path.begin(), path.end());
    return path;
  }

  vector<int> get_path_edges(int target) const {
    if (!is_reachable(target))
      return {};
    vector<int> path_edges;
    for (int cur = target; parent_[cur] != -1; cur = parent_[cur]) {
      path_edges.push_back(parent_edge_[cur]);
    }
    reverse(path_edges.begin(), path_edges.end());
    return path_edges;
  }
};

// --- 簡易呼び出し関数 ---
template <typename W = long long>
inline vector<long long>
zero_one_bfs(const vector<vector<pair<int, W>>> &g, int start = 0) {
  ZeroOneBFS<long long> bfs(g);
  return bfs.run(start);
}

template <typename W = long long>
inline vector<long long>
zero_one_bfs(const vector<vector<pair<int, W>>> &g, const vector<int> &starts) {
  ZeroOneBFS<long long> bfs(g);
  return bfs.run(starts);
}

template <typename W = long long>
inline pair<vector<long long>, vector<int>>
zero_one_bfs_with_path(const vector<vector<pair<int, W>>> &g, int start = 0) {
  int n = (int)g.size();
  vector<long long> dist(n, -1);
  vector<int> prev_node(n, -1);
  if (n == 0)
    return {dist, prev_node};
  assert(0 <= start && start < n);

  deque<pair<long long, int>> todo;
  dist[start] = 0;
  todo.push_back({0, start});

  while (!todo.empty()) {
    auto [c, u] = todo.front();
    todo.pop_front();
    if (c != dist[u])
      continue;
    for (const auto &edge : g[u]) {
      int to = edge.first;
      assert(edge.second == 0 || edge.second == 1);
      int w = (int)edge.second;
      long long nc = c + w;
      if (dist[to] == -1 || nc < dist[to]) {
        dist[to] = nc;
        prev_node[to] = u;
        if (w == 0)
          todo.push_front({nc, to});
        else
          todo.push_back({nc, to});
      }
    }
  }
  return {dist, prev_node};
}

// --- グリッド版 01-BFS ---
// step_cost(x, y, nx, ny) -> int: 0 (push_front), 1 (push_back), -1 (通行不可)
template <typename T = long long, typename StepCost>
inline vector<vector<T>> zero_one_bfs_grid(int h, int w,
                                           const vector<pair<int, int>> &starts,
                                           StepCost step_cost, int dirs = 4) {
  assert(h >= 0 && w >= 0);
  assert(dirs == 4 || dirs == 8);
  vector<vector<T>> dist(h, vector<T>(w, (T)-1));
  if (h == 0 || w == 0)
    return dist;

  static constexpr int dx8[8] = {1, 0, -1, 0, 1, 1, -1, -1};
  static constexpr int dy8[8] = {0, 1, 0, -1, 1, -1, 1, -1};

  deque<pair<T, pair<int, int>>> todo;
  for (auto [sx, sy] : starts) {
    assert(0 <= sx && sx < h && 0 <= sy && sy < w);
    if (dist[sx][sy] == (T)-1) {
      dist[sx][sy] = 0;
      todo.push_back({(T)0, {sx, sy}});
    }
  }

  while (!todo.empty()) {
    auto [c, pos] = todo.front();
    auto [x, y] = pos;
    todo.pop_front();
    if (c != dist[x][y])
      continue;

    for (int k = 0; k < dirs; ++k) {
      int nx = x + dx8[k];
      int ny = y + dy8[k];
      if (nx < 0 || nx >= h || ny < 0 || ny >= w)
        continue;
      auto raw_step = step_cost(x, y, nx, ny);
      if constexpr (is_signed_v<decltype(raw_step)>) {
        if (raw_step < 0)
          continue;
      }
      assert(raw_step == 0 || raw_step == 1);
      int w_step = (int)raw_step;
      T nc = c + (T)w_step;
      if (dist[nx][ny] == (T)-1 || nc < dist[nx][ny]) {
        dist[nx][ny] = nc;
        if (w_step == 0)
          todo.push_front({nc, {nx, ny}});
        else
          todo.push_back({nc, {nx, ny}});
      }
    }
  }
  return dist;
}

template <typename T = long long, typename StepCost>
inline vector<vector<T>> zero_one_bfs_grid(int h, int w, pair<int, int> start,
                                           StepCost step_cost, int dirs = 4) {
  return zero_one_bfs_grid<T>(h, w, vector<pair<int, int>>{start}, step_cost,
                              dirs);
}

// 文字グリッド版 (cost0_ch への移動はコスト 0、それ以外はコスト 1、blocked_ch は進入不可)
inline vector<vector<long long>>
zero_one_bfs_grid(const vector<string> &grid, pair<int, int> start,
                  char cost0_ch = '.', char blocked_ch = '\0', int dirs = 4) {
  int h = (int)grid.size();
  int w = (h == 0) ? 0 : (int)grid[0].size();
  auto step_cost = [&](int, int, int nx, int ny) -> int {
    char ch = grid[nx][ny];
    if (blocked_ch != '\0' && ch == blocked_ch)
      return -1;
    return (ch == cost0_ch) ? 0 : 1;
  };
  return zero_one_bfs_grid<long long>(h, w, start, step_cost, dirs);
}
