#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numeric>
#include <optional>
#include <queue>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// 多機能ベルマンフォード法・SPFA・差分制約系・最小平均閉路ライブラリ (BellmanFord)
// ============================================================================
// 【概要】
// 負の重みを持つ有向/無向グラフに対する単一始点・多始点最短経路、負閉路検出と復元、
// 到達可能な負閉路の影響を受ける頂点 (-INF) の厳密判定、最長経路 (+INF 判定対応)、
// SPFA、辺数制約付き最短経路、最小/最大平均重み閉路 (Karp のアルゴリズム)、
// Johnson のポテンシャル計算・全点対最短経路 (APSP)、差分制約系 (牛ゲー) を
// ひとつのヘッダで網羅的に提供します。
//
// 【主な機能】
// 1. BellmanFord<T> (デフォルト T = long long):
//    - run(start) / run(starts) / run_all_sources() / run_with_init(init_dist)
//    - run_max(start) / run_max(starts) (最長経路 / 最大スコア、+INF 判定対応)
//    - run_spfa(...) / run_spfa_max(...) (FIFOキュー高速化・O(VE)最悪保証)
//    - 厳密な状態管理: is_reachable(v), is_neg_inf(v), is_pos_inf(v), has_finite_dist(v)
//    - 最短/最長経路の復元: get_path(t), get_path_edges(t)
//    - 負閉路/正閉路の検出と単純閉路の復元: get_cycle(), get_cycle_edges(),
//      find_negative_cycle(start), find_any_negative_cycle()
//    - 辺数制限最短・最長路:
//      shortest_path_at_most_k_edges(start, k), shortest_path_exact_k_edges(start, k)
//    - 最小/最大平均重み閉路 (Karp の O(VE) アルゴリズム + 整数誤差なし既約分数 & 閉路復元):
//      min_mean_cycle(), max_mean_cycle()
//    - グラフ反転・ポテンシャル・全点対最短路:
//      reversed(), compute_potentials(), all_pairs_shortest_path()
//
// 2. DifferenceConstraints<T> (差分制約系 / 牛ゲー):
//    - x_u - x_v <= w, >= w, < w, > w, == w や絶対範囲 low <= x_u <= high を記述可能
//    - solve(), is_feasible(), get_contradiction_cycle(), max_diff(u, v), min_diff(u, v)
//
// 3. 簡易関数:
//    - bellman_ford(g, start): 隣接リストから即座に実行済みの BellmanFord を返す
// ============================================================================

namespace bf_internal {

template <typename T> struct Limits {
  static constexpr T max_val() {
    if constexpr (is_same_v<T, __int128_t>) {
      return (T)((((unsigned __int128)1) << 126) - 1 + (((unsigned __int128)1) << 126));
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

template <typename T> inline T abs_val(T x) { return x < 0 ? -x : x; }

template <typename T> inline T gcd_val(T a, T b) {
  a = abs_val(a);
  b = abs_val(b);
  while (b != 0) {
    a %= b;
    swap(a, b);
  }
  return a;
}

} // namespace bf_internal

template <typename T = long long> struct BellmanFord {
  static constexpr T INF = bf_internal::Limits<T>::max_val() / 2;
  static constexpr T NEG_INF = bf_internal::Limits<T>::min_val() / 2;

  enum VertexState : uint8_t {
    UNREACHABLE = 0,
    FINITE = 1,
    UNBOUNDED = 2, // 最小化時は -INF、最大化時は +INF
  };

  struct Edge {
    int from;
    int to;
    T cost;
    int id;
  };

  struct MeanCycleResult {
    bool has_cycle = false;
    long double value = 0; // 平均重み (少数)
    T num = 0;             // 整数型時の既約分数分子 (num / den)
    int den = 1;           // 整数型時の既約分数分母 (> 0)
    vector<int> cycle;       // 単純閉路の頂点列 [v_0, v_1, ..., v_k(=v_0)]
    vector<int> cycle_edges; // 単純閉路の辺 ID 列 [e_0, ..., e_{k-1}]
  };

private:
  int n_;
  T eps_;
  vector<Edge> edges_;
  vector<vector<int>> adj_; // adj_[u] = edge indices from u

  // 直近の実行結果
  vector<T> dist_;
  vector<VertexState> state_;
  vector<int> parent_;
  vector<int> parent_edge_;
  bool has_unbounded_ = false;
  bool last_maximize_ = false;
  int last_cycle_node_ = -1;
  int last_clamped_node_ = -1;

  // オーバーフロー安全な加算・クランプ判定
  pair<T, bool> add_clamped(T d, T cost) const {
    if constexpr (is_integral_v<T> && sizeof(T) <= 8) {
      __int128 res = (__int128)d + (__int128)cost;
      if (res <= (__int128)NEG_INF) return {NEG_INF, true};
      if (res >= (__int128)INF) return {INF, true};
      return {(T)res, false};
    } else if constexpr (is_same_v<T, __int128_t>) {
      if (cost < 0 && d < NEG_INF - cost) return {NEG_INF, true};
      if (cost > 0 && d > INF - cost) return {INF, true};
      T res = d + cost;
      if (res <= NEG_INF) return {NEG_INF, true};
      if (res >= INF) return {INF, true};
      return {res, false};
    } else {
      T res = d + cost;
      if (res <= NEG_INF) return {NEG_INF, true};
      if (res >= INF) return {INF, true};
      return {res, false};
    }
  }

  bool is_better(T cand, T cur, bool maximize) const {
    if (maximize) {
      return cand > cur + eps_;
    } else {
      return cand < cur - eps_;
    }
  }

  void reset_result(bool maximize) {
    last_maximize_ = maximize;
    T unreach_val = maximize ? NEG_INF : INF;
    dist_.assign(n_, unreach_val);
    state_.assign(n_, UNREACHABLE);
    parent_.assign(n_, -1);
    parent_edge_.assign(n_, -1);
    has_unbounded_ = false;
    last_cycle_node_ = -1;
    last_clamped_node_ = -1;
  }

  // seed 配列の頂点から到達可能なすべての頂点へ UNBOUNDED (-INF / +INF) を伝播
  void propagate_unbounded_bfs(const vector<bool> &seeds, bool maximize) {
    T unb_val = maximize ? INF : NEG_INF;
    queue<int> q;
    for (int i = 0; i < n_; ++i) {
      if (seeds[i]) {
        state_[i] = UNBOUNDED;
        dist_[i] = unb_val;
        q.push(i);
      }
    }
    while (!q.empty()) {
      int u = q.front();
      q.pop();
      for (int eid : adj_[u]) {
        int v = edges_[eid].to;
        if (state_[v] != UNBOUNDED) {
          state_[v] = UNBOUNDED;
          dist_[v] = unb_val;
          q.push(v);
        }
      }
    }
  }

  // 決定論的ベルマンフォード本体 O(VE)
  bool run_core(const vector<T> &init_dist, bool maximize) {
    assert((int)init_dist.size() == n_);
    reset_result(maximize);
    if (n_ == 0) return true;

    T unreach_val = maximize ? NEG_INF : INF;
    for (int i = 0; i < n_; ++i) {
      if (init_dist[i] != unreach_val) {
        dist_[i] = init_dist[i];
        state_[i] = FINITE;
      }
    }

    vector<bool> neg_seed(n_, false);

    for (int it = 0; it < n_; ++it) {
      bool updated = false;
      for (const auto &e : edges_) {
        if (state_[e.from] == UNREACHABLE) continue;
        auto [cand, clamped] = add_clamped(dist_[e.from], e.cost);

        if (clamped) {
          bool hit_unb_dir = maximize ? (cand >= INF) : (cand <= NEG_INF);
          if (hit_unb_dir) {
            bool can_relax = (state_[e.to] == UNREACHABLE) || !neg_seed[e.to];
            if (can_relax) {
              dist_[e.to] = cand;
              state_[e.to] = FINITE;
              if (!neg_seed[e.to]) {
                parent_[e.to] = e.from;
                parent_edge_[e.to] = e.id;
                last_clamped_node_ = e.to;
              }
              neg_seed[e.to] = true;
              has_unbounded_ = true;
              updated = true;
            }
            continue;
          }
        }

        if (state_[e.to] == UNREACHABLE || is_better(cand, dist_[e.to], maximize)) {
          dist_[e.to] = cand;
          state_[e.to] = FINITE;
          parent_[e.to] = e.from;
          parent_edge_[e.to] = e.id;
          updated = true;
          if (it == n_ - 1) {
            neg_seed[e.to] = true;
            has_unbounded_ = true;
            last_cycle_node_ = e.to;
          }
        }
      }
      if (it < n_ - 1 && !updated) {
        break;
      }
    }

    if (has_unbounded_) {
      propagate_unbounded_bfs(neg_seed, maximize);
      return false;
    }
    return true;
  }

  // SPFA (Shortest Path Faster Algorithm / 標準FIFOキューで最悪O(VE)保証) 本体
  bool run_spfa_core(const vector<T> &init_dist, bool maximize, bool find_all_unbounded) {
    assert((int)init_dist.size() == n_);
    reset_result(maximize);
    if (n_ == 0) return true;

    T unreach_val = maximize ? NEG_INF : INF;
    T unb_val = maximize ? INF : NEG_INF;

    queue<int> q;
    vector<bool> in_q(n_, false);
    vector<int> path_len(n_, 0);

    for (int i = 0; i < n_; ++i) {
      if (init_dist[i] != unreach_val) {
        dist_[i] = init_dist[i];
        state_[i] = FINITE;
        q.push(i);
        in_q[i] = true;
      }
    }
    if (q.empty()) return true;

    auto mark_subtree_unbounded = [&](int root) {
      queue<int> bq;
      state_[root] = UNBOUNDED;
      dist_[root] = unb_val;
      bq.push(root);
      while (!bq.empty()) {
        int cur = bq.front();
        bq.pop();
        for (int eid : adj_[cur]) {
          int nxt = edges_[eid].to;
          if (state_[nxt] != UNBOUNDED) {
            state_[nxt] = UNBOUNDED;
            dist_[nxt] = unb_val;
            bq.push(nxt);
          }
        }
      }
    };

    auto check_parent_cycle = [&](int v) -> pair<bool, int> {
      int cur = v;
      for (int step = 1; step <= n_; ++step) {
        cur = parent_[cur];
        if (cur == -1) return {false, step - 1};
      }
      return {true, n_};
    };

    int rounds = 0;
    int rem_in_round = (int)q.size();

    while (!q.empty()) {
      int u = q.front();
      q.pop();
      in_q[u] = false;

      if (state_[u] != UNBOUNDED) {
        for (int eid : adj_[u]) {
          const auto &e = edges_[eid];
          int v = e.to;
          if (state_[v] == UNBOUNDED) continue;

          auto [cand, clamped] = add_clamped(dist_[u], e.cost);
          bool hit_unb_clamp = clamped && (maximize ? (cand >= INF) : (cand <= NEG_INF));

          if (hit_unb_clamp) {
            dist_[v] = cand;
            parent_[v] = u;
            parent_edge_[v] = e.id;
            has_unbounded_ = true;
            if (last_clamped_node_ == -1) last_clamped_node_ = v;
            if (!find_all_unbounded) {
              state_[v] = UNBOUNDED;
              return false;
            }
            mark_subtree_unbounded(v);
            if (state_[u] == UNBOUNDED) break;
            continue;
          }

          if (state_[v] == UNREACHABLE || is_better(cand, dist_[v], maximize)) {
            dist_[v] = cand;
            state_[v] = FINITE;
            parent_[v] = u;
            parent_edge_[v] = e.id;
            path_len[v] = path_len[u] + 1;

            if (path_len[v] >= n_) {
              auto [has_cyc, actual_len] = check_parent_cycle(v);
              if (has_cyc) {
                has_unbounded_ = true;
                if (last_cycle_node_ == -1) last_cycle_node_ = v;
                if (!find_all_unbounded) {
                  state_[v] = UNBOUNDED;
                  dist_[v] = unb_val;
                  return false;
                }
                mark_subtree_unbounded(v);
                if (state_[u] == UNBOUNDED) break;
                continue;
              }
              path_len[v] = actual_len;
            }

            if (!in_q[v]) {
              q.push(v);
              in_q[v] = true;
            }
          }
        }
      }

      if (--rem_in_round == 0) {
        rounds++;
        if (!q.empty()) {
          if (rounds >= n_) {
            // 負閉路判定のラウンド上限に達した場合も O(VE) 決定論的処理で厳密完了
            return run_core(init_dist, maximize);
          }
          rem_in_round = (int)q.size();
        }
      }
    }

    return !has_unbounded_;
  }

  // 辺数制約付きウォーク (at_most_k / exact_k) 共通エンジン
  vector<T> walk_k_edges_core(const vector<int> &starts, int k, bool exact, bool maximize) const {
    assert(k >= 0);
    T unreach_val = maximize ? NEG_INF : INF;
    vector<T> dp(n_, unreach_val);
    vector<uint8_t> has(n_, 0);
    for (int s : starts) {
      assert(0 <= s && s < n_);
      dp[s] = 0;
      has[s] = 1;
    }

    for (int step = 0; step < k; ++step) {
      vector<T> ndp = exact ? vector<T>(n_, unreach_val) : dp;
      vector<uint8_t> nhas = exact ? vector<uint8_t>(n_, 0) : has;
      bool changed = false;

      for (const auto &e : edges_) {
        if (!has[e.from]) continue;
        auto [cand, _] = add_clamped(dp[e.from], e.cost);
        if (!nhas[e.to] || is_better(cand, ndp[e.to], maximize)) {
          ndp[e.to] = cand;
          nhas[e.to] = 1;
          changed = true;
        }
      }
      dp.swap(ndp);
      has.swap(nhas);
      if (!exact && !changed) break;
    }
    return dp;
  }

  // Karp のアルゴリズムによる最小/最大平均重み閉路
  MeanCycleResult mean_cycle_core(bool maximize, bool reconstruct_cycle) const {
    MeanCycleResult res;
    if (n_ == 0 || edges_.empty()) return res;

    using DP_T = conditional_t<is_integral_v<T> && sizeof(T) < 8, long long, T>;
    constexpr DP_T DP_INF = bf_internal::Limits<DP_T>::max_val() / 2;

    vector<DP_T> dp((size_t)(n_ + 1) * n_, DP_INF);
    for (int v = 0; v < n_; ++v) dp[v] = 0;

    for (int k = 1; k <= n_; ++k) {
      size_t prev_off = (size_t)(k - 1) * n_;
      size_t cur_off = (size_t)k * n_;
      for (const auto &e : edges_) {
        if (dp[prev_off + e.from] == DP_INF) continue;
        DP_T w = maximize ? (DP_T)(-e.cost) : (DP_T)e.cost;
        DP_T cand = dp[prev_off + e.from] + w;
        if (cand < dp[cur_off + e.to]) {
          dp[cur_off + e.to] = cand;
        }
      }
    }

    size_t last_off = (size_t)n_ * n_;
    bool found = false;

    if constexpr (is_integral_v<T>) {
      __int128 best_num = 0;
      int best_den = 1;

      for (int v = 0; v < n_; ++v) {
        if (dp[last_off + v] == DP_INF) continue;
        bool has_k = false;
        __int128 max_num = 0;
        int max_den = 1;

        for (int k = 0; k < n_; ++k) {
          size_t off = (size_t)k * n_;
          if (dp[off + v] == DP_INF) continue;
          __int128 num = (__int128)dp[last_off + v] - (__int128)dp[off + v];
          int den = n_ - k;
          if (!has_k || num * max_den > max_num * den) {
            has_k = true;
            max_num = num;
            max_den = den;
          }
        }
        if (has_k) {
          if (!found || max_num * best_den < best_num * max_den) {
            found = true;
            best_num = max_num;
            best_den = max_den;
          }
        }
      }

      if (!found) return res;
      res.has_cycle = true;
      __int128 g = bf_internal::gcd_val(best_num, (__int128)best_den);
      __int128 final_num = (maximize ? -best_num : best_num) / g;
      int final_den = (int)((__int128)best_den / g);
      res.num = (T)final_num;
      res.den = final_den;
      res.value = (long double)final_num / (long double)final_den;

      if (reconstruct_cycle) {
        // 元のグラフ（最小化側へ変換）の既約分数 (p / q) を用いて、
        // w'(e) = (q * w(e) - p) * (n + 1) - 1 と変換すると、
        // 平均重みがちょうど p/q の閉路のみが負閉路 (重み -L) となる
        __int128 p = best_num / g;
        __int128 q = (__int128)best_den / g;
        BellmanFord<__int128_t> bf_ext(n_);
        for (const auto &e : edges_) {
          __int128 w = maximize ? -(__int128)e.cost : (__int128)e.cost;
          __int128 nw = (q * w - p) * (n_ + 1) - 1;
          bf_ext.add_edge(e.from, e.to, nw);
        }
        if (!bf_ext.run_all_sources()) {
          res.cycle = bf_ext.get_cycle();
          res.cycle_edges = bf_ext.get_cycle_edges();
        }
      }
    } else {
      long double best_mean = 0;
      for (int v = 0; v < n_; ++v) {
        if (dp[last_off + v] == DP_INF) continue;
        bool has_k = false;
        long double max_val = 0;
        for (int k = 0; k < n_; ++k) {
          size_t off = (size_t)k * n_;
          if (dp[off + v] == DP_INF) continue;
          long double val = ((long double)dp[last_off + v] - (long double)dp[off + v]) / (n_ - k);
          if (!has_k || val > max_val) {
            has_k = true;
            max_val = val;
          }
        }
        if (has_k && (!found || max_val < best_mean)) {
          found = true;
          best_mean = max_val;
        }
      }

      if (!found) return res;
      res.has_cycle = true;
      res.value = maximize ? -best_mean : best_mean;

      if (reconstruct_cycle) {
        long double scale = max((long double)1.0, fabsl(best_mean));
        for (long double shift : {(long double)1e-11 * scale, (long double)1e-8 * scale, (long double)1e-5 * scale}) {
          BellmanFord<long double> bf_ext(n_);
          for (const auto &e : edges_) {
            long double w = maximize ? -(long double)e.cost : (long double)e.cost;
            bf_ext.add_edge(e.from, e.to, w - best_mean - shift);
          }
          if (!bf_ext.run_all_sources()) {
            res.cycle = bf_ext.get_cycle();
            res.cycle_edges = bf_ext.get_cycle_edges();
            if (!res.cycle.empty()) break;
          }
        }
      }
    }

    return res;
  }

public:
  explicit BellmanFord(int n = 0, T eps = 0)
      : n_(n), eps_(eps), adj_(n) {
    assert(n >= 0);
  }

  // 隣接リスト g[u] = {{v, cost}, ...} から構築
  explicit BellmanFord(const vector<vector<pair<int, T>>> &g, T eps = 0)
      : n_((int)g.size()), eps_(eps), adj_(g.size()) {
    for (int u = 0; u < n_; ++u) {
      for (const auto &p : g[u]) {
        add_edge(u, p.first, p.second);
      }
    }
  }

  // 辺リスト (from, to, cost) の配列から構築
  explicit BellmanFord(int n, const vector<tuple<int, int, T>> &edges, T eps = 0)
      : n_(n), eps_(eps), adj_(n) {
    assert(n >= 0);
    edges_.reserve(edges.size());
    for (const auto &e : edges) {
      add_edge(get<0>(e), get<1>(e), get<2>(e));
    }
  }

  // 有向辺の追加 (辺 ID を返す)
  int add_edge(int from, int to, T cost) {
    assert(0 <= from && from < n_);
    assert(0 <= to && to < n_);
    int id = (int)edges_.size();
    edges_.push_back({from, to, cost, id});
    adj_[from].push_back(id);
    return id;
  }

  // 無向辺の追加
  void add_undirected_edge(int u, int v, T cost) {
    add_edge(u, v, cost);
    add_edge(v, u, cost);
  }

  // 浮動小数点比較時の許容誤差を設定
  void set_eps(T eps) { eps_ = eps; }

  // すべての辺・結果を削除 (頂点数は維持または再設定)
  void clear(int new_n = -1) {
    if (new_n >= 0) n_ = new_n;
    edges_.clear();
    adj_.assign(n_, {});
    dist_.clear();
    state_.clear();
    parent_.clear();
    parent_edge_.clear();
    has_unbounded_ = false;
    last_cycle_node_ = -1;
    last_clamped_node_ = -1;
  }

  int num_vertices() const { return n_; }
  int num_edges() const { return (int)edges_.size(); }
  const vector<Edge> &edges() const { return edges_; }

  // すべての辺の向きを反転させたグラフを返す (終点固定最短路などに有用)
  BellmanFord<T> reversed() const {
    BellmanFord<T> rev(n_, eps_);
    for (const auto &e : edges_) {
      rev.add_edge(e.to, e.from, e.cost);
    }
    return rev;
  }

  // --- 最短経路 (最小化) 実行 ---
  // 戻り値: true = 始点から到達可能な負閉路なし / false = 負閉路あり
  bool run(int start = 0) {
    if (n_ == 0) return run_core({}, false);
    assert(0 <= start && start < n_);
    vector<T> init(n_, INF);
    init[start] = 0;
    return run_core(init, false);
  }

  // 多始点最短経路 (starts の各頂点を初期コスト 0 とする)
  bool run(const vector<int> &starts) {
    vector<T> init(n_, INF);
    for (int s : starts) {
      assert(0 <= s && s < n_);
      init[s] = 0;
    }
    return run_core(init, false);
  }

  // 全頂点を初期コスト 0 で実行 (グラフ全体の任意の負閉路検出 & ポテンシャル計算)
  bool run_all_sources() {
    vector<T> init(n_, 0);
    return run_core(init, false);
  }

  // 任意の初期距離配列を指定して実行 (INF は未到達扱い)
  bool run_with_init(const vector<T> &init_dist) {
    return run_core(init_dist, false);
  }

  // --- 最長経路 (最大化) 実行 ---
  // 戻り値: true = 到達可能な正閉路なし / false = 正閉路あり (+INF あり)
  bool run_max(int start = 0) {
    if (n_ == 0) return run_core({}, true);
    assert(0 <= start && start < n_);
    vector<T> init(n_, NEG_INF);
    init[start] = 0;
    return run_core(init, true);
  }

  bool run_max(const vector<int> &starts) {
    vector<T> init(n_, NEG_INF);
    for (int s : starts) {
      assert(0 <= s && s < n_);
      init[s] = 0;
    }
    return run_core(init, true);
  }

  bool run_max_with_init(const vector<T> &init_dist) {
    return run_core(init_dist, true);
  }

  // --- SPFA (Shortest Path Faster Algorithm) ---
  // find_all_unbounded = false: 負閉路/正閉路を検知した瞬間に早期終了し false を返す
  // find_all_unbounded = true : 閉路の影響を受けるすべての頂点を厳密にマークして継続する
  bool run_spfa(int start = 0, bool find_all_unbounded = false) {
    if (n_ == 0) return run_spfa_core({}, false, find_all_unbounded);
    assert(0 <= start && start < n_);
    vector<T> init(n_, INF);
    init[start] = 0;
    return run_spfa_core(init, false, find_all_unbounded);
  }

  bool run_spfa(const vector<int> &starts, bool find_all_unbounded = false) {
    vector<T> init(n_, INF);
    for (int s : starts) {
      assert(0 <= s && s < n_);
      init[s] = 0;
    }
    return run_spfa_core(init, false, find_all_unbounded);
  }

  bool run_spfa_all_sources(bool find_all_unbounded = false) {
    vector<T> init(n_, 0);
    return run_spfa_core(init, false, find_all_unbounded);
  }

  bool run_spfa_with_init(const vector<T> &init_dist, bool find_all_unbounded = false) {
    return run_spfa_core(init_dist, false, find_all_unbounded);
  }

  bool run_spfa_max(int start = 0, bool find_all_unbounded = false) {
    if (n_ == 0) return run_spfa_core({}, true, find_all_unbounded);
    assert(0 <= start && start < n_);
    vector<T> init(n_, NEG_INF);
    init[start] = 0;
    return run_spfa_core(init, true, find_all_unbounded);
  }

  // --- 結果取得 API ---
  bool has_negative_cycle() const { return !last_maximize_ && has_unbounded_; }
  bool has_positive_cycle() const { return last_maximize_ && has_unbounded_; }
  bool has_unbounded_cycle() const { return has_unbounded_; }

  bool is_reachable(int v) const {
    assert(0 <= v && v < n_);
    return !state_.empty() && state_[v] != UNREACHABLE;
  }

  bool is_unbounded(int v) const {
    assert(0 <= v && v < n_);
    return !state_.empty() && state_[v] == UNBOUNDED;
  }

  bool is_neg_inf(int v) const { return !last_maximize_ && is_unbounded(v); }
  bool is_pos_inf(int v) const { return last_maximize_ && is_unbounded(v); }

  bool has_finite_dist(int v) const {
    assert(0 <= v && v < n_);
    return !state_.empty() && state_[v] == FINITE;
  }

  T dist(int v) const {
    assert(0 <= v && v < n_);
    assert(!dist_.empty());
    return dist_[v];
  }

  T operator[](int v) const { return dist(v); }
  const vector<T> &dists() const { return dist_; }

  // 有限の最短/最長距離が存在する場合のみ値を返す (未到達・+-INF なら nullopt)
  optional<T> get_dist_optional(int v) const {
    if (!has_finite_dist(v)) return nullopt;
    return dist_[v];
  }

  // 負閉路/正閉路の影響を受ける (+-INF の) 全頂点リストを返す
  vector<int> get_unbounded_vertices() const {
    vector<int> res;
    for (int i = 0; i < n_; ++i) {
      if (!state_.empty() && state_[i] == UNBOUNDED) res.push_back(i);
    }
    return res;
  }

  vector<int> get_neg_inf_vertices() const { return get_unbounded_vertices(); }

  // --- 経路復元 ---
  // 頂点 target への最短/最長経路 [start, ..., target] を返す。
  // 未到達または +-INF の場合は空配列 {} を返す。
  vector<int> get_path(int target) const {
    if (!has_finite_dist(target)) return {};
    vector<int> path;
    int cur = target;
    int steps = 0;
    while (cur != -1 && steps <= n_) {
      path.push_back(cur);
      cur = parent_[cur];
      steps++;
    }
    if (cur != -1) return {};
    reverse(path.begin(), path.end());
    return path;
  }

  // 頂点 target への経路を構成する辺 ID の列を返す
  vector<int> get_path_edges(int target) const {
    if (!has_finite_dist(target)) return {};
    vector<int> path_edges;
    int cur = target;
    int steps = 0;
    while (cur != -1 && parent_[cur] != -1 && steps <= n_) {
      path_edges.push_back(parent_edge_[cur]);
      cur = parent_[cur];
      steps++;
    }
    if (steps > n_) return {};
    reverse(path_edges.begin(), path_edges.end());
    return path_edges;
  }

  // --- 負閉路 / 正閉路の単純閉路復元 ---
  // 直近の run / run_max / run_spfa で検出された単純閉路 [v_0, v_1, ..., v_k(=v_0)] を返す
  vector<int> get_cycle() const {
    int x = (last_cycle_node_ != -1) ? last_cycle_node_ : last_clamped_node_;
    if (x == -1) return {};
    for (int i = 0; i < n_; ++i) {
      x = parent_[x];
      if (x == -1) return {};
    }
    vector<int> cyc;
    int cur = x;
    do {
      cyc.push_back(cur);
      cur = parent_[cur];
      if (cur == -1) return {};
    } while (cur != x && (int)cyc.size() <= n_);
    if (cur != x) return {};
    cyc.push_back(x);
    reverse(cyc.begin(), cyc.end());
    return cyc;
  }

  // 直近で検出された単純閉路を構成する辺 ID 列 [e_0, ..., e_{k-1}] を返す
  vector<int> get_cycle_edges() const {
    int x = (last_cycle_node_ != -1) ? last_cycle_node_ : last_clamped_node_;
    if (x == -1) return {};
    for (int i = 0; i < n_; ++i) {
      x = parent_[x];
      if (x == -1) return {};
    }
    vector<int> edge_cyc;
    int cur = x;
    do {
      edge_cyc.push_back(parent_edge_[cur]);
      cur = parent_[cur];
      if (cur == -1) return {};
    } while (cur != x && (int)edge_cyc.size() <= n_);
    if (cur != x) return {};
    reverse(edge_cyc.begin(), edge_cyc.end());
    return edge_cyc;
  }

  // 始点 start から到達可能な負閉路を1つ見つけて返す (なければ空配列)
  vector<int> find_negative_cycle(int start = 0) {
    if (run(start)) return {};
    return get_cycle();
  }

  // グラフ全体から任意の負閉路を1つ見つけて返す (なければ空配列)
  vector<int> find_any_negative_cycle() {
    if (run_all_sources()) return {};
    return get_cycle();
  }

  // --- 辺数制約付き最短・最長経路 ---
  // 高々 k 本の辺を使って到達する最短距離 (負閉路があっても有限) O(k(V + E))
  vector<T> shortest_path_at_most_k_edges(int start, int k) const {
    return walk_k_edges_core({start}, k, false, false);
  }
  vector<T> shortest_path_at_most_k_edges(const vector<int> &starts, int k) const {
    return walk_k_edges_core(starts, k, false, false);
  }
  vector<T> longest_path_at_most_k_edges(int start, int k) const {
    return walk_k_edges_core({start}, k, false, true);
  }

  // ちょうど k 本の辺を使って到達する最短距離 (未到達は INF) O(k(V + E))
  vector<T> shortest_path_exact_k_edges(int start, int k) const {
    return walk_k_edges_core({start}, k, true, false);
  }
  vector<T> shortest_path_exact_k_edges(const vector<int> &starts, int k) const {
    return walk_k_edges_core(starts, k, true, false);
  }
  vector<T> longest_path_exact_k_edges(int start, int k) const {
    return walk_k_edges_core({start}, k, true, true);
  }

  // --- 最小平均重み閉路 / 最大平均重み閉路 (Karp's Algorithm) O(VE) ---
  MeanCycleResult min_mean_cycle(bool reconstruct_cycle = true) const {
    return mean_cycle_core(false, reconstruct_cycle);
  }

  MeanCycleResult max_mean_cycle(bool reconstruct_cycle = true) const {
    return mean_cycle_core(true, reconstruct_cycle);
  }

  // --- Johnson のポテンシャル & 全点対最短経路 (APSP) ---
  // すべての辺 (u, v, w) について w + h[u] - h[v] >= 0 となるポテンシャル h を計算
  // 負閉路が存在する場合は nullopt を返す
  optional<vector<T>> compute_potentials(bool use_spfa = false) {
    bool ok = use_spfa ? run_spfa_all_sources(false) : run_all_sources();
    if (!ok) return nullopt;
    return dist_;
  }

  // Johnson のアルゴリズムによる全点対最短経路 O(VE + VE log V)
  // 負閉路が存在する場合は nullopt を返す (未到達ペアは INF)
  optional<vector<vector<T>>> all_pairs_shortest_path(bool use_spfa = false) {
    auto h_opt = compute_potentials(use_spfa);
    if (!h_opt) return nullopt;
    const vector<T> &h = *h_opt;

    using D_T = conditional_t<is_integral_v<T> && sizeof(T) <= 8, __int128_t, T>;
    constexpr D_T D_INF = bf_internal::Limits<D_T>::max_val() / 2;

    vector<vector<T>> res(n_, vector<T>(n_, INF));
    using P = pair<D_T, int>;

    for (int s = 0; s < n_; ++s) {
      vector<D_T> d(n_, D_INF);
      vector<uint8_t> done(n_, 0);
      priority_queue<P, vector<P>, greater<P>> pq;
      d[s] = 0;
      pq.push({0, s});

      while (!pq.empty()) {
        auto [c, u] = pq.top();
        pq.pop();
        if (done[u]) continue;
        done[u] = 1;

        for (int eid : adj_[u]) {
          const auto &e = edges_[eid];
          int v = e.to;
          D_T rc = (D_T)e.cost + (D_T)h[u] - (D_T)h[v];
          if (rc < 0) rc = 0; // 浮動小数点誤差安全対策
          if (!done[v] && d[u] + rc < d[v]) {
            d[v] = d[u] + rc;
            pq.push({d[v], v});
          }
        }
      }

      for (int v = 0; v < n_; ++v) {
        if (done[v]) {
          D_T actual = d[v] - (D_T)h[s] + (D_T)h[v];
          if constexpr (is_integral_v<T> && sizeof(T) <= 8) {
            if (actual >= (D_T)INF) actual = (D_T)INF;
            else if (actual <= (D_T)NEG_INF) actual = (D_T)NEG_INF;
          }
          res[s][v] = (T)actual;
        }
      }
    }
    return res;
  }
};

// ============================================================================
// 差分制約系ソルバー (System of Difference Constraints / 牛ゲー)
// ============================================================================
// n 個の変数 x_0, ..., x_{n-1} に関する以下の線形不等式制約を管理・解決します:
// - x_u - x_v <= w   <=>  辺 v -> u (重み w)
// - x_u - x_v >= w   <=>  辺 u -> v (重み -w)
// - x_u <= w, x_u >= w などの絶対値境界 (内部参照頂点 n を基準 x_n = 0 として管理)
// ============================================================================
template <typename T = long long> struct DifferenceConstraints {
  struct BoundResult {
    enum Status { FINITE, INFEASIBLE, UNBOUNDED } status;
    T value = 0;
  };

private:
  int n_;
  BellmanFord<T> bf_; // 頂点 0..n-1 が変数、頂点 n が基準変数 x_zero (= 0)

public:
  explicit DifferenceConstraints(int n = 0, T eps = 0) : n_(n), bf_(n + 1, eps) {
    assert(n >= 0);
  }

  int num_variables() const { return n_; }

  // x_u - x_v <= w
  void add_diff_leq(int u, int v, T w) {
    assert(0 <= u && u < n_ && 0 <= v && v < n_);
    bf_.add_edge(v, u, w);
  }

  // x_u - x_v >= w  <=>  x_v - x_u <= -w
  void add_diff_geq(int u, int v, T w) {
    assert(0 <= u && u < n_ && 0 <= v && v < n_);
    bf_.add_edge(u, v, -w);
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

  // x_u <= w  <=>  x_u - x_n <= w
  void add_bound_leq(int u, T w) {
    assert(0 <= u && u < n_);
    bf_.add_edge(n_, u, w);
  }

  // x_u >= w  <=>  x_n - x_u <= -w
  void add_bound_geq(int u, T w) {
    assert(0 <= u && u < n_);
    bf_.add_edge(u, n_, -w);
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

  // 制約系に矛盾がないか判定
  bool is_feasible(bool use_spfa = false) {
    return use_spfa ? bf_.run_spfa_all_sources(false) : bf_.run_all_sources();
  }

  // すべての制約を満たす解 x[0..n-1] を 1 つ構成して返す (矛盾時は nullopt)
  optional<vector<T>> solve(bool use_spfa = false) {
    if (!is_feasible(use_spfa)) return nullopt;
    T zero_val = bf_.dist(n_);
    vector<T> x(n_);
    for (int i = 0; i < n_; ++i) {
      x[i] = bf_.dist(i) - zero_val;
    }
    return x;
  }

  // 矛盾の原因となっている負閉路を返す (先に is_feasible() や solve() を呼ぶか自動実行)
  vector<int> get_contradiction_cycle() {
    if (bf_.run_all_sources()) return {};
    return bf_.get_cycle();
  }

  // 制約のもとでの (x_u - x_v) の最大値を求める (POJ 3169 Layout など)
  // INFEASIBLE: 制約に矛盾あり / UNBOUNDED: 差をいくらでも大きくできる / FINITE: 最大値
  BoundResult max_diff(int u, int v, bool use_spfa = false) {
    assert(0 <= u && u < n_ && 0 <= v && v < n_);
    if (!is_feasible(use_spfa)) {
      return {BoundResult::INFEASIBLE, 0};
    }
    if (use_spfa) {
      bf_.run_spfa(v, false);
    } else {
      bf_.run(v);
    }
    if (!bf_.is_reachable(u)) {
      return {BoundResult::UNBOUNDED, 0};
    }
    return {BoundResult::FINITE, bf_.dist(u)};
  }

  // 制約のもとでの (x_u - x_v) の最小値を求める
  // min(x_u - x_v) = -max(x_v - x_u)
  BoundResult min_diff(int u, int v, bool use_spfa = false) {
    BoundResult res = max_diff(v, u, use_spfa);
    if (res.status == BoundResult::FINITE) {
      res.value = -res.value;
    }
    return res;
  }
};

// ============================================================================
// 隣接リストからの簡易呼び出し関数 (Dijkstra.hpp 互換スタイル)
// ============================================================================
template <typename T = long long>
inline BellmanFord<T> bellman_ford(const vector<vector<pair<int, T>>> &g,
                                   int start = 0) {
  BellmanFord<T> bf(g);
  bf.run(start);
  return bf;
}
