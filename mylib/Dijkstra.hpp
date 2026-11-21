#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <limits>
#include <optional>
#include <queue>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

namespace dijkstra_internal {

template <typename T> inline T add_saturated(T a, T b) {
  if constexpr (is_integral_v<T> && sizeof(T) <= 8) {
    if constexpr (is_signed_v<T>) {
      __int128 res = (__int128)a + (__int128)b;
      constexpr __int128 hi = (__int128)numeric_limits<T>::max();
      return res >= hi ? numeric_limits<T>::max() : (T)res;
    } else {
      unsigned __int128 res = (unsigned __int128)a + (unsigned __int128)b;
      constexpr unsigned __int128 hi =
          (unsigned __int128)numeric_limits<T>::max();
      return res >= hi ? numeric_limits<T>::max() : (T)res;
    }
  } else if constexpr (is_same_v<T, __int128_t>) {
    constexpr T hi = (T)((((unsigned __int128)1) << 127) - 1);
    return (b > hi - a) ? hi : (a + b);
  } else if constexpr (is_same_v<T, unsigned __int128>) {
    constexpr T hi = ~(T)0;
    return (b > hi - a) ? hi : (a + b);
  } else {
    return a + b;
  }
}

} // namespace dijkstra_internal

// ============================================================================
// ダイクストラ法 (Dijkstra's Algorithm) O((V + E) log V)
// ============================================================================
// 【主な機能】
// 1. 簡易関数 (互換 API):
//    - dijkstra(g, start_pos) / dijkstra(g, starts): 各頂点への最短距離 (未到達は -1)
//    - dijkstra_with_path(g, start_pos): {dist, prev_node} を返す
//    - dijkstra_grid(...): グリッド上のダイクストラ法 (任意コスト・4/8方向対応)
//
// 2. Dijkstra<T> 構造体 (デフォルト T = long long):
//    - run(start) / run(starts) / get_path(target) / get_path_edges(target)
//    - is_reachable(v) / dist(v) / get_dist_optional(v) / dists()
// ============================================================================

template <typename T = long long> struct Dijkstra {
  struct Edge {
    int from;
    int to;
    T cost;
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
  explicit Dijkstra(int n = 0) : n_(n), adj_(n) { assert(n >= 0); }

  explicit Dijkstra(const vector<vector<pair<int, T>>> &g)
      : n_((int)g.size()), adj_(g.size()) {
    for (int u = 0; u < n_; ++u) {
      for (const auto &p : g[u]) {
        add_edge(u, p.first, p.second);
      }
    }
  }

  int add_edge(int from, int to, T cost) {
    assert(0 <= from && from < n_);
    assert(0 <= to && to < n_);
    assert(cost >= 0);
    int id = (int)edges_.size();
    edges_.push_back({from, to, cost, id});
    adj_[from].push_back(id);
    return id;
  }

  void add_undirected_edge(int u, int v, T cost) {
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

    using P = pair<T, int>;
    priority_queue<P, vector<P>, greater<P>> todo;
    for (int s : starts) {
      assert(0 <= s && s < n_);
      if (!reached_[s]) {
        reached_[s] = 1;
        dist_[s] = 0;
        todo.push({(T)0, s});
      }
    }

    while (!todo.empty()) {
      auto [c, u] = todo.top();
      todo.pop();
      if (c > dist_[u])
        continue;

      for (int eid : adj_[u]) {
        const auto &e = edges_[eid];
        int v = e.to;
        T nc = dijkstra_internal::add_saturated(c, e.cost);
        if (!reached_[v] || nc < dist_[v]) {
          reached_[v] = 1;
          dist_[v] = nc;
          parent_[v] = u;
          parent_edge_[v] = e.id;
          todo.push({nc, v});
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

// ダイクストラ法 (単一始点最短経路)
// g: 隣接リスト g[from] = {{to, cost}, ...}
// start_pos: 始点
// 戻り値: 各頂点への最短距離 (到達不可は -1)
inline vector<long long>
dijkstra(const vector<vector<pair<int, long long>>> &g, int start_pos) {
  int n = (int)g.size();
  vector<long long> dist(n, -1);
  if (n == 0)
    return dist;
  assert(0 <= start_pos && start_pos < n);

  using P = pair<long long, int>;
  priority_queue<P, vector<P>, greater<P>> todo;
  dist[start_pos] = 0;
  todo.push({0, start_pos});

  while (!todo.empty()) {
    auto [c, p] = todo.top();
    todo.pop();
    if (c > dist[p]) {
      continue;
    }
    for (const auto &edge : g[p]) {
      int to = edge.first;
      long long cost = edge.second;
      assert(cost >= 0);
      long long nc = dijkstra_internal::add_saturated(c, cost);
      if (dist[to] == -1 || nc < dist[to]) {
        dist[to] = nc;
        todo.push({nc, to});
      }
    }
  }
  return dist;
}

// 多始点ダイクストラ法
inline vector<long long>
dijkstra(const vector<vector<pair<int, long long>>> &g,
         const vector<int> &starts) {
  Dijkstra<long long> dij(g);
  return dij.run(starts);
}

// スペルミス互換用エイリアス
inline vector<long long> dijksta(const vector<vector<pair<int, long long>>> &g,
                                 int start_pos) {
  return dijkstra(g, start_pos);
}

// 経路復元用 prev_node 付きダイクストラ ({dist, prev_node} を返す)
inline pair<vector<long long>, vector<int>>
dijkstra_with_path(const vector<vector<pair<int, long long>>> &g,
                   int start_pos) {
  int n = (int)g.size();
  vector<long long> dist(n, -1);
  vector<int> prev_node(n, -1);
  if (n == 0)
    return {dist, prev_node};
  assert(0 <= start_pos && start_pos < n);

  using P = pair<long long, int>;
  priority_queue<P, vector<P>, greater<P>> todo;
  dist[start_pos] = 0;
  todo.push({0, start_pos});

  while (!todo.empty()) {
    auto [c, p] = todo.top();
    todo.pop();
    if (c > dist[p])
      continue;
    for (const auto &edge : g[p]) {
      int to = edge.first;
      long long cost = edge.second;
      assert(cost >= 0);
      long long nc = dijkstra_internal::add_saturated(c, cost);
      if (dist[to] == -1 || nc < dist[to]) {
        dist[to] = nc;
        prev_node[to] = p;
        todo.push({nc, to});
      }
    }
  }
  return {dist, prev_node};
}

// --- グリッド版ダイクストラ法 ---
// step_cost(x, y, nx, ny) -> T: 0 以上なら移動コスト、負なら通行不可
template <typename T = long long, typename StepCost>
inline vector<vector<T>> dijkstra_grid(int h, int w,
                                       const vector<pair<int, int>> &starts,
                                       StepCost step_cost, int dirs = 4) {
  assert(h >= 0 && w >= 0);
  assert(dirs == 4 || dirs == 8);
  vector<vector<T>> dist(h, vector<T>(w, (T)-1));
  vector<vector<uint8_t>> reached(h, vector<uint8_t>(w, 0));
  if (h == 0 || w == 0)
    return dist;

  static constexpr int dx8[8] = {1, 0, -1, 0, 1, 1, -1, -1};
  static constexpr int dy8[8] = {0, 1, 0, -1, 1, -1, 1, -1};

  using P = pair<T, pair<int, int>>;
  priority_queue<P, vector<P>, greater<P>> todo;
  for (auto [sx, sy] : starts) {
    assert(0 <= sx && sx < h && 0 <= sy && sy < w);
    if (!reached[sx][sy]) {
      reached[sx][sy] = 1;
      dist[sx][sy] = 0;
      todo.push({(T)0, {sx, sy}});
    }
  }

  while (!todo.empty()) {
    auto [c, pos] = todo.top();
    auto [x, y] = pos;
    todo.pop();
    if (c > dist[x][y])
      continue;

    for (int k = 0; k < dirs; ++k) {
      int nx = x + dx8[k];
      int ny = y + dy8[k];
      if (nx < 0 || nx >= h || ny < 0 || ny >= w)
        continue;
      T w_step = (T)step_cost(x, y, nx, ny);
      if constexpr (is_signed_v<T>) {
        if (w_step < 0)
          continue;
      }
      T nc = dijkstra_internal::add_saturated(c, w_step);
      if (!reached[nx][ny] || nc < dist[nx][ny]) {
        reached[nx][ny] = 1;
        dist[nx][ny] = nc;
        todo.push({nc, {nx, ny}});
      }
    }
  }
  return dist;
}

template <typename T = long long, typename StepCost>
inline vector<vector<T>> dijkstra_grid(int h, int w, pair<int, int> start,
                                       StepCost step_cost, int dirs = 4) {
  return dijkstra_grid<T>(h, w, vector<pair<int, int>>{start}, step_cost, dirs);
}

inline vector<vector<long long>>
dijkstra_grid(const vector<string> &grid, pair<int, int> start,
              char wall = '#', long long step = 1, int dirs = 4) {
  int h = (int)grid.size();
  int w = (h == 0) ? 0 : (int)grid[0].size();
  auto step_cost = [&](int, int, int nx, int ny) -> long long {
    return (grid[nx][ny] == wall) ? -1LL : step;
  };
  return dijkstra_grid<long long>(h, w, start, step_cost, dirs);
}
