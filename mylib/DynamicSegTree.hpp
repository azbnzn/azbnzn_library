#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

// 動的セグメント木 (Dynamic Segment Tree / Sparse Segment Tree)
// 座標が 0 <= p < MAX_COORD (例: 10^9 や 10^18) のような巨大な範囲で、
// オンデマンドにノードを生成しながら点更新・区間積・二分探索を O(log(MAX)) で処理する。
//
// S: 要素の型
// op: 2つの S を結合する演算
// e: 単位元を返す関数
template <class S, S (*op)(S, S), S (*e)()> struct DynamicSegTree {
  using Index = long long;

  struct Node {
    S val;
    int left, right; // 子ノードのインデックス (-1 は未生成)
    Node(S val) : val(val), left(-1), right(-1) {}
  };

  Index L, R; // カバーする座標範囲 [L, R)
  vector<Node> pool;
  int root;

  // デフォルトコンストラクタ: 範囲 [0, max_range)
  // max_range のデフォルトは 1LL << 30 (約 1.07 * 10^9)
  DynamicSegTree(Index max_range = (1LL << 30))
      : DynamicSegTree(0, max_range) {}

  DynamicSegTree(Index l, Index r) : L(l), R(r) { clear(); }

  void clear() {
    pool.clear();
    root = new_node();
  }

  // p 番目の要素を x に更新 (O(log(MAX)))
  void set(Index p, S x) {
    assert(L <= p && p < R);
    set(root, L, R, p, x);
  }

  // p 番目の要素に x を作用: A[p] = op(A[p], x) (O(log(MAX)))
  void apply(Index p, S x) {
    assert(L <= p && p < R);
    apply(root, L, R, p, x);
  }

  // p 番目の要素を取得 (O(log(MAX)))
  S get(Index p) const {
    assert(L <= p && p < R);
    return get(root, L, R, p);
  }

  // 半開区間 [l, r) の総積を取得 (l >= r の場合は e()) (O(log(MAX)))
  S prod(Index l, Index r) const {
    if (l >= r)
      return e();
    assert(L <= l && r <= R);
    return prod(root, L, R, l, r);
  }

  // 全区間 [L, R) の総積を取得 (O(1))
  S all_prod() const { return pool[root].val; }

  // 二分探索: f(prod(l, r)) = true となる最大の r を求める (O(log(MAX)))
  template <class F> Index max_right(Index l, F f) const {
    assert(L <= l && l <= R);
    assert(f(e()));
    if (l == R)
      return R;
    S current = e();
    return max_right(root, L, R, l, f, current);
  }

  // 二分探索: f(prod(l, r)) = true となる最小の l を求める (O(log(MAX)))
  template <class F> Index min_left(Index r, F f) const {
    assert(L <= r && r <= R);
    assert(f(e()));
    if (r == L)
      return L;
    S current = e();
    return min_left(root, L, R, r, f, current);
  }

  // 生成されたノード数を取得
  int size() const { return pool.size(); }

private:
  int new_node() {
    pool.emplace_back(e());
    return (int)pool.size() - 1;
  }

  void set(int u, Index l, Index r, Index p, S x) {
    if (r - l == 1) {
      pool[u].val = x;
      return;
    }
    Index m = l + ((r - l) >> 1);
    if (p < m) {
      if (pool[u].left == -1) {
        int nxt = new_node();
        pool[u].left = nxt;
      }
      set(pool[u].left, l, m, p, x);
    } else {
      if (pool[u].right == -1) {
        int nxt = new_node();
        pool[u].right = nxt;
      }
      set(pool[u].right, m, r, p, x);
    }
    update(u);
  }

  void apply(int u, Index l, Index r, Index p, S x) {
    if (r - l == 1) {
      pool[u].val = op(pool[u].val, x);
      return;
    }
    Index m = l + ((r - l) >> 1);
    if (p < m) {
      if (pool[u].left == -1) {
        int nxt = new_node();
        pool[u].left = nxt;
      }
      apply(pool[u].left, l, m, p, x);
    } else {
      if (pool[u].right == -1) {
        int nxt = new_node();
        pool[u].right = nxt;
      }
      apply(pool[u].right, m, r, p, x);
    }
    update(u);
  }

  S get(int u, Index l, Index r, Index p) const {
    if (u == -1)
      return e();
    if (r - l == 1)
      return pool[u].val;
    Index m = l + ((r - l) >> 1);
    if (p < m)
      return get(pool[u].left, l, m, p);
    else
      return get(pool[u].right, m, r, p);
  }

  S prod(int u, Index l, Index r, Index ql, Index qr) const {
    if (u == -1 || qr <= l || r <= ql)
      return e();
    if (ql <= l && r <= qr)
      return pool[u].val;
    Index m = l + ((r - l) >> 1);
    return op(prod(pool[u].left, l, m, ql, qr),
              prod(pool[u].right, m, r, ql, qr));
  }

  void update(int u) {
    S lv = (pool[u].left == -1 ? e() : pool[pool[u].left].val);
    S rv = (pool[u].right == -1 ? e() : pool[pool[u].right].val);
    pool[u].val = op(lv, rv);
  }

  template <class F>
  Index max_right(int u, Index l, Index r, Index ql, F f, S &current) const {
    if (u == -1) {
      // 未生成区間はすべて e() なので current は変化しない
      return min(r, R);
    }
    if (r <= ql)
      return r;
    if (ql <= l) {
      S nxt = op(current, pool[u].val);
      if (f(nxt)) {
        current = nxt;
        return r;
      }
      if (r - l == 1)
        return l;
    }
    Index m = l + ((r - l) >> 1);
    Index res = max_right(pool[u].left, l, m, ql, f, current);
    if (res < m)
      return res;
    return max_right(pool[u].right, m, r, ql, f, current);
  }

  template <class F>
  Index min_left(int u, Index l, Index r, Index qr, F f, S &current) const {
    if (u == -1) {
      return max(l, L);
    }
    if (qr <= l)
      return l;
    if (r <= qr) {
      S nxt = op(pool[u].val, current);
      if (f(nxt)) {
        current = nxt;
        return l;
      }
      if (r - l == 1)
        return r;
    }
    Index m = l + ((r - l) >> 1);
    Index res = min_left(pool[u].right, m, r, qr, f, current);
    if (res > m)
      return res;
    return min_left(pool[u].left, l, m, qr, f, current);
  }
};
