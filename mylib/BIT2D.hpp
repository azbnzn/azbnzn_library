#pragma once
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

// ============================================================================
// 動的2次元 BIT (2D Binary Indexed Tree / Fenwick Tree)
// ============================================================================
// 【概要】
// 2次元グリッド H x W に対し、オンライン（動的）に更新クエリと区間和クエリを
// 高速に処理するデータ構造です。
//
// 【提供クラス】
// 1. BIT2D<T>:
//    - 点加算 add(r, c, val) (O(log H log W))
//    - 矩形和 sum(r1, c1, r2, c2) (O(log H log W))
//    - プレフィックス和 prefix_sum(r, c) (O(log H log W))
//    - 1点取得 get(r, c) (O(log H log W))
//
// 2. DualBIT2D<T>: (双対2次元 BIT)
//    - 矩形加算 add(r1, c1, r2, c2, val) (O(log H log W))
//    - 1点取得 get(r, c) (O(log H log W))
//
// 3. RangeAddBIT2D<T>: (領域加算・領域和取得 RAQ - 4本の BIT を使用)
//    - 矩形加算 add(r1, c1, r2, c2, val) (O(log H log W))
//    - 矩形和 sum(r1, c1, r2, c2) (O(log H log W))
//
// ※すべての区間は 0-indexed 半開区間 [r1, r2) x [c1, c2) です。
// ============================================================================

// ----------------------------------------------------------------------------
// 1. BIT2D (点加算・矩形和取得)
// ----------------------------------------------------------------------------
template <typename T = long long>
class BIT2D {
private:
  int H = 0, W = 0;
  std::vector<T> tree;

  inline int id(int r, int c) const {
    return r * (W + 1) + c;
  }

public:
  BIT2D() {}
  BIT2D(int h, int w) : H(h), W(w), tree((h + 1) * (w + 1), T(0)) {}

  // 点 (r, c) に val を加算 (O(log H log W))
  void add(int r, int c, T val) {
    assert(0 <= r && r < H && 0 <= c && c < W);
    for (int i = r + 1; i <= H; i += i & -i) {
      for (int j = c + 1; j <= W; j += j & -j) {
        tree[id(i, j)] += val;
      }
    }
  }

  // 半開区間 [0, r) x [0, c) のプレフィックス総和 (O(log H log W))
  T prefix_sum(int r, int c) const {
    r = std::max(0, std::min(r, H));
    c = std::max(0, std::min(c, W));
    T res = 0;
    for (int i = r; i > 0; i -= i & -i) {
      for (int j = c; j > 0; j -= j & -j) {
        res += tree[id(i, j)];
      }
    }
    return res;
  }

  // 矩形区間 [r1, r2) x [c1, c2) の総和 (O(log H log W))
  T sum(int r1, int c1, int r2, int c2) const {
    if (r1 >= r2 || c1 >= c2) return T(0);
    assert(0 <= r1 && r2 <= H && 0 <= c1 && c2 <= W);
    return prefix_sum(r2, c2) - prefix_sum(r1, c2) - prefix_sum(r2, c1) + prefix_sum(r1, c1);
  }

  // 点 (r, c) の現在の値を取得 (O(log H log W))
  T get(int r, int c) const {
    return sum(r, c, r + 1, c + 1);
  }

  int height() const { return H; }
  int width() const { return W; }
};

// ----------------------------------------------------------------------------
// 2. DualBIT2D (矩形加算・一点取得 / 双対 2D BIT)
// ----------------------------------------------------------------------------
template <typename T = long long>
class DualBIT2D {
private:
  int H = 0, W = 0;
  BIT2D<T> bit;

public:
  DualBIT2D() {}
  DualBIT2D(int h, int w) : H(h), W(w), bit(h + 1, w + 1) {}

  // 矩形区間 [r1, r2) x [c1, c2) に val を加算 (O(log H log W))
  void add(int r1, int c1, int r2, int c2, T val) {
    if (r1 >= r2 || c1 >= c2) return;
    assert(0 <= r1 && r2 <= H && 0 <= c1 && c2 <= W);
    bit.add(r1, c1, val);
    bit.add(r1, c2, -val);
    bit.add(r2, c1, -val);
    bit.add(r2, c2, val);
  }

  // 点 (r, c) の現在の値を取得 (O(log H log W))
  T get(int r, int c) const {
    assert(0 <= r && r < H && 0 <= c && c < W);
    return bit.prefix_sum(r + 1, c + 1);
  }

  int height() const { return H; }
  int width() const { return W; }
};

// ----------------------------------------------------------------------------
// 3. RangeAddBIT2D (矩形加算・矩形和取得 RAQ / 4本の BIT を使用)
// ----------------------------------------------------------------------------
template <typename T = long long>
class RangeAddBIT2D {
private:
  int H = 0, W = 0;
  BIT2D<T> bit_xy, bit_x, bit_y, bit_0;

  void add_point(int r, int c, T val) {
    bit_xy.add(r, c, val);
    bit_x.add(r, c, -val * c);
    bit_y.add(r, c, -val * r);
    bit_0.add(r, c, val * r * c);
  }

public:
  RangeAddBIT2D() {}
  RangeAddBIT2D(int h, int w)
      : H(h), W(w),
        bit_xy(h + 1, w + 1), bit_x(h + 1, w + 1),
        bit_y(h + 1, w + 1), bit_0(h + 1, w + 1) {}

  // 矩形区間 [r1, r2) x [c1, c2) に val を一括加算 (O(log H log W))
  void add(int r1, int c1, int r2, int c2, T val) {
    if (r1 >= r2 || c1 >= c2) return;
    assert(0 <= r1 && r2 <= H && 0 <= c1 && c2 <= W);
    add_point(r1, c1, val);
    add_point(r1, c2, -val);
    add_point(r2, c1, -val);
    add_point(r2, c2, val);
  }

  // 半開区間 [0, r) x [0, c) のプレフィックス総和 (O(log H log W))
  T prefix_sum(int r, int c) const {
    r = std::max(0, std::min(r, H));
    c = std::max(0, std::min(c, W));
    return bit_xy.prefix_sum(r, c) * r * c +
           bit_x.prefix_sum(r, c) * r +
           bit_y.prefix_sum(r, c) * c +
           bit_0.prefix_sum(r, c);
  }

  // 矩形区間 [r1, r2) x [c1, c2) の総和 (O(log H log W))
  T sum(int r1, int c1, int r2, int c2) const {
    if (r1 >= r2 || c1 >= c2) return T(0);
    assert(0 <= r1 && r2 <= H && 0 <= c1 && c2 <= W);
    return prefix_sum(r2, c2) - prefix_sum(r1, c2) - prefix_sum(r2, c1) + prefix_sum(r1, c1);
  }

  // 点 (r, c) の現在の値を取得 (O(log H log W))
  T get(int r, int c) const {
    return sum(r, c, r + 1, c + 1);
  }

  int height() const { return H; }
  int width() const { return W; }
};

template <typename T = long long>
using FenwickTree2D = BIT2D<T>;
