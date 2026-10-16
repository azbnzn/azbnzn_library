#pragma once
#include <bits/stdc++.h>

using namespace std;

// 順列・組合せを計算するライブラリ
template <class mint> struct BiCoef {
  vector<mint> fact_, inv_fact_;

  // デフォルトコンストラクタ
  BiCoef() {}

  // コンストラクタで n までの階乗とその逆元を事前計算 (計算量: O(n))
  BiCoef(int n) { init(n); }

  // 初期化・再構築
  void init(int n) {
    if (n < 0) return;
    fact_.assign(n + 1, 1);
    inv_fact_.assign(n + 1, 1);
    for (int i = 1; i <= n; i++) {
      fact_[i] = fact_[i - 1] * i;
    }
    // ACLのmodintは .inv() メソッドを持つのでそのまま呼べる
    inv_fact_[n] = fact_[n].inv();
    // 逆元の計算は後ろからやると高速 (O(1)回の逆元計算で済む)
    for (int i = n; i > 0; i--) {
      inv_fact_[i - 1] = inv_fact_[i] * i;
    }
  }

  // 階乗 n!
  mint fact(int n) const {
    if (n < 0 || n >= (int)fact_.size())
      return 0;
    return fact_[n];
  }

  // 階乗の逆元 1 / n!
  mint inv_fact(int n) const {
    if (n < 0 || n >= (int)inv_fact_.size())
      return 0;
    return inv_fact_[n];
  }

  // 順列 nPr
  mint P(int n, int r) const {
    if (n < 0 || n < r || r < 0 || n >= (int)fact_.size())
      return 0;
    return fact_[n] * inv_fact_[n - r];
  }

  // 組合せ nCr
  mint C(int n, int r) const {
    if (n < 0 || n < r || r < 0 || n >= (int)fact_.size())
      return 0;
    return fact_[n] * inv_fact_[r] * inv_fact_[n - r];
  }

  // 重複組合せ nHr (n種類の中から重複を許してr個選ぶ)
  mint H(int n, int r) const {
    if (n < 0 || r < 0)
      return 0;
    if (n == 0 && r == 0)
      return 1;
    return C(n + r - 1, r);
  }
};
