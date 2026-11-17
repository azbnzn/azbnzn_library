#pragma once
#include <algorithm>
#include <cassert>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// マージテク / Small to Large (MergeTechnique)
// ============================================================================
// 「小さい方の集合を大きい方の集合へ移す (必要なら swap する)」ことで、
// 要素の移動回数を 1 要素あたり O(log N) 回、全体 O(N log N) に抑えるテクニック集。
//
// 【主な機能】
// 1. merge_small_to_large(dst, src):
//    - set / multiset / unordered_set / vector / deque を高速マージして src を空にする
// 2. merge_map_small_to_large(dst, src, combine):
//    - map / unordered_map のキーごとマージ (既定は += で値合算)
// 3. SmallToLargeDSU:
//    - クエリでの集合マージを O(N log N) で管理 (各集合の要素列・O(1)代表元取得・移動コールバック)
// 4. TreeMerge (DSU on Tree / Sack):
//    - 部分木クエリを O(N log N) 回・省メモリ (O(N)) で解く木上マージフレームワーク
// ============================================================================

namespace merge_internal {

template <typename T, typename = void> struct has_push_back : false_type {};
template <typename T>
struct has_push_back<
    T, void_t<decltype(declval<T &>().push_back(declval<typename T::value_type>()))>>
    : true_type {};

template <typename T, typename = void> struct has_reserve : false_type {};
template <typename T>
struct has_reserve<T, void_t<decltype(declval<T &>().reserve(0))>> : true_type {};

template <typename T, typename = void> struct has_merge_method : false_type {};
template <typename T>
struct has_merge_method<T, void_t<decltype(declval<T &>().merge(declval<T &>()))>>
    : true_type {};

} // namespace merge_internal

// set / multiset / unordered_set / vector / deque の Small-to-Large マージ
// 小さい方を大きい方へ吸収し、dst に統合結果、src を空にする。
template <typename Container>
inline void merge_small_to_large(Container &dst, Container &src) {
  if (&dst == &src)
    return;
  if (dst.size() < src.size())
    swap(dst, src);
  if constexpr (merge_internal::has_push_back<Container>::value) {
    if constexpr (merge_internal::has_reserve<Container>::value) {
      dst.reserve(dst.size() + src.size());
    }
    for (auto &x : src) {
      dst.push_back(std::move(x));
    }
  } else if constexpr (merge_internal::has_merge_method<Container>::value) {
    dst.merge(src);
  } else {
    dst.insert(make_move_iterator(src.begin()), make_move_iterator(src.end()));
  }
  src.clear();
}

// map / unordered_map の Small-to-Large マージ (値結合関数 combine 指定可能)
template <typename Map, typename Combine>
inline void merge_map_small_to_large(Map &dst, Map &src, Combine combine) {
  if (&dst == &src)
    return;
  if (dst.size() < src.size())
    swap(dst, src);
  for (auto &kv : src) {
    auto it = dst.find(kv.first);
    if (it == dst.end()) {
      dst.emplace(kv.first, std::move(kv.second));
    } else {
      combine(it->second, kv.second);
    }
  }
  src.clear();
}

template <typename Map>
inline void merge_map_small_to_large(Map &dst, Map &src) {
  merge_map_small_to_large(
      dst, src, [](auto &a, const auto &b) { a += b; });
}

// ----------------------------------------------------------------------------
// クエリによる集合マージ (3.cpp スタイル / 要素列保持 + O(1) 代表元参照)
// ----------------------------------------------------------------------------
struct SmallToLargeDSU {
  int n;
  int num_groups_;
  vector<vector<int>> groups; // groups[leader(u)] に属する全要素
  vector<int> par;            // par[x]: 要素 x の現在の代表元 (常に O(1))

  explicit SmallToLargeDSU(int n = 0) : n(n), num_groups_(n), groups(n), par(n) {
    assert(n >= 0);
    for (int i = 0; i < n; ++i) {
      groups[i].push_back(i);
      par[i] = i;
    }
  }

  int leader(int u) const {
    assert(0 <= u && u < n);
    return par[u];
  }

  int root(int u) const { return leader(u); }

  bool same(int u, int v) const {
    assert(0 <= u && u < n && 0 <= v && v < n);
    return par[u] == par[v];
  }

  int size(int u) const {
    assert(0 <= u && u < n);
    return (int)groups[par[u]].size();
  }

  int count_groups() const { return num_groups_; }

  const vector<int> &members(int u) const {
    assert(0 <= u && u < n);
    return groups[par[u]];
  }

  // 集合併合時に移動する各要素 (elem, new_leader, old_leader) へコールバックしつつマージ
  template <typename OnMove>
  bool merge(int u, int v, OnMove on_move) {
    assert(0 <= u && u < n && 0 <= v && v < n);
    u = par[u];
    v = par[v];
    if (u == v)
      return false;
    if (groups[u].size() < groups[v].size())
      swap(u, v);
    groups[u].reserve(groups[u].size() + groups[v].size());
    for (int x : groups[v]) {
      on_move(x, u, v);
      groups[u].push_back(x);
      par[x] = u;
    }
    groups[v].clear();
    num_groups_--;
    return true;
  }

  bool merge(int u, int v) {
    return merge(u, v, [](int, int, int) {});
  }

  bool unite(int u, int v) { return merge(u, v); }
};

// ----------------------------------------------------------------------------
// 木上のマージテク (DSU on Tree / Sack) 汎用エンジン O(N log N)
// ----------------------------------------------------------------------------
// add(v)    : 頂点 v の寄与をグローバル状態へ追加
// remove(v) : 頂点 v の寄与をグローバル状態から削除
// answer(u) : 状態が「部分木 u の全頂点」のみを保持しているタイミングで呼ばれる
struct TreeMerge {
  int n;
  vector<vector<int>> g;

  explicit TreeMerge(int n = 0) : n(n), g(n) { assert(n >= 0); }

  void add_edge(int u, int v) {
    assert(0 <= u && u < n && 0 <= v && v < n);
    g[u].push_back(v);
    g[v].push_back(u);
  }

  template <typename Add, typename Remove, typename Answer>
  void run(int root, Add add, Remove remove, Answer answer) const {
    if (n == 0)
      return;
    assert(0 <= root && root < n);

    vector<int> sub(n, 1), heavy(n, -1), tin(n), tout(n), order(n);
    int timer = 0;
    auto dfs_sz = [&](auto &self, int u, int p) -> void {
      tin[u] = timer;
      order[timer++] = u;
      int max_sz = 0;
      for (int v : g[u]) {
        if (v == p)
          continue;
        self(self, v, u);
        sub[u] += sub[v];
        if (sub[v] > max_sz) {
          max_sz = sub[v];
          heavy[u] = v;
        }
      }
      tout[u] = timer;
    };
    dfs_sz(dfs_sz, root, -1);

    auto dfs = [&](auto &self, int u, int p, bool keep) -> void {
      for (int v : g[u]) {
        if (v != p && v != heavy[u]) {
          self(self, v, u, false);
        }
      }
      if (heavy[u] != -1) {
        self(self, heavy[u], u, true);
      }
      for (int v : g[u]) {
        if (v != p && v != heavy[u]) {
          for (int idx = tin[v]; idx < tout[v]; ++idx) {
            add(order[idx]);
          }
        }
      }
      add(u);
      answer(u);
      if (!keep) {
        for (int idx = tin[u]; idx < tout[u]; ++idx) {
          remove(order[idx]);
        }
      }
    };
    dfs(dfs, root, -1, false);
  }
};
