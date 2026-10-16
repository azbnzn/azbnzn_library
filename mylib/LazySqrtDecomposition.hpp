#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

// 遅延評価平方分割 (Lazy Sqrt Decomposition) - ACL lazy_segtree 完全互換
// S: 要素の型
// op: 2つの S を結合する演算
// e: 単位元を返す関数
// F: 作用素の型
// mapping: 作用素 f を要素 s に適用する関数 mapping(f, s)
// composition: 作用素 f, g を合成する関数 composition(f, g) (先に g を適用し、その後に f を適用)
// id: 恒等写像を返す関数
template <class S, S (*op)(S, S), S (*e)(), class F, S (*mapping)(F, S),
          F (*composition)(F, F), F (*id)()>
struct LazySqrtDecomposition {
  int n;
  int block_size;
  int num_blocks;
  vector<S> data;
  vector<S> block_val;
  vector<F> lazy;
  vector<bool> has_lazy;

  LazySqrtDecomposition() : LazySqrtDecomposition(0) {}
  explicit LazySqrtDecomposition(int n, int b_size = 0)
      : LazySqrtDecomposition(vector<S>(n, e()), b_size) {}

  explicit LazySqrtDecomposition(const vector<S> &v, int b_size = 0)
      : n(v.size()) {
    if (b_size <= 0) {
      block_size = max(1, (int)sqrt(n));
    } else {
      block_size = b_size;
    }
    num_blocks = (n + block_size - 1) / block_size;
    data = v;
    block_val.assign(num_blocks, e());
    lazy.assign(num_blocks, id());
    has_lazy.assign(num_blocks, false);
    for (int k = 0; k < num_blocks; k++) {
      update_block(k);
    }
  }

  // p 番目の要素を x に更新 (O(√N))
  void set(int p, S x) {
    assert(0 <= p && p < n);
    int k = p / block_size;
    push(k);
    data[p] = x;
    update_block(k);
  }

  // p 番目の要素を取得 (O(√N))
  S get(int p) {
    assert(0 <= p && p < n);
    int k = p / block_size;
    push(k);
    return data[p];
  }

  // p 番目の要素に作用素 f を適用 (O(√N))
  void apply(int p, F f) {
    assert(0 <= p && p < n);
    int k = p / block_size;
    push(k);
    data[p] = mapping(f, data[p]);
    update_block(k);
  }

  // 区間 [l, r) に作用素 f を適用 (O(√N))
  void apply(int l, int r, F f) {
    assert(0 <= l && l <= r && r <= n);
    if (l == r)
      return;

    int kl = l / block_size;
    int kr = (r - 1) / block_size;

    if (kl == kr) {
      push(kl);
      for (int i = l; i < r; i++) {
        data[i] = mapping(f, data[i]);
      }
      update_block(kl);
      return;
    }

    // 左端の半端部分
    push(kl);
    int l_end = (kl + 1) * block_size;
    for (int i = l; i < l_end; i++) {
      data[i] = mapping(f, data[i]);
    }
    update_block(kl);

    // 中間の完全ブロック
    for (int k = kl + 1; k < kr; k++) {
      apply_to_block(k, f);
    }

    // 右端の半端部分
    push(kr);
    int r_start = kr * block_size;
    for (int i = r_start; i < r; i++) {
      data[i] = mapping(f, data[i]);
    }
    update_block(kr);
  }

  // 半開区間 [l, r) の総積を取得 (O(√N))
  S prod(int l, int r) {
    assert(0 <= l && l <= r && r <= n);
    if (l == r)
      return e();

    int kl = l / block_size;
    int kr = (r - 1) / block_size;

    if (kl == kr) {
      push(kl);
      S res = e();
      for (int i = l; i < r; i++) {
        res = op(res, data[i]);
      }
      return res;
    }

    S res = e();
    // 左端の半端部分
    push(kl);
    int l_end = (kl + 1) * block_size;
    for (int i = l; i < l_end; i++) {
      res = op(res, data[i]);
    }

    // 中間の完全ブロック
    for (int k = kl + 1; k < kr; k++) {
      res = op(res, block_val[k]);
    }

    // 右端の半端部分
    push(kr);
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
      res = op(res, block_val[k]);
    }
    return res;
  }

  // 二分探索: g(prod(l, r)) = true となる最大の r を求める (O(√N))
  template <class G> int max_right(int l, G g) {
    assert(0 <= l && l <= n);
    assert(g(e()));
    if (l == n)
      return n;

    S current = e();
    int k = l / block_size;

    // 左端の半端部分
    push(k);
    int block_end = min(n, (k + 1) * block_size);
    for (int i = l; i < block_end; i++) {
      S nxt = op(current, data[i]);
      if (!g(nxt))
        return i;
      current = nxt;
    }
    k++;

    // 中間の完全ブロック
    while (k < num_blocks) {
      S nxt = op(current, block_val[k]);
      if (!g(nxt)) {
        // このブロック内で探索
        push(k);
        int start = k * block_size;
        int end = min(n, (k + 1) * block_size);
        for (int i = start; i < end; i++) {
          S nxt_elem = op(current, data[i]);
          if (!g(nxt_elem))
            return i;
          current = nxt_elem;
        }
      }
      current = nxt;
      k++;
    }
    return n;
  }

  // 二分探索: g(prod(l, r)) = true となる最小の l を求める (O(√N))
  template <class G> int min_left(int r, G g) {
    assert(0 <= r && r <= n);
    assert(g(e()));
    if (r == 0)
      return 0;

    S current = e();
    int k = (r - 1) / block_size;

    // 右端の半端部分
    push(k);
    int block_start = k * block_size;
    for (int i = r - 1; i >= block_start; i--) {
      S nxt = op(data[i], current);
      if (!g(nxt))
        return i + 1;
      current = nxt;
    }
    k--;

    // 中間の完全ブロック
    while (k >= 0) {
      S nxt = op(block_val[k], current);
      if (!g(nxt)) {
        // このブロック内で探索
        push(k);
        int start = k * block_size;
        int end = min(n, (k + 1) * block_size);
        for (int i = end - 1; i >= start; i--) {
          S nxt_elem = op(data[i], current);
          if (!g(nxt_elem))
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
  void push(int k) {
    if (!has_lazy[k])
      return;
    int start = k * block_size;
    int end = min(n, (k + 1) * block_size);
    for (int i = start; i < end; i++) {
      data[i] = mapping(lazy[k], data[i]);
    }
    lazy[k] = id();
    has_lazy[k] = false;
  }

  void update_block(int k) {
    int start = k * block_size;
    int end = min(n, (k + 1) * block_size);
    S res = e();
    for (int i = start; i < end; i++) {
      res = op(res, data[i]);
    }
    block_val[k] = res;
  }

  void apply_to_block(int k, F f) {
    block_val[k] = mapping(f, block_val[k]);
    if (has_lazy[k]) {
      lazy[k] = composition(f, lazy[k]);
    } else {
      lazy[k] = f;
      has_lazy[k] = true;
    }
  }
};
