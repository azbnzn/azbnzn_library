#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// 区間を set で管理するテク (Interval Set & Interval Map)
// ============================================================================
// 【概要】
// 互いに交差しない閉区間 [l, r] の集合を std::set で管理し、区間の追加・削除・
// 統合（マージ）・被覆判定・mex（最小未被覆値）検索を高速に行うデータ構造です。
//
// 【提供クラス】
// 1. IntervalSet<T>:
//    - 閉区間 [l, r] の集合を管理 (整数型 T, デフォルト long long)
//    - insert(l, r): 区間の追加。重複・隣接区間をマージし、被覆長の変化量を返す
//    - erase(l, r): 区間の削除。被覆部分を削り、必要に応じて区間を分割
//    - contains(x) / contains(l, r): 点または区間が完全に被覆されているか判定
//    - intersects(l, r): 区間と一部でも交差するか判定
//    - mex(x): x 以上の最小の「被覆されていない」整数を O(log N) で取得 (ABC228D等)
//    - prev_absent(x): x 以下の最大の「被覆されていない」整数を取得
//    - get(x): 点 x を含む区間 [l, r] を取得 (存在しない場合は nullopt)
//    - covered_length(): 被覆されている整数の総数 (O(1))
//    - size(): 独立した区間の個数 (O(1))
//    - コールバック付き insert / erase (drken式):
//        - del と add が逆操作である場合: on_add, on_erase を指定してマージ前後の区間を処理
//        - 新規追加/削除部分のみ処理したい場合: insert_strictly_new, erase_strictly_removed
//    - 隣接区間の自動結合 (merge_adjacent): true なら [1, 3] と [4, 6] を [1, 6] に統合
//
// 2. IntervalMap<T, V>: (Chtholly Tree / Old Driver Tree / 区間カラーリング)
//    - 区間 [l, r] に値（色や数値）を割り当てて管理
//    - assign(l, r, val): 区間 [l, r] の値を val に一括上書き (ならし O(log N))
//    - get(x): 点 x の値を取得
//    - for_each(l, r, fn): 区間 [l, r] 内の各連続成分に対して処理を実行
//
// 【計算量】
// - insert / erase / assign: ならし O(log N) (N は区間の個数)
// - contains / intersects / mex / get: O(log N)
// - covered_length / size / empty: O(1)
// ============================================================================

template <typename T = long long>
class IntervalSet {
public:
  using Interval = std::pair<T, T>;

private:
  std::set<Interval> st;
  T total_len = 0;
  bool merge_adj = true;

  static constexpr T INF = std::numeric_limits<T>::max();

public:
  // コンストラクタ
  // merge_adjacent: 隣接する区間 (例: [1, 2] と [3, 4]) をマージするかどうか (デフォルト: true)
  explicit IntervalSet(bool merge_adjacent = true) : merge_adj(merge_adjacent) {}

  // 状態取得
  bool empty() const { return st.empty(); }
  size_t size() const { return st.size(); }
  T covered_length() const { return total_len; }
  void clear() {
    st.clear();
    total_len = 0;
  }

  // イテレータ (区間 [l, r] の昇順)
  auto begin() const { return st.begin(); }
  auto end() const { return st.end(); }
  auto rbegin() const { return st.rbegin(); }
  auto rend() const { return st.rend(); }

  // 全区間のリストを取得
  std::vector<Interval> get_intervals() const {
    return std::vector<Interval>(st.begin(), st.end());
  }

  // --------------------------------------------------------------------------
  // 基本操作: insert (区間の追加)
  // --------------------------------------------------------------------------
  // [l, r] を追加し、交差または隣接する区間をマージする。
  // 戻り値: pair<最終的なマージ後区間 [L, R], 新たに被覆された要素数 (差分)>
  std::pair<Interval, T> insert(T l, T r) {
    if (l > r) return {{l, r}, 0};

    auto it = st.upper_bound({l, INF});
    if (it != st.begin()) {
      auto pit = std::prev(it);
      if (pit->second + (merge_adj ? 1 : 0) >= l) {
        it = pit;
      }
    }

    T nl = l, nr = r;
    T removed_len = 0;

    while (it != st.end() && it->first <= r + (merge_adj ? 1 : 0)) {
      nl = std::min(nl, it->first);
      nr = std::max(nr, it->second);
      removed_len += (it->second - it->first + 1);
      it = st.erase(it);
    }

    st.insert({nl, nr});
    T added_len = (nr - nl + 1) - removed_len;
    total_len += added_len;
    return {{nl, nr}, added_len};
  }

  // 単一の点 x を追加
  std::pair<Interval, T> insert(T x) {
    return insert(x, x);
  }

  // --------------------------------------------------------------------------
  // 基本操作: erase (区間の削除)
  // --------------------------------------------------------------------------
  // [l, r] を集合から削除する（被覆部分を削り、必要に応じて区間を分割）。
  // 戻り値: 実際に削除された要素数
  T erase(T l, T r) {
    if (l > r || st.empty()) return 0;

    auto it = st.upper_bound({l, INF});
    if (it != st.begin()) {
      auto pit = std::prev(it);
      if (pit->second >= l) {
        it = pit;
      }
    }

    T prev_total = total_len;
    std::vector<Interval> to_readd;

    while (it != st.end() && it->first <= r) {
      T il = it->first;
      T ir = it->second;
      total_len -= (ir - il + 1);
      it = st.erase(it);

      if (il < l) {
        to_readd.push_back({il, l - 1});
      }
      if (ir > r) {
        to_readd.push_back({r + 1, ir});
      }
    }

    for (const auto &iv : to_readd) {
      st.insert(iv);
      total_len += (iv.second - iv.first + 1);
    }

    return prev_total - total_len;
  }

  // 単一の点 x を削除
  T erase(T x) {
    return erase(x, x);
  }

  // --------------------------------------------------------------------------
  // 判定・クエリ操作
  // --------------------------------------------------------------------------
  // 点 x がいずれかの区間に含まれているか
  bool contains(T x) const {
    auto it = st.upper_bound({x, INF});
    if (it == st.begin()) return false;
    --it;
    return it->first <= x && x <= it->second;
  }

  // 区間 [l, r] 全体が単一の区間に完全に含まれているか
  bool contains(T l, T r) const {
    if (l > r) return true;
    auto it = st.upper_bound({l, INF});
    if (it == st.begin()) return false;
    --it;
    return it->first <= l && r <= it->second;
  }

  // 区間 [l, r] が既存のいずれかの区間と一部でも交差しているか
  bool intersects(T l, T r) const {
    if (l > r || st.empty()) return false;
    auto it = st.upper_bound({l, INF});
    if (it != st.begin()) {
      auto pit = std::prev(it);
      if (pit->second >= l) return true;
    }
    return (it != st.end() && it->first <= r);
  }

  // 点 x を含む区間 [l, r] を取得 (含まれていなければ nullopt)
  std::optional<Interval> get(T x) const {
    auto it = st.upper_bound({x, INF});
    if (it == st.begin()) return std::nullopt;
    --it;
    if (it->first <= x && x <= it->second) return *it;
    return std::nullopt;
  }

  // 点 x 以上の最小の「被覆されていない」整数を取得 (mex)
  // 【用途】ABC228D (Linear Probing), ゲーム木の mex 遷移, 重複なし採番など
  T mex(T x = 0) const {
    auto it = st.upper_bound({x, INF});
    if (it == st.begin()) return x;
    --it;
    if (it->first <= x && x <= it->second) {
      return it->second + 1;
    }
    return x;
  }

  // 点 x 以下の最大の「被覆されていない」整数を取得
  T prev_absent(T x) const {
    auto it = st.upper_bound({x, INF});
    if (it == st.begin()) return x;
    --it;
    if (it->first <= x && x <= it->second) {
      return it->first - 1;
    }
    return x;
  }

  // --------------------------------------------------------------------------
  // コールバック対応 insert / erase (drken 式テクニック 1)
  // --------------------------------------------------------------------------
  // 区間の追加と削除が互いに可逆（逆操作可能）な場合に、
  // 削除された各区間に対して on_erase(l, r)、
  // 最終的に追加された区間に on_add(L, R) を呼び出します。
  template <typename OnAdd, typename OnErase>
  Interval insert(T l, T r, OnAdd on_add, OnErase on_erase) {
    if (l > r) return {l, r};

    auto it = st.upper_bound({l, INF});
    if (it != st.begin()) {
      auto pit = std::prev(it);
      if (pit->second + (merge_adj ? 1 : 0) >= l) {
        it = pit;
      }
    }

    T nl = l, nr = r;
    T removed_len = 0;

    while (it != st.end() && it->first <= r + (merge_adj ? 1 : 0)) {
      nl = std::min(nl, it->first);
      nr = std::max(nr, it->second);
      removed_len += (it->second - it->first + 1);
      on_erase(it->first, it->second);
      it = st.erase(it);
    }

    st.insert({nl, nr});
    total_len += ((nr - nl + 1) - removed_len);
    on_add(nl, nr);
    return {nl, nr};
  }

  template <typename OnAdd, typename OnErase>
  void erase(T l, T r, OnAdd on_add, OnErase on_erase) {
    if (l > r || st.empty()) return;

    auto it = st.upper_bound({l, INF});
    if (it != st.begin()) {
      auto pit = std::prev(it);
      if (pit->second >= l) {
        it = pit;
      }
    }

    std::vector<Interval> to_readd;

    while (it != st.end() && it->first <= r) {
      T il = it->first;
      T ir = it->second;
      total_len -= (ir - il + 1);
      on_erase(il, ir);
      it = st.erase(it);

      if (il < l) {
        to_readd.push_back({il, l - 1});
      }
      if (ir > r) {
        to_readd.push_back({r + 1, ir});
      }
    }

    for (const auto &iv : to_readd) {
      st.insert(iv);
      total_len += (iv.second - iv.first + 1);
      on_add(iv.first, iv.second);
    }
  }

  // --------------------------------------------------------------------------
  // 新規追加 / 削除差分コールバック (drken 式テクニック 2)
  // --------------------------------------------------------------------------
  // 「純粋に新しく被覆された区間」のみに対して on_new_interval(sub_l, sub_r) を実行
  template <typename OnNew>
  Interval insert_strictly_new(T l, T r, OnNew on_new_interval) {
    if (l > r) return {l, r};

    // [l, r] 内で未被覆だった部分を探索
    T cur = l;
    while (cur <= r) {
      auto it = st.upper_bound({cur, INF});
      if (it != st.begin()) {
        auto pit = std::prev(it);
        if (pit->second >= cur) {
          cur = pit->second + 1;
          continue;
        }
      }
      // cur は未被覆
      T nxt = r;
      if (it != st.end()) {
        nxt = std::min(r, it->first - 1);
      }
      if (cur <= nxt) {
        on_new_interval(cur, nxt);
        cur = nxt + 1;
      }
    }

    // 集合へのマージ追加
    return insert(l, r).first;
  }

  // 「純粋に削除された区間」のみに対して on_removed_interval(sub_l, sub_r) を実行
  template <typename OnRemoved>
  void erase_strictly_removed(T l, T r, OnRemoved on_removed_interval) {
    if (l > r || st.empty()) return;

    auto it = st.upper_bound({l, INF});
    if (it != st.begin()) {
      auto pit = std::prev(it);
      if (pit->second >= l) {
        it = pit;
      }
    }

    while (it != st.end() && it->first <= r) {
      T subl = std::max(l, it->first);
      T subr = std::min(r, it->second);
      if (subl <= subr) {
        on_removed_interval(subl, subr);
      }
      ++it;
    }

    erase(l, r);
  }
};

// ============================================================================
// IntervalMap (Chtholly Tree / Old Driver Tree)
// ============================================================================
// 区間 [l, r] に任意の値（色や数値）を割り当てて管理するデータ構造
template <typename T = long long, typename V = long long>
class IntervalMap {
public:
  struct Node {
    T l, r;
    mutable V val;
    bool operator<(const Node &rhs) const {
      return l < rhs.l;
    }
  };

  using SetIterator = typename std::set<Node>::iterator;

private:
  std::set<Node> st;

public:
  IntervalMap() {}

  // 初期区間 [l, r] を初期値 val で構築
  IntervalMap(T l, T r, V val) {
    st.insert({l, r, val});
  }

  bool empty() const { return st.empty(); }
  size_t size() const { return st.size(); }
  void clear() { st.clear(); }

  auto begin() const { return st.begin(); }
  auto end() const { return st.end(); }

  // 点 x で区間を分割し、[l, x-1] と [x, r] に切り分ける
  // 戻り値: [x, r] を指すイテレータ
  SetIterator split(T x) {
    auto it = st.upper_bound({x, 0, V()});
    if (it != st.begin()) {
      --it;
      if (it->l <= x && x <= it->r) {
        if (it->l == x) return it;
        T l = it->l, r = it->r;
        V v = it->val;
        st.erase(it);
        st.insert({l, x - 1, v});
        return st.insert({x, r, v}).first;
      }
    }
    return st.end();
  }

  // 区間 [l, r] の値を一括して val に更新する (ならし O(log N))
  void assign(T l, T r, V val) {
    if (l > r) return;
    auto itr = split(r + 1);
    auto itl = split(l);
    st.erase(itl, itr);
    st.insert({l, r, val});
  }

  // 点 x の値を取得
  V get(T x) const {
    auto it = st.upper_bound({x, 0, V()});
    assert(it != st.begin());
    --it;
    assert(it->l <= x && x <= it->r);
    return it->val;
  }

  // 区間 [l, r] に含まれる各連続部分区間に対して fn(sub_l, sub_r, val) を実行
  template <typename Func>
  void for_each(T l, T r, Func fn) {
    if (l > r) return;
    auto itr = split(r + 1);
    auto itl = split(l);
    for (auto it = itl; it != itr; ++it) {
      fn(it->l, it->r, it->val);
    }
  }
};

using IntervalSetLL = IntervalSet<long long>;
using IntervalMapLL = IntervalMap<long long, long long>;
