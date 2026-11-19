#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <optional>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// 差分制約系ソルバー / 牛ゲー専用ライブラリ (DifferenceConstraints)
// ============================================================================
// n 個の変数 x_0, ..., x_{n-1} に関する線形不等式制約を最短経路問題に帰着して解きます:
//   - x_u - x_v <= w   <=>  辺 v -> u (重み w)
//   - x_u - x_v >= w   <=>  辺 u -> v (重み -w)
//   - x_u <= w, x_u >= w などの絶対値境界 (内部参照頂点 n を基準 x_zero = 0 として管理)
//
// 【特徴・構成方法と距離の取得】
// - 非負辺のみ (w >= 0) の場合は自動的に Dijkstra O((V+E) log V) で高速実行し、
//   負辺を含む場合は Bellman-Ford O(VE) または SPFA を使用します。
// - 最小・最大の「値」だけでなく、それを構成する解配列 solution() および
//   最短経路アルゴリズムが計算した生の距離配列 dist(v) / dists()、パス復元 get_path(v) を取得可能です。
//   1. solve()                 : 制約を満たす任意の解 x[0..n-1] を 1 つ構成
//   2. solve_max(base = -1)    : x_base = 0 基準で各 x_i を同時最大化 (元グラフ最短距離 d[i])
//   3. solve_min(base = -1)    : x_base = 0 基準で各 x_i を同時最小化 (反転グラフ最短距離 -d_rev[i])
//   4. max_diff(u, v) / min_diff(u, v) : 2変数差の上限・下限と達成解 solution() / dists() を計算
//
// 使用例:
//   DifferenceConstraints<long long> dc(n);
//   dc.add_diff_leq(1, 0, 5);  // x_1 - x_0 <= 5
//   dc.add_diff_geq(2, 1, 3);  // x_2 - x_1 >= 3
//   dc.add_bound_geq(0, 0);    // x_0 >= 0 (base = -1 の基準変数 x_zero = 0 に対する制約)
//   if (auto x = dc.solve_min()) {
//     // (*x)[i]: 最小を達成する構成解
//     // dc.dist(i): 反転グラフ上の生の最短距離 (= -(*x)[i])
//   }
// ============================================================================

namespace dc_internal {

template <typename T> struct Limits {
  static constexpr T max_val() {
    if constexpr (is_same_v<T, __int128_t>) {
      return (T)((((unsigned __int128)1) << 126) - 1 +
                 (((unsigned __int128)1) << 126));
    } else {
      return numeric_limits<T>::max();
    }
  }
  static constexpr T min_val() {
    if constexpr (is_same_v<T, __int128_t>) {
      return -max_val() - 1;
    } else {
      return numeric_limits<T>::lowest();
    }
  }
};

} // namespace dc_internal

template <typename T = long long> struct DifferenceConstraints {
  static constexpr T INF = dc_internal::Limits<T>::max_val() / 2;
  static constexpr T NEG_INF = dc_internal::Limits<T>::min_val() / 2;

  enum Status : uint8_t { FINITE = 0, INFEASIBLE = 1, UNBOUNDED = 2 };
  enum Algorithm : uint8_t {
    NONE = 0,
    DIJKSTRA = 1,
    BELLMAN_FORD = 2,
    SPFA = 3
  };

  struct BoundResult {
    Status status;
    T value = 0;
  };

  struct Edge {
    int from;
    int to;
    T cost;
  };

private:
  int n_; // 頂点 0..n-1 が変数、頂点 n が基準変数 x_zero (= 0)
  T eps_;
  bool has_negative_edge_ = false;
  vector<Edge> edges_;
  vector<vector<pair<int, int>>> adj_;     // adj_[u] = {{v, edge_id}, ...}
  vector<vector<pair<int, int>>> rev_adj_; // rev_adj_[v] = {{u, edge_id}, ...}

  // 直近の探索・構成結果 (サイズ n_ + 1、末尾 n_ が x_zero)
  vector<T> dist_;
  vector<uint8_t> reached_;
  vector<int> parent_;
  vector<T> solution_;
  Status last_status_ = FINITE;
  Algorithm last_algo_ = NONE;
  int last_source_ = -1;
  bool last_reversed_ = false;
  int last_cycle_node_ = -1;

  void add_edge_internal(int from, int to, T cost) {
    assert(0 <= from && from <= n_ && 0 <= to && to <= n_);
    if (cost < (T)0)
      has_negative_edge_ = true;
    int id = (int)edges_.size();
    edges_.push_back({from, to, cost});
    adj_[from].push_back({to, id});
    rev_adj_[to].push_back({from, id});
  }

  pair<T, bool> add_clamped(T d, T cost) const {
    if constexpr (is_integral_v<T> && sizeof(T) <= 8) {
      __int128 res = (__int128)d + (__int128)cost;
      if (res <= (__int128)NEG_INF)
        return {NEG_INF, true};
      if (res >= (__int128)INF)
        return {INF, true};
      return {(T)res, false};
    } else if constexpr (is_same_v<T, __int128_t>) {
      if (cost < 0 && d < NEG_INF - cost)
        return {NEG_INF, true};
      if (cost > 0 && d > INF - cost)
        return {INF, true};
      T res = d + cost;
      if (res <= NEG_INF)
        return {NEG_INF, true};
      if (res >= INF)
        return {INF, true};
      return {res, false};
    } else {
      T res = d + cost;
      if (res <= NEG_INF)
        return {NEG_INF, true};
      if (res >= INF)
        return {INF, true};
      return {res, false};
    }
  }

  bool is_better(T cand, T cur) const { return cand < cur - eps_; }

  // 全頂点 0..n_ から一斉開始する実行可能性判定 (ポテンシャル p も生成)
  bool run_all_sources_core(bool use_spfa, vector<T> &out_dist,
                            vector<uint8_t> &out_reached,
                            vector<int> &out_parent,
                            int &out_cycle_node, Algorithm &out_algo) const {
    int total = n_ + 1;
    out_dist.assign(total, (T)0);
    out_reached.assign(total, 1);
    out_parent.assign(total, -1);
    out_cycle_node = -1;

    if (!has_negative_edge_) {
      out_algo = DIJKSTRA;
      return true;
    }

    if (use_spfa) {
      out_algo = SPFA;
      queue<int> q;
      vector<uint8_t> in_q(total, 1);
      vector<int> path_len(total, 0);
      for (int i = 0; i < total; ++i)
        q.push(i);
      int rounds = 0;
      int rem = total;

      auto has_parent_cycle = [&](int v) -> pair<bool, int> {
        int cur = v;
        for (int step = 1; step <= total; ++step) {
          cur = out_parent[cur];
          if (cur == -1)
            return {false, step - 1};
        }
        return {true, total};
      };

      while (!q.empty()) {
        int u = q.front();
        q.pop();
        in_q[u] = 0;

        for (auto [v, eid] : adj_[u]) {
          auto [cand, clamped] = add_clamped(out_dist[u], edges_[eid].cost);
          if (clamped && cand <= NEG_INF) {
            out_dist[v] = NEG_INF;
            out_parent[v] = u;
            out_cycle_node = v;
            return false;
          }
          if (is_better(cand, out_dist[v])) {
            out_dist[v] = cand;
            out_parent[v] = u;
            path_len[v] = path_len[u] + 1;
            if (path_len[v] >= total) {
              auto [has_cyc, actual_len] = has_parent_cycle(v);
              if (has_cyc) {
                out_cycle_node = v;
                return false;
              }
              path_len[v] = actual_len;
            }
            if (!in_q[v]) {
              q.push(v);
              in_q[v] = 1;
            }
          }
        }
        if (--rem == 0) {
          rounds++;
          if (!q.empty()) {
            if (rounds >= total) {
              // フォールバックとして決定論的ベルマンフォードを実行
              break;
            }
            rem = (int)q.size();
          }
        }
      }
      if (q.empty())
        return true;
      // rounds >= total で抜けた場合は決定論的ベルマンフォードへフォールバック
      out_dist.assign(total, (T)0);
      out_parent.assign(total, -1);
    }

    out_algo = BELLMAN_FORD;
    for (int it = 0; it < total; ++it) {
      bool updated = false;
      for (const auto &e : edges_) {
        auto [cand, clamped] = add_clamped(out_dist[e.from], e.cost);
        if (clamped && cand <= NEG_INF) {
          out_dist[e.to] = NEG_INF;
          out_parent[e.to] = e.from;
          out_cycle_node = e.to;
          return false;
        }
        if (is_better(cand, out_dist[e.to])) {
          out_dist[e.to] = cand;
          out_parent[e.to] = e.from;
          updated = true;
          if (it == total - 1) {
            out_cycle_node = e.to;
            return false;
          }
        }
      }
      if (!updated)
        break;
    }
    return true;
  }

  // 単一始点 start から最短経路を実行 (元グラフまたは反転グラフ)
  // ※ 事前に負閉路がない (feasible) グラフ上で呼ぶこと
  void run_sssp(int start, bool reversed, bool use_spfa, vector<T> &out_dist,
                vector<uint8_t> &out_reached, vector<int> &out_parent,
                Algorithm &out_algo) const {
    int total = n_ + 1;
    out_dist.assign(total, INF);
    out_reached.assign(total, 0);
    out_parent.assign(total, -1);

    const auto &g = reversed ? rev_adj_ : adj_;
    out_dist[start] = 0;
    out_reached[start] = 1;

    if (!has_negative_edge_) {
      out_algo = DIJKSTRA;
      using P = pair<T, int>;
      priority_queue<P, vector<P>, greater<P>> pq;
      pq.push({(T)0, start});
      while (!pq.empty()) {
        auto [c, u] = pq.top();
        pq.pop();
        if (c > out_dist[u])
          continue;
        for (auto [v, eid] : g[u]) {
          auto [nc, _] = add_clamped(c, edges_[eid].cost);
          if (!out_reached[v] || is_better(nc, out_dist[v])) {
            out_reached[v] = 1;
            out_dist[v] = nc;
            out_parent[v] = u;
            pq.push({nc, v});
          }
        }
      }
      return;
    }

    if (use_spfa) {
      out_algo = SPFA;
      queue<int> q;
      vector<uint8_t> in_q(total, 0);
      q.push(start);
      in_q[start] = 1;
      int rounds = 0;
      int rem = 1;

      while (!q.empty()) {
        int u = q.front();
        q.pop();
        in_q[u] = 0;
        for (auto [v, eid] : g[u]) {
          auto [nc, _] = add_clamped(out_dist[u], edges_[eid].cost);
          if (!out_reached[v] || is_better(nc, out_dist[v])) {
            out_reached[v] = 1;
            out_dist[v] = nc;
            out_parent[v] = u;
            if (!in_q[v]) {
              q.push(v);
              in_q[v] = 1;
            }
          }
        }
        if (--rem == 0) {
          rounds++;
          if (!q.empty()) {
            if (rounds >= total)
              break;
            rem = (int)q.size();
          }
        }
      }
      if (q.empty())
        return;
      out_dist.assign(total, INF);
      out_reached.assign(total, 0);
      out_parent.assign(total, -1);
      out_dist[start] = 0;
      out_reached[start] = 1;
    }

    out_algo = BELLMAN_FORD;
    for (int it = 0; it < total; ++it) {
      bool updated = false;
      for (const auto &e : edges_) {
        int u = reversed ? e.to : e.from;
        int v = reversed ? e.from : e.to;
        if (!out_reached[u])
          continue;
        auto [nc, _] = add_clamped(out_dist[u], e.cost);
        if (!out_reached[v] || is_better(nc, out_dist[v])) {
          out_reached[v] = 1;
          out_dist[v] = nc;
          out_parent[v] = u;
          updated = true;
        }
      }
      if (!updated)
        break;
    }
  }

  // 到達可能部の最短路 d と全体ポテンシャル p を合成し、全変数が制約を満たす完全解を作る
  void build_completed_solution(const vector<T> &d,
                                const vector<uint8_t> &reached,
                                const vector<T> &p) {
    using W_T = conditional_t<is_integral_v<T> && sizeof(T) <= 8, __int128_t, T>;
    bool has_c = false;
    W_T shift_c = 0;
    for (int i = 0; i <= n_; ++i) {
      if (reached[i]) {
        W_T cand = (W_T)d[i] - (W_T)p[i];
        if (!has_c || cand > shift_c) {
          has_c = true;
          shift_c = cand;
        }
      }
    }
    vector<W_T> x_full(n_ + 1);
    for (int i = 0; i <= n_; ++i) {
      x_full[i] = reached[i] ? (W_T)d[i] : ((W_T)p[i] + shift_c);
    }
    W_T zero_off = x_full[n_];
    solution_.resize(n_);
    for (int i = 0; i < n_; ++i) {
      solution_[i] = (T)(x_full[i] - zero_off);
    }
  }

public:
  explicit DifferenceConstraints(int n = 0, T eps = 0)
      : n_(n), eps_(eps), adj_(n + 1), rev_adj_(n + 1) {
    assert(n >= 0);
  }

  int num_variables() const { return n_; }
  int num_constraints() const { return (int)edges_.size(); }
  bool has_negative_edge() const { return has_negative_edge_; }
  const vector<Edge> &edges() const { return edges_; }

  void clear(int new_n = -1) {
    if (new_n >= 0) {
      assert(new_n >= 0);
      n_ = new_n;
    }
    edges_.clear();
    adj_.assign(n_ + 1, {});
    rev_adj_.assign(n_ + 1, {});
    has_negative_edge_ = false;
    dist_.clear();
    reached_.clear();
    parent_.clear();
    solution_.clear();
    last_status_ = FINITE;
    last_algo_ = NONE;
    last_source_ = -1;
    last_reversed_ = false;
    last_cycle_node_ = -1;
  }

  // --- 制約の追加 ---
  // x_u - x_v <= w
  void add_diff_leq(int u, int v, T w) {
    assert(0 <= u && u < n_ && 0 <= v && v < n_);
    add_edge_internal(v, u, w);
  }

  // x_u - x_v >= w  <=>  x_v - x_u <= -w
  void add_diff_geq(int u, int v, T w) {
    assert(0 <= u && u < n_ && 0 <= v && v < n_);
    add_edge_internal(u, v, -w);
  }

  // x_u - x_v < w (整数型では x_u - x_v <= w - 1)
  void add_diff_lt(int u, int v, T w) {
    static_assert(is_integral_v<T>, "add_diff_lt requires integral type");
    add_diff_leq(u, v, w - 1);
  }

  // x_u - x_v > w (整数型では x_u - x_v >= w + 1)
  void add_diff_gt(int u, int v, T w) {
    static_assert(is_integral_v<T>, "add_diff_gt requires integral type");
    add_diff_geq(u, v, w + 1);
  }

  // x_u - x_v == w
  void add_diff_eq(int u, int v, T w) {
    add_diff_leq(u, v, w);
    add_diff_geq(u, v, w);
  }

  // x_u <= w  <=>  x_u - x_zero <= w
  void add_bound_leq(int u, T w) {
    assert(0 <= u && u < n_);
    add_edge_internal(n_, u, w);
  }

  // x_u >= w  <=>  x_zero - x_u <= -w
  void add_bound_geq(int u, T w) {
    assert(0 <= u && u < n_);
    add_edge_internal(u, n_, -w);
  }

  // x_u == w
  void add_bound_eq(int u, T w) {
    add_bound_leq(u, w);
    add_bound_geq(u, w);
  }

  // low <= x_u <= high
  void add_range(int u, T low, T high) {
    add_bound_geq(u, low);
    add_bound_leq(u, high);
  }

  // --- 実行可能性・任意解・最小化/最大化解構成 ---
  // 制約系に矛盾がないか判定 (非負辺のみなら O(1) で true)
  bool is_feasible(bool use_spfa = false) {
    int cyc_node = -1;
    bool ok = run_all_sources_core(use_spfa, dist_, reached_, parent_, cyc_node,
                                   last_algo_);
    last_source_ = -1;
    last_reversed_ = false;
    last_cycle_node_ = cyc_node;
    if (!ok) {
      last_status_ = INFEASIBLE;
      solution_.clear();
      return false;
    }
    last_status_ = FINITE;
    T zero_val = dist_[n_];
    solution_.resize(n_);
    for (int i = 0; i < n_; ++i) {
      solution_[i] = dist_[i] - zero_val;
    }
    return true;
  }

  // すべての制約を満たす実行可能解 x[0..n-1] を 1 つ構成して返す (矛盾時は nullopt)
  optional<vector<T>> solve(bool use_spfa = false) {
    if (!is_feasible(use_spfa))
      return nullopt;
    return solution_;
  }

  // 基準頂点 base (x_base = 0; base == -1 のときは絶対値基準 x_zero = 0) のもとで
  // すべての変数 x[0..n-1] を同時最大化する解を構成して返す。
  // 矛盾または上界なし (未到達) の変数が存在する場合は nullopt を返すが、
  // 到達可能頂点の最短距離はそのまま dist(i) / dists() / is_reachable(i) で参照可能。
  optional<vector<T>> solve_max(int base = -1, bool use_spfa = false) {
    assert(-1 <= base && base < n_);
    int s = (base == -1) ? n_ : base;

    vector<T> p;
    vector<uint8_t> p_reach;
    vector<int> p_par;
    int cyc_node = -1;
    Algorithm feas_algo = NONE;
    if (!run_all_sources_core(use_spfa, p, p_reach, p_par, cyc_node,
                              feas_algo)) {
      dist_ = std::move(p);
      reached_ = std::move(p_reach);
      parent_ = std::move(p_par);
      last_algo_ = feas_algo;
      last_cycle_node_ = cyc_node;
      last_source_ = -1;
      last_reversed_ = false;
      last_status_ = INFEASIBLE;
      solution_.clear();
      return nullopt;
    }

    run_sssp(s, false, use_spfa, dist_, reached_, parent_, last_algo_);
    last_source_ = s;
    last_reversed_ = false;
    last_cycle_node_ = -1;

    for (int i = 0; i < n_; ++i) {
      if (!reached_[i]) {
        last_status_ = UNBOUNDED;
        solution_.clear();
        return nullopt;
      }
    }

    last_status_ = FINITE;
    solution_.resize(n_);
    for (int i = 0; i < n_; ++i) {
      solution_[i] = dist_[i];
    }
    return solution_;
  }

  // 基準頂点 base (x_base = 0; base == -1 のときは絶対値基準 x_zero = 0) のもとで
  // すべての変数 x[0..n-1] を同時最小化 (反転グラフで最短路を解き x[i] = -dist[i]) する解を返す。
  // 矛盾または下界なし (反転グラフで未到達) の変数が存在する場合は nullopt を返すが、
  // 到達可能頂点の反転グラフ上最短距離は dist(i) / dists() / is_reachable(i) で参照可能。
  optional<vector<T>> solve_min(int base = -1, bool use_spfa = false) {
    assert(-1 <= base && base < n_);
    int s = (base == -1) ? n_ : base;

    vector<T> p;
    vector<uint8_t> p_reach;
    vector<int> p_par;
    int cyc_node = -1;
    Algorithm feas_algo = NONE;
    if (!run_all_sources_core(use_spfa, p, p_reach, p_par, cyc_node,
                              feas_algo)) {
      dist_ = std::move(p);
      reached_ = std::move(p_reach);
      parent_ = std::move(p_par);
      last_algo_ = feas_algo;
      last_cycle_node_ = cyc_node;
      last_source_ = -1;
      last_reversed_ = false;
      last_status_ = INFEASIBLE;
      solution_.clear();
      return nullopt;
    }

    run_sssp(s, true, use_spfa, dist_, reached_, parent_, last_algo_);
    last_source_ = s;
    last_reversed_ = true;
    last_cycle_node_ = -1;

    for (int i = 0; i < n_; ++i) {
      if (!reached_[i]) {
        last_status_ = UNBOUNDED;
        solution_.clear();
        return nullopt;
      }
    }

    last_status_ = FINITE;
    solution_.resize(n_);
    for (int i = 0; i < n_; ++i) {
      solution_[i] = -dist_[i];
    }
    return solution_;
  }

  // 矛盾の原因となっている負閉路を返す (なければ空配列、頂点 n は基準変数 x_zero)
  vector<int> get_contradiction_cycle() {
    if (is_feasible())
      return {};
    int total = n_ + 1;
    int x = last_cycle_node_;
    if (x == -1)
      return {};
    for (int i = 0; i < total; ++i) {
      x = parent_[x];
      if (x == -1)
        return {};
    }
    vector<int> cyc;
    int cur = x;
    do {
      cyc.push_back(cur);
      cur = parent_[cur];
      if (cur == -1)
        return {};
    } while (cur != x && (int)cyc.size() <= total);
    if (cur != x)
      return {};
    cyc.push_back(x);
    reverse(cyc.begin(), cyc.end());
    return cyc;
  }

  // 制約のもとでの (x_u - x_v) の最大値を求める (POJ 3169 Layout など)
  // 同時に solution() でその最大値を達成する完全解 x[0..n-1]、
  // dists() で最短経路計算の生距離 (基準 v からの距離) を取得可能になる。
  BoundResult max_diff(int u, int v, bool use_spfa = false) {
    assert(0 <= u && u < n_ && 0 <= v && v < n_);
    vector<T> p;
    vector<uint8_t> p_reach;
    vector<int> p_par;
    int cyc_node = -1;
    Algorithm feas_algo = NONE;
    if (!run_all_sources_core(use_spfa, p, p_reach, p_par, cyc_node,
                              feas_algo)) {
      dist_ = std::move(p);
      reached_ = std::move(p_reach);
      parent_ = std::move(p_par);
      last_algo_ = feas_algo;
      last_cycle_node_ = cyc_node;
      last_source_ = -1;
      last_reversed_ = false;
      last_status_ = INFEASIBLE;
      solution_.clear();
      return {INFEASIBLE, 0};
    }

    run_sssp(v, false, use_spfa, dist_, reached_, parent_, last_algo_);
    last_source_ = v;
    last_reversed_ = false;
    last_cycle_node_ = -1;

    if (!reached_[u]) {
      last_status_ = UNBOUNDED;
      solution_.clear();
      return {UNBOUNDED, 0};
    }

    build_completed_solution(dist_, reached_, p);
    last_status_ = FINITE;
    return {FINITE, dist_[u]};
  }

  // 制約のもとでの (x_u - x_v) の最小値を求める: min(x_u - x_v) = -max(x_v - x_u)
  // 同時に solution() でその最小値を達成する完全解 x[0..n-1]、
  // dists() で最短経路計算の生距離 (基準 u からの距離) を取得可能になる。
  BoundResult min_diff(int u, int v, bool use_spfa = false) {
    BoundResult res = max_diff(v, u, use_spfa);
    if (res.status == FINITE) {
      res.value = -res.value;
    }
    return res;
  }

  // --- 直近に実行した解・距離・構成パスの参照 ---
  Status last_status() const { return last_status_; }
  Algorithm last_algorithm() const { return last_algo_; }

  // 直近の solve / solve_min / solve_max / max_diff / min_diff で構成した変数配列 x[0..n-1]
  const vector<T> &solution() const { return solution_; }

  // 直近の最短経路計算における頂点 v (0 <= v <= n, n は x_zero) までの生距離 (未到達は INF)
  T dist(int v) const {
    assert(0 <= v && v <= n_);
    assert(!dist_.empty());
    return dist_[v];
  }

  const vector<T> &dists() const { return dist_; }

  bool is_reachable(int v) const {
    assert(0 <= v && v <= n_);
    return !reached_.empty() && reached_[v] != 0;
  }

  optional<T> get_dist_optional(int v) const {
    if (!is_reachable(v))
      return nullopt;
    return dist_[v];
  }

  // 直近の最短経路計算において target までの距離 (制約の上界/下界) を決定したパス列を復元
  vector<int> get_path(int target) const {
    if (!is_reachable(target))
      return {};
    int total = n_ + 1;
    vector<int> path;
    int cur = target;
    int steps = 0;
    while (cur != -1 && steps <= total) {
      path.push_back(cur);
      cur = parent_[cur];
      steps++;
    }
    if (cur != -1)
      return {};
    reverse(path.begin(), path.end());
    return path;
  }
};
