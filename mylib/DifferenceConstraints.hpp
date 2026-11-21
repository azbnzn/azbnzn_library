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
  using CalcT =
      conditional_t<is_integral_v<T> && sizeof(T) <= 8, __int128_t, T>;
  static constexpr CalcT CALC_INF = dc_internal::Limits<CalcT>::max_val() / 2;
  static constexpr CalcT CALC_NEG_INF =
      dc_internal::Limits<CalcT>::min_val() / 2;

  int n_; // 頂点 0..n-1 が変数、頂点 n が基準変数 x_zero (= 0)
  T eps_;
  bool has_negative_edge_ = false;
  vector<Edge> edges_;
  vector<CalcT> edge_costs_;
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
  bool last_overflow_ = false;

  T clamp_to_T(CalcT val, bool &overflowed) const {
    if constexpr (is_integral_v<T> && sizeof(T) <= 8) {
      constexpr CalcT max_val = (CalcT)dc_internal::Limits<T>::max_val();
      constexpr CalcT min_val = (CalcT)dc_internal::Limits<T>::min_val();
      if (val > max_val) {
        overflowed = true;
        return dc_internal::Limits<T>::max_val();
      }
      if (val < min_val) {
        overflowed = true;
        return dc_internal::Limits<T>::min_val();
      }
      return (T)val;
    } else {
      return val;
    }
  }

  void add_edge_internal(int from, int to, CalcT cost) {
    assert(0 <= from && from <= n_ && 0 <= to && to <= n_);
    if (cost < (CalcT)0)
      has_negative_edge_ = true;
    bool unused = false;
    T stored_cost = clamp_to_T(cost, unused);
    int id = (int)edges_.size();
    edges_.push_back({from, to, stored_cost});
    edge_costs_.push_back(cost);
    adj_[from].push_back({to, id});
    rev_adj_[to].push_back({from, id});
  }

  pair<CalcT, bool> add_clamped_calc(CalcT d, CalcT cost) const {
    if constexpr (is_integral_v<T> || is_same_v<CalcT, __int128_t>) {
      if (cost < 0 && d < CALC_NEG_INF - cost)
        return {CALC_NEG_INF, true};
      if (cost > 0 && d > CALC_INF - cost)
        return {CALC_INF, true};
      CalcT res = d + cost;
      if (res <= CALC_NEG_INF)
        return {CALC_NEG_INF, true};
      if (res >= CALC_INF)
        return {CALC_INF, true};
      return {res, false};
    } else {
      CalcT res = d + cost;
      if (res <= CALC_NEG_INF)
        return {CALC_NEG_INF, true};
      if (res >= CALC_INF)
        return {CALC_INF, true};
      return {res, false};
    }
  }

  bool is_better_calc(CalcT cand, CalcT cur) const {
    return cand < cur - (CalcT)eps_;
  }

  void copy_dists_to_T(const vector<CalcT> &calc_dist,
                       const vector<uint8_t> &reached) {
    int total = n_ + 1;
    dist_.assign(total, INF);
    for (int i = 0; i < total; ++i) {
      if (reached[i]) {
        if (calc_dist[i] <= CALC_NEG_INF) {
          dist_[i] = NEG_INF;
        } else if (calc_dist[i] >= CALC_INF) {
          dist_[i] = INF;
        } else {
          dist_[i] = clamp_to_T(calc_dist[i], last_overflow_);
        }
      }
    }
  }

  // 全頂点 0..n_ から一斉開始する実行可能性判定 (ポテンシャル p も生成)
  bool run_all_sources_core(bool use_spfa, vector<CalcT> &out_dist,
                            vector<uint8_t> &out_reached,
                            vector<int> &out_parent,
                            int &out_cycle_node, Algorithm &out_algo) const {
    int total = n_ + 1;
    out_dist.assign(total, (CalcT)0);
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
          auto [cand, clamped] = add_clamped_calc(out_dist[u], edge_costs_[eid]);
          if (clamped && cand <= CALC_NEG_INF) {
            out_dist[v] = CALC_NEG_INF;
            out_parent[v] = u;
            out_cycle_node = v;
            return false;
          }
          if (is_better_calc(cand, out_dist[v])) {
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
      out_dist.assign(total, (CalcT)0);
      out_parent.assign(total, -1);
    }

    out_algo = BELLMAN_FORD;
    for (int it = 0; it < total; ++it) {
      bool updated = false;
      for (size_t eid = 0; eid < edges_.size(); ++eid) {
        const auto &e = edges_[eid];
        auto [cand, clamped] = add_clamped_calc(out_dist[e.from], edge_costs_[eid]);
        if (clamped && cand <= CALC_NEG_INF) {
          out_dist[e.to] = CALC_NEG_INF;
          out_parent[e.to] = e.from;
          out_cycle_node = e.to;
          return false;
        }
        if (is_better_calc(cand, out_dist[e.to])) {
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
  void run_sssp(int start, bool reversed, bool use_spfa, vector<CalcT> &out_dist,
                vector<uint8_t> &out_reached, vector<int> &out_parent,
                bool &out_clamped, Algorithm &out_algo) const {
    int total = n_ + 1;
    out_dist.assign(total, CALC_INF);
    out_reached.assign(total, 0);
    out_parent.assign(total, -1);
    out_clamped = false;

    const auto &g = reversed ? rev_adj_ : adj_;
    out_dist[start] = 0;
    out_reached[start] = 1;
    vector<uint8_t> node_clamped(total, 0);

    if (!has_negative_edge_) {
      out_algo = DIJKSTRA;
      using P = pair<CalcT, int>;
      priority_queue<P, vector<P>, greater<P>> pq;
      pq.push({(CalcT)0, start});
      while (!pq.empty()) {
        auto [c, u] = pq.top();
        pq.pop();
        if (c > out_dist[u])
          continue;
        for (auto [v, eid] : g[u]) {
          auto [nc, clamped] = add_clamped_calc(c, edge_costs_[eid]);
          if (!out_reached[v] || is_better_calc(nc, out_dist[v])) {
            out_reached[v] = 1;
            out_dist[v] = nc;
            out_parent[v] = u;
            node_clamped[v] = clamped ? 1 : 0;
            pq.push({nc, v});
          }
        }
      }
      for (int i = 0; i < total; ++i) {
        if (out_reached[i] && node_clamped[i])
          out_clamped = true;
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
          auto [nc, clamped] = add_clamped_calc(out_dist[u], edge_costs_[eid]);
          if (!out_reached[v] || is_better_calc(nc, out_dist[v])) {
            out_reached[v] = 1;
            out_dist[v] = nc;
            out_parent[v] = u;
            node_clamped[v] = clamped ? 1 : 0;
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
      if (q.empty()) {
        for (int i = 0; i < total; ++i) {
          if (out_reached[i] && node_clamped[i])
            out_clamped = true;
        }
        return;
      }
      out_dist.assign(total, CALC_INF);
      out_reached.assign(total, 0);
      out_parent.assign(total, -1);
      node_clamped.assign(total, 0);
      out_dist[start] = 0;
      out_reached[start] = 1;
    }

    out_algo = BELLMAN_FORD;
    for (int it = 0; it < total; ++it) {
      bool updated = false;
      for (size_t eid = 0; eid < edges_.size(); ++eid) {
        const auto &e = edges_[eid];
        int u = reversed ? e.to : e.from;
        int v = reversed ? e.from : e.to;
        if (!out_reached[u])
          continue;
        auto [nc, clamped] = add_clamped_calc(out_dist[u], edge_costs_[eid]);
        if (!out_reached[v] || is_better_calc(nc, out_dist[v])) {
          out_reached[v] = 1;
          out_dist[v] = nc;
          out_parent[v] = u;
          node_clamped[v] = clamped ? 1 : 0;
          updated = true;
        }
      }
      if (!updated)
        break;
    }
    for (int i = 0; i < total; ++i) {
      if (out_reached[i] && node_clamped[i])
        out_clamped = true;
    }
  }

  // 到達可能部の最短路 d と全体ポテンシャル p を合成し、全変数が制約を満たす完全解を作る
  void build_completed_solution(const vector<CalcT> &d,
                                const vector<uint8_t> &reached,
                                const vector<CalcT> &p) {
    bool has_c = false;
    CalcT shift_c = 0;
    for (size_t eid = 0; eid < edges_.size(); ++eid) {
      int u = edges_[eid].from;
      int v = edges_[eid].to;
      if (!reached[u] && reached[v]) {
        CalcT cand = d[v] - p[u] - edge_costs_[eid];
        if (!has_c || cand > shift_c) {
          has_c = true;
          shift_c = cand;
        }
      }
    }
    vector<CalcT> x_full(n_ + 1);
    for (int i = 0; i <= n_; ++i) {
      x_full[i] = reached[i] ? d[i] : (p[i] + shift_c);
    }
    CalcT zero_off = x_full[n_];
    solution_.resize(n_);
    for (int i = 0; i < n_; ++i) {
      solution_[i] = clamp_to_T(x_full[i] - zero_off, last_overflow_);
    }
  }

  // 反転グラフ最短路 d_rev と全体ポテンシャル p を合成し、全変数が制約を満たす完全解を作る
  void build_completed_solution_reversed(const vector<CalcT> &d_rev,
                                         const vector<uint8_t> &reached_rev,
                                         const vector<CalcT> &p) {
    vector<CalcT> neg_d(n_ + 1, 0);
    for (int i = 0; i <= n_; ++i) {
      if (reached_rev[i])
        neg_d[i] = -d_rev[i];
    }
    bool has_c = false;
    CalcT shift_c = 0;
    for (size_t eid = 0; eid < edges_.size(); ++eid) {
      int u = edges_[eid].from;
      int v = edges_[eid].to;
      if (reached_rev[u] && !reached_rev[v]) {
        CalcT cand = edge_costs_[eid] + neg_d[u] - p[v];
        if (!has_c || cand < shift_c) {
          has_c = true;
          shift_c = cand;
        }
      }
    }
    vector<CalcT> x_full(n_ + 1);
    for (int i = 0; i <= n_; ++i) {
      x_full[i] = reached_rev[i] ? neg_d[i] : (p[i] + shift_c);
    }
    CalcT zero_off = x_full[n_];
    solution_.resize(n_);
    for (int i = 0; i < n_; ++i) {
      solution_[i] = clamp_to_T(x_full[i] - zero_off, last_overflow_);
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
  bool has_overflow() const { return last_overflow_; }
  const vector<Edge> &edges() const { return edges_; }

  void clear(int new_n = -1) {
    if (new_n >= 0) {
      assert(new_n >= 0);
      n_ = new_n;
    }
    edges_.clear();
    edge_costs_.clear();
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
    last_overflow_ = false;
  }

  // --- 制約の追加 ---
  // x_u - x_v <= w
  void add_diff_leq(int u, int v, T w) {
    assert(0 <= u && u < n_ && 0 <= v && v < n_);
    add_edge_internal(v, u, (CalcT)w);
  }

  // x_u - x_v >= w  <=>  x_v - x_u <= -w
  void add_diff_geq(int u, int v, T w) {
    assert(0 <= u && u < n_ && 0 <= v && v < n_);
    add_edge_internal(u, v, -(CalcT)w);
  }

  // x_u - x_v < w (整数型では x_u - x_v <= w - 1)
  void add_diff_lt(int u, int v, T w) {
    static_assert(is_integral_v<T>, "add_diff_lt requires integral type");
    assert(0 <= u && u < n_ && 0 <= v && v < n_);
    add_edge_internal(v, u, (CalcT)w - 1);
  }

  // x_u - x_v > w (整数型では x_u - x_v >= w + 1)
  void add_diff_gt(int u, int v, T w) {
    static_assert(is_integral_v<T>, "add_diff_gt requires integral type");
    assert(0 <= u && u < n_ && 0 <= v && v < n_);
    add_edge_internal(u, v, -((CalcT)w + 1));
  }

  // x_u - x_v == w
  void add_diff_eq(int u, int v, T w) {
    add_diff_leq(u, v, w);
    add_diff_geq(u, v, w);
  }

  // x_u <= w  <=>  x_u - x_zero <= w
  void add_bound_leq(int u, T w) {
    assert(0 <= u && u < n_);
    add_edge_internal(n_, u, (CalcT)w);
  }

  // x_u >= w  <=>  x_zero - x_u <= -w
  void add_bound_geq(int u, T w) {
    assert(0 <= u && u < n_);
    add_edge_internal(u, n_, -(CalcT)w);
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
    last_overflow_ = false;
    vector<CalcT> calc_dist;
    int cyc_node = -1;
    bool ok = run_all_sources_core(use_spfa, calc_dist, reached_, parent_,
                                   cyc_node, last_algo_);
    copy_dists_to_T(calc_dist, reached_);
    last_source_ = -1;
    last_reversed_ = false;
    last_cycle_node_ = cyc_node;
    if (!ok) {
      last_status_ = INFEASIBLE;
      solution_.clear();
      return false;
    }
    last_status_ = FINITE;
    CalcT zero_val = calc_dist[n_];
    solution_.resize(n_);
    for (int i = 0; i < n_; ++i) {
      solution_[i] = clamp_to_T(calc_dist[i] - zero_val, last_overflow_);
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
    last_overflow_ = false;
    int s = (base == -1) ? n_ : base;

    vector<CalcT> p;
    vector<uint8_t> p_reach;
    vector<int> p_par;
    int cyc_node = -1;
    Algorithm feas_algo = NONE;
    if (!run_all_sources_core(use_spfa, p, p_reach, p_par, cyc_node,
                              feas_algo)) {
      reached_ = std::move(p_reach);
      parent_ = std::move(p_par);
      copy_dists_to_T(p, reached_);
      last_algo_ = feas_algo;
      last_cycle_node_ = cyc_node;
      last_source_ = -1;
      last_reversed_ = false;
      last_status_ = INFEASIBLE;
      solution_.clear();
      return nullopt;
    }

    vector<CalcT> calc_dist;
    bool clamped = false;
    run_sssp(s, false, use_spfa, calc_dist, reached_, parent_, clamped,
             last_algo_);
    if (clamped)
      last_overflow_ = true;
    copy_dists_to_T(calc_dist, reached_);
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
    build_completed_solution(calc_dist, reached_, p);
    return solution_;
  }

  // 基準頂点 base (x_base = 0; base == -1 のときは絶対値基準 x_zero = 0) のもとで
  // すべての変数 x[0..n-1] を同時最小化 (反転グラフで最短路を解き x[i] = -dist[i]) する解を返す。
  // 矛盾または下界なし (反転グラフで未到達) の変数が存在する場合は nullopt を返すが、
  // 到達可能頂点の反転グラフ上最短距離は dist(i) / dists() / is_reachable(i) で参照可能。
  optional<vector<T>> solve_min(int base = -1, bool use_spfa = false) {
    assert(-1 <= base && base < n_);
    last_overflow_ = false;
    int s = (base == -1) ? n_ : base;

    vector<CalcT> p;
    vector<uint8_t> p_reach;
    vector<int> p_par;
    int cyc_node = -1;
    Algorithm feas_algo = NONE;
    if (!run_all_sources_core(use_spfa, p, p_reach, p_par, cyc_node,
                              feas_algo)) {
      reached_ = std::move(p_reach);
      parent_ = std::move(p_par);
      copy_dists_to_T(p, reached_);
      last_algo_ = feas_algo;
      last_cycle_node_ = cyc_node;
      last_source_ = -1;
      last_reversed_ = false;
      last_status_ = INFEASIBLE;
      solution_.clear();
      return nullopt;
    }

    vector<CalcT> calc_dist;
    bool clamped = false;
    run_sssp(s, true, use_spfa, calc_dist, reached_, parent_, clamped,
             last_algo_);
    if (clamped)
      last_overflow_ = true;
    copy_dists_to_T(calc_dist, reached_);
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
    build_completed_solution_reversed(calc_dist, reached_, p);
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
    last_overflow_ = false;
    vector<CalcT> p;
    vector<uint8_t> p_reach;
    vector<int> p_par;
    int cyc_node = -1;
    Algorithm feas_algo = NONE;
    if (!run_all_sources_core(use_spfa, p, p_reach, p_par, cyc_node,
                              feas_algo)) {
      reached_ = std::move(p_reach);
      parent_ = std::move(p_par);
      copy_dists_to_T(p, reached_);
      last_algo_ = feas_algo;
      last_cycle_node_ = cyc_node;
      last_source_ = -1;
      last_reversed_ = false;
      last_status_ = INFEASIBLE;
      solution_.clear();
      return {INFEASIBLE, 0};
    }

    vector<CalcT> calc_dist;
    bool clamped = false;
    run_sssp(v, false, use_spfa, calc_dist, reached_, parent_, clamped,
             last_algo_);
    if (clamped)
      last_overflow_ = true;
    copy_dists_to_T(calc_dist, reached_);
    last_source_ = v;
    last_reversed_ = false;
    last_cycle_node_ = -1;

    if (!reached_[u]) {
      last_status_ = UNBOUNDED;
      solution_.clear();
      return {UNBOUNDED, 0};
    }

    build_completed_solution(calc_dist, reached_, p);
    last_status_ = FINITE;
    T ans_val = clamp_to_T(calc_dist[u], last_overflow_);
    return {FINITE, ans_val};
  }

  // 制約のもとでの (x_u - x_v) の最小値を求める: min(x_u - x_v) = -max(x_v - x_u)
  // 同時に solution() でその最小値を達成する完全解 x[0..n-1]、
  // dists() で最短経路計算の生距離 (基準 u からの距離) を取得可能になる。
  BoundResult min_diff(int u, int v, bool use_spfa = false) {
    assert(0 <= u && u < n_ && 0 <= v && v < n_);
    last_overflow_ = false;
    vector<CalcT> p;
    vector<uint8_t> p_reach;
    vector<int> p_par;
    int cyc_node = -1;
    Algorithm feas_algo = NONE;
    if (!run_all_sources_core(use_spfa, p, p_reach, p_par, cyc_node,
                              feas_algo)) {
      reached_ = std::move(p_reach);
      parent_ = std::move(p_par);
      copy_dists_to_T(p, reached_);
      last_algo_ = feas_algo;
      last_cycle_node_ = cyc_node;
      last_source_ = -1;
      last_reversed_ = false;
      last_status_ = INFEASIBLE;
      solution_.clear();
      return {INFEASIBLE, 0};
    }

    vector<CalcT> calc_dist;
    bool clamped = false;
    run_sssp(u, false, use_spfa, calc_dist, reached_, parent_, clamped,
             last_algo_);
    if (clamped)
      last_overflow_ = true;
    copy_dists_to_T(calc_dist, reached_);
    last_source_ = u;
    last_reversed_ = false;
    last_cycle_node_ = -1;

    if (!reached_[v]) {
      last_status_ = UNBOUNDED;
      solution_.clear();
      return {UNBOUNDED, 0};
    }

    build_completed_solution(calc_dist, reached_, p);
    last_status_ = FINITE;
    T ans_val = clamp_to_T(-calc_dist[v], last_overflow_);
    return {FINITE, ans_val};
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
