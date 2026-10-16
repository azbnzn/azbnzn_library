#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

// --- 半環構造の定義 ---

// 通常の四則演算（modint や通常の整数・小数向け）
template <typename T> struct StandardSemiring {
  static T zero() { return T(0); }
  static T one() { return T(1); }
  static T add(const T &a, const T &b) { return a + b; }
  static T mul(const T &a, const T &b) { return a * b; }
  static bool is_zero(const T &a) { return a == zero(); }
};

// トロピカル半環 (min, +) : 最短経路・最小コストDP用
template <typename T, T INF> struct MinPlusSemiring {
  static T zero() { return INF; } // 加法の単位元
  static T one() { return T(0); } // 乗法の単位元
  static T add(const T &a, const T &b) { return min(a, b); }
  static T mul(const T &a, const T &b) {
    if (a == INF || b == INF)
      return INF;
    return a + b;
  }
  static bool is_zero(const T &a) { return a == INF; }
};

// トロピカル半環 (max, +) : 最長経路・最大スコアDP用
template <typename T, T NEG_INF> struct MaxPlusSemiring {
  static T zero() { return NEG_INF; }
  static T one() { return T(0); }
  static T add(const T &a, const T &b) { return max(a, b); }
  static T mul(const T &a, const T &b) {
    if (a == NEG_INF || b == NEG_INF)
      return NEG_INF;
    return a + b;
  }
  static bool is_zero(const T &a) { return a == NEG_INF; }
};

// ブール半環 (OR, AND) : グラフの到達可能性・推移閉包用
struct BooleanSemiring {
  static bool zero() { return false; }
  static bool one() { return true; }
  static bool add(const bool &a, const bool &b) { return a || b; }
  static bool mul(const bool &a, const bool &b) { return a && b; }
  static bool is_zero(const bool &a) { return !a; }
};

// 1. スカラー向け (各要素が 0 または 1 の GF(2) 上の線形代数)
template <typename T = int> struct XorAndSemiring {
  static T zero() { return T(0); }
  static T one() { return T(1); }
  static T add(const T &a, const T &b) { return a ^ b; }
  static T mul(const T &a, const T &b) { return a & b; }
  static bool is_zero(const T &a) { return a == T(0); }
};

// 2. ビット並列向け (unsigned long long 等で全ビット独立に並列計算)
template <typename T = unsigned long long> struct BitwiseXorAndSemiring {
  static T zero() { return T(0); }
  static T one() { return ~T(0); } // 全ビットが 1 (AND の単位元)
  static T add(const T &a, const T &b) { return a ^ b; }
  static T mul(const T &a, const T &b) { return a & b; }
  static bool is_zero(const T &a) { return a == T(0); }
};

// --- 行列クラス ---

template <typename T, typename Semiring = StandardSemiring<T>> struct Matrix {
  int h, w;
  vector<vector<T>> mat;

  Matrix() : h(0), w(0) {}
  Matrix(int h, int w, T val = Semiring::zero())
      : h(h), w(w), mat(h, vector<T>(w, val)) {}
  Matrix(const vector<vector<T>> &d)
      : h(d.size()), w(d.empty() ? 0 : d[0].size()), mat(d) {}

  static Matrix identity(int n) {
    Matrix res(n, n, Semiring::zero());
    for (int i = 0; i < n; ++i)
      res[i][i] = Semiring::one();
    return res;
  }

  const vector<T> &operator[](int i) const { return mat[i]; }
  vector<T> &operator[](int i) { return mat[i]; }

  // 行列積 (i -> k -> j 最適化 & ゼロ元スキップ)
  Matrix operator*(const Matrix &r) const {
    assert(w == r.h);
    Matrix res(h, r.w, Semiring::zero());
    for (int i = 0; i < h; ++i) {
      for (int k = 0; k < w; ++k) {
        if (Semiring::is_zero(mat[i][k]))
          continue;
        for (int j = 0; j < r.w; ++j) {
          res.mat[i][j] = Semiring::add(res.mat[i][j],
                                        Semiring::mul(mat[i][k], r.mat[k][j]));
        }
      }
    }
    return res;
  }

  Matrix &operator*=(const Matrix &r) { return *this = *this * r; }

  // 行列累乗 (繰り返し二乗法)
  Matrix pow(long long k) const {
    assert(h == w && k >= 0);
    Matrix res = Matrix::identity(h);
    Matrix a = *this;
    while (k > 0) {
      if (k & 1)
        res *= a;
      a *= a;
      k >>= 1;
    }
    return res;
  }

  // 行列 × ベクトル
  vector<T> operator*(const vector<T> &v) const {
    assert(w == (int)v.size());
    vector<T> res(h, Semiring::zero());
    for (int i = 0; i < h; ++i) {
      for (int j = 0; j < w; ++j) {
        if (Semiring::is_zero(mat[i][j]) || Semiring::is_zero(v[j]))
          continue;
        res[i] = Semiring::add(res[i], Semiring::mul(mat[i][j], v[j]));
      }
    }
    return res;
  }
};