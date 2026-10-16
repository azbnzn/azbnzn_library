#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

// 完全永続セグメント木 (Fully Persistent Segment Tree)
// 各更新で新しい根ノードのインデックス (version) を返し、過去のあらゆるバージョンに対する
// 点更新・区間取得・k番目の値・二分探索を O(log N) で行える。
//
// S: 要素の型
// op: 2つの S を結合する演算
// e: 単位元を返す関数
template <class S, S (*op)(S, S), S (*e)()> struct PersistentSegTree {
  struct Node {
    S val;
    int left = -1, right = -1;
    Node(S val) : val(val), left(-1), right(-1) {}
    Node(S val, int l, int r) : val(val), left(l), right(r) {}
  };

  int n;
  vector<Node> pool;

  PersistentSegTree() : n(0) {}
  PersistentSegTree(int n) : n(n) {}

  // 初期配列 v から初期バージョン (根ノードID) を O(N) で構築
  int build(const vector<S> &v) {
    n = v.size();
    if (n == 0)
      return -1;
    return build_rec(0, n, v);
  }

  // 空 (すべて e()) の初期バージョン (根ノードID) を作成
  int build_empty(int size) {
    n = size;
    if (n == 0)
      return -1;
    return build_empty_rec(0, n);
  }

  // version root の状態をベースに A[p] = x とした新しいバージョンの根IDを返す (O(log N))
  int set(int root, int p, S x) {
    assert(0 <= p && p < n);
    return set_rec(root, 0, n, p, x);
  }

  // version root の状態をベースに A[p] = op(A[p], x) とした新しいバージョンの根IDを返す (O(log N))
  int apply(int root, int p, S x) {
    assert(0 <= p && p < n);
    return apply_rec(root, 0, n, p, x);
  }

  // version root における A[p] を取得 (O(log N))
  S get(int root, int p) const {
    assert(0 <= p && p < n);
    return get_rec(root, 0, n, p);
  }

  // version root における区間 [l, r) の総積を取得 (O(log N))
  S prod(int root, int l, int r) const {
    if (l >= r)
      return e();
    assert(0 <= l && r <= n);
    return prod_rec(root, 0, n, l, r);
  }

  // version root の全区間の総積
  S all_prod(int root) const {
    if (root == -1)
      return e();
    return pool[root].val;
  }

  // 【頻出典型】区間 k 番目の値 (k-th element)
  // 永続カウントセグ木において、root_r - root_l の差分を取り、
  // 区間 [0, n) の中で k 番目 (0-indexed: 0 <= k < total_count) のインデックスを O(log N) で見つける
  // ※ S が数値カウント型 (加算が op) であることを前提とします
  int kth_element(int root_l, int root_r, S k) const {
    assert(k >= 0);
    return kth_element_rec(root_l, root_r, 0, n, k);
  }

  // 二分探索: f(prod(l, r)) = true となる最大の r を求める (O(log N))
  template <class F> int max_right(int root, int l, F f) const {
    assert(0 <= l && l <= n);
    assert(f(e()));
    if (l == n)
      return n;
    S current = e();
    return max_right_rec(root, 0, n, l, f, current);
  }

  // 二分探索: f(prod(l, r)) = true となる最小の l を求める (O(log N))
  template <class F> int min_left(int root, int r, F f) const {
    assert(0 <= r && r <= n);
    assert(f(e()));
    if (r == 0)
      return 0;
    S current = e();
    return min_left_rec(root, 0, n, r, f, current);
  }

  // 生成された全ノード数を取得
  int node_count() const { return pool.size(); }

private:
  int new_node(S val, int l = -1, int r = -1) {
    pool.emplace_back(val, l, r);
    return (int)pool.size() - 1;
  }

  int build_rec(int l, int r, const vector<S> &v) {
    if (r - l == 1) {
      return new_node(v[l]);
    }
    int m = (l + r) / 2;
    int lc = build_rec(l, m, v);
    int rc = build_rec(m, r, v);
    return new_node(op(pool[lc].val, pool[rc].val), lc, rc);
  }

  int build_empty_rec(int l, int r) {
    if (r - l == 1) {
      return new_node(e());
    }
    int m = (l + r) / 2;
    int lc = build_empty_rec(l, m);
    int rc = build_empty_rec(m, r);
    return new_node(e(), lc, rc);
  }

  int set_rec(int u, int l, int r, int p, S x) {
    if (r - l == 1) {
      return new_node(x);
    }
    int m = (l + r) / 2;
    int lc = (u == -1 ? -1 : pool[u].left);
    int rc = (u == -1 ? -1 : pool[u].right);
    if (p < m) {
      lc = set_rec(lc, l, m, p, x);
    } else {
      rc = set_rec(rc, m, r, p, x);
    }
    S lv = (lc == -1 ? e() : pool[lc].val);
    S rv = (rc == -1 ? e() : pool[rc].val);
    return new_node(op(lv, rv), lc, rc);
  }

  int apply_rec(int u, int l, int r, int p, S x) {
    if (r - l == 1) {
      S cur = (u == -1 ? e() : pool[u].val);
      return new_node(op(cur, x));
    }
    int m = (l + r) / 2;
    int lc = (u == -1 ? -1 : pool[u].left);
    int rc = (u == -1 ? -1 : pool[u].right);
    if (p < m) {
      lc = apply_rec(lc, l, m, p, x);
    } else {
      rc = apply_rec(rc, m, r, p, x);
    }
    S lv = (lc == -1 ? e() : pool[lc].val);
    S rv = (rc == -1 ? e() : pool[rc].val);
    return new_node(op(lv, rv), lc, rc);
  }

  S get_rec(int u, int l, int r, int p) const {
    if (u == -1)
      return e();
    if (r - l == 1)
      return pool[u].val;
    int m = (l + r) / 2;
    if (p < m)
      return get_rec(pool[u].left, l, m, p);
    else
      return get_rec(pool[u].right, m, r, p);
  }

  S prod_rec(int u, int l, int r, int ql, int qr) const {
    if (u == -1 || qr <= l || r <= ql)
      return e();
    if (ql <= l && r <= qr)
      return pool[u].val;
    int m = (l + r) / 2;
    return op(prod_rec(pool[u].left, l, m, ql, qr),
              prod_rec(pool[u].right, m, r, ql, qr));
  }

  int kth_element_rec(int ul, int ur, int l, int r, S k) const {
    if (r - l == 1)
      return l;
    int m = (l + r) / 2;
    int l_left = (ul == -1 ? -1 : pool[ul].left);
    int r_left = (ur == -1 ? -1 : pool[ur].left);
    S cnt_l = (l_left == -1 ? e() : pool[l_left].val);
    S cnt_r = (r_left == -1 ? e() : pool[r_left].val);
    S left_count = cnt_r - cnt_l; // [l, m) の区間内の出現頻度
    if (k < left_count) {
      return kth_element_rec(l_left, r_left, l, m, k);
    } else {
      int l_right = (ul == -1 ? -1 : pool[ul].right);
      int r_right = (ur == -1 ? -1 : pool[ur].right);
      return kth_element_rec(l_right, r_right, m, r, k - left_count);
    }
  }

  template <class F>
  int max_right_rec(int u, int l, int r, int ql, F f, S &current) const {
    if (u == -1)
      return min(r, n);
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
    int m = (l + r) / 2;
    int res = max_right_rec(pool[u].left, l, m, ql, f, current);
    if (res < m)
      return res;
    return max_right_rec(pool[u].right, m, r, ql, f, current);
  }

  template <class F>
  int min_left_rec(int u, int l, int r, int qr, F f, S &current) const {
    if (u == -1)
      return max(l, 0);
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
    int m = (l + r) / 2;
    int res = min_left_rec(pool[u].right, m, r, qr, f, current);
    if (res > m)
      return res;
    return min_left_rec(pool[u].left, l, m, qr, f, current);
  }
};
