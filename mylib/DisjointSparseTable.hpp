#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

// Disjoint Sparse Table
// 結合律のみを満たす任意の半群 (Semigroup) に対し、静的な区間積クエリを O(1) で処理するデータ構造。
// 通常の Sparse Table と異なり、羃等性 (op(x, x) == x) は不要です (行列積や非可換演算も可)。
//
// 構築: O(N log N)
// クエリ: O(1)
//
// S: 要素の型
// op: 2つの S を結合する二項演算
// e: 単位元を返す関数 (省略時は nullptr。空区間クエリ時に使用)
template <class S, S (*op)(S, S), S (*e)() = nullptr>
struct DisjointSparseTable {
  int n;
  vector<vector<S>> table;

  DisjointSparseTable() : n(0) {}

  explicit DisjointSparseTable(const vector<S> &v) : n(v.size()) {
    if (n == 0)
      return;

    int max_log = 1;
    while ((1 << max_log) <= n)
      max_log++;

    table.assign(max_log, vector<S>(n));
    table[0] = v;

    for (int i = 1; i < max_log; i++) {
      int shift = 1 << i;
      for (int m = shift; m < n; m += (shift << 1)) {
        // m を境界として左へ累積
        table[i][m - 1] = v[m - 1];
        for (int j = m - 2; j >= m - shift && j >= 0; j--) {
          table[i][j] = op(v[j], table[i][j + 1]);
        }
        // m を境界として右へ累積
        table[i][m] = v[m];
        int r_lim = min(n, m + shift);
        for (int j = m + 1; j < r_lim; j++) {
          table[i][j] = op(table[i][j - 1], v[j]);
        }
      }
    }
  }

  // 半開区間 [l, r) の総積を取得 (O(1))
  S prod(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    if (l == r) {
      if constexpr (e != nullptr) {
        return e();
      } else {
        assert(false && "Empty query [l, r) with no identity element provided.");
      }
    }
    int r_idx = r - 1;
    if (l == r_idx) {
      return table[0][l];
    }
    // l と r_idx の最上位の異なるビット (MSB)
    int diff = l ^ r_idx;
    int p = 31 - __builtin_clz(diff);
    return op(table[p][l], table[p][r_idx]);
  }

  // 1点取得 (O(1))
  S get(int p) const {
    assert(0 <= p && p < n);
    return table[0][p];
  }

  S operator[](int p) const { return get(p); }
};
