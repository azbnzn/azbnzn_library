#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

// 平方分割 (Sqrt Decomposition) - ACL segtree 完全互換
// S: 要素の型
// op: 2つの S を結合する演算
// e: 単位元を返す関数
template <class S, S (*op)(S, S), S (*e)()> struct SqrtDecomposition {
  int n;
  int block_size;
  int num_blocks;
  vector<S> data;
  vector<S> block;

  SqrtDecomposition() : SqrtDecomposition(0) {}
  explicit SqrtDecomposition(int n, int b_size = 0)
      : SqrtDecomposition(vector<S>(n, e()), b_size) {}

  explicit SqrtDecomposition(const vector<S> &v, int b_size = 0) : n(v.size()) {
    if (b_size <= 0) {
      block_size = max(1, (int)sqrt(n));
    } else {
      block_size = b_size;
    }
    num_blocks = (n + block_size - 1) / block_size;
    data = v;
    block.assign(num_blocks, e());
    for (int k = 0; k < num_blocks; k++) {
      update_block(k);
    }
  }

  // p 番目の要素を x に更新 (O(B) = O(√N))
  void set(int p, S x) {
    assert(0 <= p && p < n);
    data[p] = x;
    update_block(p / block_size);
  }

  // p 番目の要素を取得 (O(1))
  S get(int p) const {
    assert(0 <= p && p < n);
    return data[p];
  }

  // 半開区間 [l, r) の総積を取得 (O(√N))
  S prod(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    if (l == r)
      return e();

    int kl = l / block_size;
    int kr = (r - 1) / block_size;

    if (kl == kr) {
      S res = e();
      for (int i = l; i < r; i++) {
        res = op(res, data[i]);
      }
      return res;
    }

    S res = e();
    // 左端の半端部分
    int l_end = (kl + 1) * block_size;
    for (int i = l; i < l_end; i++) {
      res = op(res, data[i]);
    }
    // 中間の完全ブロック
    for (int k = kl + 1; k < kr; k++) {
      res = op(res, block[k]);
    }
    // 右端の半端部分
    int r_start = kr * block_size;
    for (int i = r_start; i < r; i++) {
      res = op(res, data[i]);
    }
    return res;
  }

  // 全区間 [0, n) の総積を取得 (O(√N))
  S all_prod() const {
    S res = e();
    for (int k = 0; k < num_blocks; k++) {
      res = op(res, block[k]);
    }
    return res;
  }

  // 二分探索: f(prod(l, r)) = true となる最大の r を求める (O(√N))
  template <class F> int max_right(int l, F f) const {
    assert(0 <= l && l <= n);
    assert(f(e()));
    if (l == n)
      return n;

    S current = e();
    int k = l / block_size;

    // 左端の半端部分
    int block_end = min(n, (k + 1) * block_size);
    for (int i = l; i < block_end; i++) {
      S nxt = op(current, data[i]);
      if (!f(nxt))
        return i;
      current = nxt;
    }
    k++;

    // 中間のブロック
    while (k < num_blocks) {
      S nxt = op(current, block[k]);
      if (!f(nxt)) {
        // このブロック内で探索
        int start = k * block_size;
        int end = min(n, (k + 1) * block_size);
        for (int i = start; i < end; i++) {
          S nxt_elem = op(current, data[i]);
          if (!f(nxt_elem))
            return i;
          current = nxt_elem;
        }
      }
      current = nxt;
      k++;
    }
    return n;
  }

  // 二分探索: f(prod(l, r)) = true となる最小の l を求める (O(√N))
  template <class F> int min_left(int r, F f) const {
    assert(0 <= r && r <= n);
    assert(f(e()));
    if (r == 0)
      return 0;

    S current = e();
    int k = (r - 1) / block_size;

    // 右端の半端部分
    int block_start = k * block_size;
    for (int i = r - 1; i >= block_start; i--) {
      S nxt = op(data[i], current);
      if (!f(nxt))
        return i + 1;
      current = nxt;
    }
    k--;

    // 中間のブロック
    while (k >= 0) {
      S nxt = op(block[k], current);
      if (!f(nxt)) {
        // このブロック内で探索
        int start = k * block_size;
        int end = min(n, (k + 1) * block_size);
        for (int i = end - 1; i >= start; i--) {
          S nxt_elem = op(data[i], current);
          if (!f(nxt_elem))
            return i + 1;
          current = nxt_elem;
        }
      }
      current = nxt;
      k--;
    }
    return 0;
  }

private:
  void update_block(int k) {
    int start = k * block_size;
    int end = min(n, (k + 1) * block_size);
    S res = e();
    for (int i = start; i < end; i++) {
      res = op(res, data[i]);
    }
    block[k] = res;
  }
};
