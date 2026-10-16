#pragma once
#include <bits/stdc++.h>

using namespace std;

// 羃等性 (idempotent: op(x, x) = x) をもつ半群に対する区間クエリを O(1) で処理する
// テンプレート引数: 要素の型 S, 演算関数ポインタ op (例: min, max, gcd)
template <class S, S (*op)(S, S)> class SparseTable {
private:
  int n;
  vector<vector<S>> table;
  vector<int> log_table;

public:
  // デフォルトコンストラクタ
  SparseTable() : n(0) {}

  // コンストラクタ：vectorを渡して構築 O(N log N)
  SparseTable(const vector<S> &v) : n(v.size()) {
    build(v);
  }

  void build(const vector<S> &v) {
    n = v.size();
    if (n == 0) {
      table.clear();
      log_table.clear();
      return;
    }

    // ログテーブルの事前計算
    log_table.assign(n + 1, 0);
    for (int i = 2; i <= n; i++) {
      log_table[i] = log_table[i >> 1] + 1;
    }

    int max_log = log_table[n];
    table.assign(max_log + 1, vector<S>(n));

    // 初期状態（長さ1の区間）
    for (int i = 0; i < n; i++) {
      table[0][i] = v[i];
    }

    // テーブルの構築
    for (int k = 1; k <= max_log; k++) {
      for (int i = 0; i + (1 << k) <= n; i++) {
        table[k][i] = op(table[k - 1][i], table[k - 1][i + (1 << (k - 1))]);
      }
    }
  }

  // 区間 [l, r) のクエリを処理 O(1)
  S query(int l, int r) const {
    assert(0 <= l && l < r && r <= n);
    int k = log_table[r - l];
    return op(table[k][l], table[k][r - (1 << k)]);
  }
};
