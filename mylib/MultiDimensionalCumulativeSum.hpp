#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

// ============================================================================
// 多次元累積和 & 多次元いもす法 (Multi-Dimensional Prefix Sum & Imos & BIT)
// ============================================================================
// 【概要】
// 任意の次元 D に対する累積和（Prefix Sum）、いもす法（Imos Method）、
// およびオンラインで更新とクエリが交互に走る Binary Indexed Tree (BIT) を提供します。
//
// 【提供クラス】
// 1. 静的一点更新・区間和取得 (Prefix Sum):
//    - MultiDimensionalPrefixSum<T, D>: 点加算 -> build() (O(D * M)) -> 超直方体区間和取得 (O(2^D))
//    - DynamicPrefixSumND<T>: 実行時に次元 D を指定できる動的次元版
//    - PrefixSum2D<T> / PrefixSum3D<T> / PrefixSum1D<T>: 座標配列不要の便利ラッパー
//
// 2. 静的区間更新・一点取得 (Imos Method):
//    - MultiDimensionalImos<T, D>: 超直方体加算 (O(2^D)) -> build() (O(D * M)) -> 一点取得 (O(1))
//    - DynamicImosND<T>: 実行時次元指定版
//    - Imos2D<T> / Imos3D<T> / Imos1D<T>: 座標配列不要の便利ラッパー
//
// 3. 動的一点更新・区間和取得 (Multi-Dimensional BIT / Fenwick Tree):
//    - MultiDimensionalBIT<T, D>: 点加算 (O(prod log N_k)) <-> 超直方体区間和 (O(2^D prod log N_k))
//    - BIT2D<T> / BIT3D<T> / BIT1D<T>: 2D/3D/1D 便利ラッパー
//
// 4. 動的区間更新・一点取得 (Multi-Dimensional Dual BIT):
//    - MultiDimensionalDualBIT<T, D>: 超直方体加算 (O(2^D prod log N_k)) <-> 一点取得 (O(prod log N_k))
//    - DualBIT2D<T> / DualBIT3D<T> / DualBIT1D<T>: 2D/3D/1D 便利ラッパー
//
// 【区間表現】
// 0-indexed 半開区間 [l, r) を標準としています。
// 例 (2D): [r1, r2) x [c1, c2)
// 例 (D次元): 点 l = (l_0, ..., l_{D-1}), 点 r = (r_0, ..., r_{D-1}) に対して
//             [l_0, r_0) x ... x [l_{D-1}, r_{D-1})
// ============================================================================

// ----------------------------------------------------------------------------
// 1. MultiDimensionalPrefixSum<T, D> (静的一点加算・区間和取得)
// ----------------------------------------------------------------------------
template <typename T = long long, size_t D = 2>
class MultiDimensionalPrefixSum {
public:
  using Point = std::array<int, D>;

private:
  Point shape{};
  std::array<int, D + 1> stride{};
  std::vector<T> data;
  bool built = false;

  int flat_idx(const Point &p) const {
    int idx = 0;
    for (size_t k = 0; k < D; ++k) {
      idx += p[k] * stride[k + 1];
    }
    return idx;
  }

public:
  MultiDimensionalPrefixSum() {}

  explicit MultiDimensionalPrefixSum(const Point &shp) : shape(shp) {
    stride[D] = 1;
    for (int k = (int)D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    data.assign(stride[0], T(0));
  }

  MultiDimensionalPrefixSum(std::initializer_list<int> shp_list) {
    assert(shp_list.size() == D);
    size_t idx = 0;
    for (int s : shp_list) shape[idx++] = s;
    stride[D] = 1;
    for (int k = (int)D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    data.assign(stride[0], T(0));
  }

  // 点 p の値を val に設定 (build 前)
  void set(const Point &p, T val) {
    assert(!built);
    Point shifted;
    for (size_t k = 0; k < D; ++k) {
      assert(0 <= p[k] && p[k] < shape[k]);
      shifted[k] = p[k] + 1;
    }
    data[flat_idx(shifted)] = val;
  }

  // 点 p の値に val を加算 (build 前)
  void add(const Point &p, T val) {
    assert(!built);
    Point shifted;
    for (size_t k = 0; k < D; ++k) {
      assert(0 <= p[k] && p[k] < shape[k]);
      shifted[k] = p[k] + 1;
    }
    data[flat_idx(shifted)] += val;
  }

  // 各軸ごとに累積和を走査計算 (O(D * prod N_k))
  void build() {
    assert(!built);
    built = true;
    for (size_t dim = 0; dim < D; ++dim) {
      int step = stride[dim + 1];
      int outer_count = stride[0] / stride[dim];
      int dim_len = shape[dim] + 1;
      for (int outer = 0; outer < outer_count; ++outer) {
        int base = outer * stride[dim];
        for (int coord = 1; coord < dim_len; ++coord) {
          int src = base + (coord - 1) * step;
          int dst = base + coord * step;
          for (int inner = 0; inner < step; ++inner) {
            data[dst + inner] += data[src + inner];
          }
        }
      }
    }
  }

  // 原点から点 r までの半開区間 [0, r_0) x ... x [0, r_{D-1}) の総和 (O(1))
  T prefix_sum(const Point &r) const {
    assert(built);
    Point shifted;
    for (size_t k = 0; k < D; ++k) {
      assert(0 <= r[k] && r[k] <= shape[k]);
      shifted[k] = r[k];
    }
    return data[flat_idx(shifted)];
  }

  // 超直方体 [l_0, r_0) x ... x [l_{D-1}, r_{D-1}) の区間和 (包除原理により O(2^D))
  T sum(const Point &l, const Point &r) const {
    assert(built);
    for (size_t k = 0; k < D; ++k) {
      if (l[k] >= r[k]) return T(0);
      assert(0 <= l[k] && r[k] <= shape[k]);
    }
    T res = 0;
    for (int mask = 0; mask < (1 << D); ++mask) {
      Point coords;
      int popcount = 0;
      for (size_t k = 0; k < D; ++k) {
        if (mask & (1 << k)) {
          coords[k] = l[k];
          popcount++;
        } else {
          coords[k] = r[k];
        }
      }
      T val = data[flat_idx(coords)];
      if (popcount & 1) res -= val;
      else res += val;
    }
    return res;
  }

  // 点 p の元の値を取得 (O(2^D))
  T get(const Point &p) const {
    Point r = p;
    for (size_t k = 0; k < D; ++k) r[k]++;
    return sum(p, r);
  }

  // 全領域の総和 (O(1))
  T all_sum() const {
    return prefix_sum(shape);
  }

  const Point &get_shape() const { return shape; }
};

// ----------------------------------------------------------------------------
// 2. MultiDimensionalImos<T, D> (静的区間更新・一点取得)
// ----------------------------------------------------------------------------
template <typename T = long long, size_t D = 2>
class MultiDimensionalImos {
public:
  using Point = std::array<int, D>;

private:
  Point shape{};
  std::array<int, D + 1> stride{};
  std::vector<T> diff;
  bool built = false;

  int flat_idx(const Point &p) const {
    int idx = 0;
    for (size_t k = 0; k < D; ++k) {
      idx += p[k] * stride[k + 1];
    }
    return idx;
  }

public:
  MultiDimensionalImos() {}

  explicit MultiDimensionalImos(const Point &shp) : shape(shp) {
    stride[D] = 1;
    for (int k = (int)D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    diff.assign(stride[0], T(0));
  }

  MultiDimensionalImos(std::initializer_list<int> shp_list) {
    assert(shp_list.size() == D);
    size_t idx = 0;
    for (int s : shp_list) shape[idx++] = s;
    stride[D] = 1;
    for (int k = (int)D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    diff.assign(stride[0], T(0));
  }

  // 超直方体 [l_0, r_0) x ... x [l_{D-1}, r_{D-1}) に val を加算 (O(2^D))
  void add(const Point &l, const Point &r, T val) {
    assert(!built);
    for (size_t k = 0; k < D; ++k) {
      if (l[k] >= r[k]) return;
      assert(0 <= l[k] && r[k] <= shape[k]);
    }
    for (int mask = 0; mask < (1 << D); ++mask) {
      Point coords;
      int popcount = 0;
      for (size_t k = 0; k < D; ++k) {
        if (mask & (1 << k)) {
          coords[k] = r[k];
          popcount++;
        } else {
          coords[k] = l[k];
        }
      }
      T delta = (popcount & 1) ? -val : val;
      diff[flat_idx(coords)] += delta;
    }
  }

  // 各軸ごとに累積和を走査計算して各点の最終値を確定 (O(D * prod N_k))
  void build() {
    assert(!built);
    built = true;
    for (size_t dim = 0; dim < D; ++dim) {
      int step = stride[dim + 1];
      int outer_count = stride[0] / stride[dim];
      int dim_len = shape[dim] + 1;
      for (int outer = 0; outer < outer_count; ++outer) {
        int base = outer * stride[dim];
        for (int coord = 1; coord < dim_len; ++coord) {
          int src = base + (coord - 1) * step;
          int dst = base + coord * step;
          for (int inner = 0; inner < step; ++inner) {
            diff[dst + inner] += diff[src + inner];
          }
        }
      }
    }
  }

  // 点 p の最終的な加算結果を取得 (O(1))
  T get(const Point &p) const {
    assert(built);
    for (size_t k = 0; k < D; ++k) {
      assert(0 <= p[k] && p[k] < shape[k]);
    }
    return diff[flat_idx(p)];
  }

  const Point &get_shape() const { return shape; }
};

// ----------------------------------------------------------------------------
// 3. MultiDimensionalBIT<T, D> (動的一点加算・区間和取得 Fenwick Tree)
// ----------------------------------------------------------------------------
template <typename T = long long, size_t D = 2>
class MultiDimensionalBIT {
public:
  using Point = std::array<int, D>;

private:
  Point shape{};
  std::array<int, D + 1> stride{};
  std::vector<T> tree;

  template <size_t dim = 0>
  void add_internal(const Point &p, T val, int base_idx) {
    if constexpr (dim == D) {
      tree[base_idx] += val;
    } else {
      int limit = shape[dim];
      int step = stride[dim + 1];
      for (int i = p[dim] + 1; i <= limit; i += i & -i) {
        add_internal<dim + 1>(p, val, base_idx + i * step);
      }
    }
  }

  template <size_t dim = 0>
  T sum_internal(const Point &r, int base_idx) const {
    if constexpr (dim == D) {
      return tree[base_idx];
    } else {
      T res = 0;
      int step = stride[dim + 1];
      for (int i = r[dim]; i > 0; i -= i & -i) {
        res += sum_internal<dim + 1>(r, base_idx + i * step);
      }
      return res;
    }
  }

public:
  MultiDimensionalBIT() {}

  explicit MultiDimensionalBIT(const Point &shp) : shape(shp) {
    stride[D] = 1;
    for (int k = (int)D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    tree.assign(stride[0], T(0));
  }

  MultiDimensionalBIT(std::initializer_list<int> shp_list) {
    assert(shp_list.size() == D);
    size_t idx = 0;
    for (int s : shp_list) shape[idx++] = s;
    stride[D] = 1;
    for (int k = (int)D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    tree.assign(stride[0], T(0));
  }

  // 点 p (0-indexed) に val を加算 (O(prod log N_k))
  void add(const Point &p, T val) {
    for (size_t k = 0; k < D; ++k) {
      assert(0 <= p[k] && p[k] < shape[k]);
    }
    add_internal<0>(p, val, 0);
  }

  // 半開区間 [0, r_0) x ... x [0, r_{D-1}) のプレフィックス和 (O(prod log N_k))
  T prefix_sum(const Point &r) const {
    for (size_t k = 0; k < D; ++k) {
      assert(0 <= r[k] && r[k] <= shape[k]);
    }
    return sum_internal<0>(r, 0);
  }

  // 超直方体 [l_0, r_0) x ... x [l_{D-1}, r_{D-1}) の区間和 (O(2^D prod log N_k))
  T sum(const Point &l, const Point &r) const {
    for (size_t k = 0; k < D; ++k) {
      if (l[k] >= r[k]) return T(0);
      assert(0 <= l[k] && r[k] <= shape[k]);
    }
    T res = 0;
    for (int mask = 0; mask < (1 << D); ++mask) {
      Point coords;
      int popcount = 0;
      for (size_t k = 0; k < D; ++k) {
        if (mask & (1 << k)) {
          coords[k] = l[k];
          popcount++;
        } else {
          coords[k] = r[k];
        }
      }
      T s = prefix_sum(coords);
      if (popcount & 1) res -= s;
      else res += s;
    }
    return res;
  }

  const Point &get_shape() const { return shape; }
};

// ----------------------------------------------------------------------------
// 4. MultiDimensionalDualBIT<T, D> (動的区間更新・一点取得 Dual Fenwick Tree)
// ----------------------------------------------------------------------------
template <typename T = long long, size_t D = 2>
class MultiDimensionalDualBIT {
public:
  using Point = std::array<int, D>;

private:
  Point shape{};
  MultiDimensionalBIT<T, D> bit;

public:
  MultiDimensionalDualBIT() {}

  explicit MultiDimensionalDualBIT(const Point &shp) : shape(shp) {
    Point bit_shape;
    for (size_t k = 0; k < D; ++k) bit_shape[k] = shape[k] + 1;
    bit = MultiDimensionalBIT<T, D>(bit_shape);
  }

  MultiDimensionalDualBIT(std::initializer_list<int> shp_list) {
    assert(shp_list.size() == D);
    size_t idx = 0;
    Point bit_shape;
    for (int s : shp_list) {
      shape[idx] = s;
      bit_shape[idx] = s + 1;
      idx++;
    }
    bit = MultiDimensionalBIT<T, D>(bit_shape);
  }

  // 超直方体 [l_0, r_0) x ... x [l_{D-1}, r_{D-1}) に val を一括加算 (O(2^D prod log N_k))
  void add(const Point &l, const Point &r, T val) {
    for (size_t k = 0; k < D; ++k) {
      if (l[k] >= r[k]) return;
      assert(0 <= l[k] && r[k] <= shape[k]);
    }
    for (int mask = 0; mask < (1 << D); ++mask) {
      Point coords;
      int popcount = 0;
      for (size_t k = 0; k < D; ++k) {
        if (mask & (1 << k)) {
          coords[k] = r[k];
          popcount++;
        } else {
          coords[k] = l[k];
        }
      }
      T delta = (popcount & 1) ? -val : val;
      bit.add(coords, delta);
    }
  }

  // 点 p (0-indexed) の現在の値を取得 (O(prod log N_k))
  T get(const Point &p) const {
    for (size_t k = 0; k < D; ++k) {
      assert(0 <= p[k] && p[k] < shape[k]);
    }
    Point coords;
    for (size_t k = 0; k < D; ++k) coords[k] = p[k] + 1;
    return bit.prefix_sum(coords);
  }

  const Point &get_shape() const { return shape; }
};

// ----------------------------------------------------------------------------
// 5. DynamicPrefixSumND<T> & DynamicImosND<T> (実行時任意次元版)
// ----------------------------------------------------------------------------
template <typename T = long long>
class DynamicPrefixSumND {
private:
  int D = 0;
  std::vector<int> shape;
  std::vector<int> stride;
  std::vector<T> data;
  bool built = false;

  int flat_idx(const std::vector<int> &p) const {
    int idx = 0;
    for (int k = 0; k < D; ++k) {
      idx += p[k] * stride[k + 1];
    }
    return idx;
  }

public:
  DynamicPrefixSumND() {}
  explicit DynamicPrefixSumND(const std::vector<int> &shp) : shape(shp) {
    D = (int)shape.size();
    stride.assign(D + 1, 1);
    for (int k = D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    data.assign(stride[0], T(0));
  }

  void add(const std::vector<int> &p, T val) {
    assert(!built);
    assert((int)p.size() == D);
    std::vector<int> shifted(D);
    for (int k = 0; k < D; ++k) {
      assert(0 <= p[k] && p[k] < shape[k]);
      shifted[k] = p[k] + 1;
    }
    data[flat_idx(shifted)] += val;
  }

  void build() {
    assert(!built);
    built = true;
    for (int dim = 0; dim < D; ++dim) {
      int step = stride[dim + 1];
      int outer_count = stride[0] / stride[dim];
      int dim_len = shape[dim] + 1;
      for (int outer = 0; outer < outer_count; ++outer) {
        int base = outer * stride[dim];
        for (int coord = 1; coord < dim_len; ++coord) {
          int src = base + (coord - 1) * step;
          int dst = base + coord * step;
          for (int inner = 0; inner < step; ++inner) {
            data[dst + inner] += data[src + inner];
          }
        }
      }
    }
  }

  T sum(const std::vector<int> &l, const std::vector<int> &r) const {
    assert(built);
    assert((int)l.size() == D && (int)r.size() == D);
    for (int k = 0; k < D; ++k) {
      if (l[k] >= r[k]) return T(0);
      assert(0 <= l[k] && r[k] <= shape[k]);
    }
    T res = 0;
    for (int mask = 0; mask < (1 << D); ++mask) {
      std::vector<int> coords(D);
      int popcount = 0;
      for (int k = 0; k < D; ++k) {
        if (mask & (1 << k)) {
          coords[k] = l[k];
          popcount++;
        } else {
          coords[k] = r[k];
        }
      }
      T val = data[flat_idx(coords)];
      if (popcount & 1) res -= val;
      else res += val;
    }
    return res;
  }
};

template <typename T = long long>
class DynamicImosND {
private:
  int D = 0;
  std::vector<int> shape;
  std::vector<int> stride;
  std::vector<T> diff;
  bool built = false;

  int flat_idx(const std::vector<int> &p) const {
    int idx = 0;
    for (int k = 0; k < D; ++k) {
      idx += p[k] * stride[k + 1];
    }
    return idx;
  }

public:
  DynamicImosND() {}
  explicit DynamicImosND(const std::vector<int> &shp) : shape(shp) {
    D = (int)shape.size();
    stride.assign(D + 1, 1);
    for (int k = D - 1; k >= 0; --k) {
      stride[k] = stride[k + 1] * (shape[k] + 1);
    }
    diff.assign(stride[0], T(0));
  }

  void add(const std::vector<int> &l, const std::vector<int> &r, T val) {
    assert(!built);
    assert((int)l.size() == D && (int)r.size() == D);
    for (int k = 0; k < D; ++k) {
      if (l[k] >= r[k]) return;
      assert(0 <= l[k] && r[k] <= shape[k]);
    }
    for (int mask = 0; mask < (1 << D); ++mask) {
      std::vector<int> coords(D);
      int popcount = 0;
      for (int k = 0; k < D; ++k) {
        if (mask & (1 << k)) {
          coords[k] = r[k];
          popcount++;
        } else {
          coords[k] = l[k];
        }
      }
      T delta = (popcount & 1) ? -val : val;
      diff[flat_idx(coords)] += delta;
    }
  }

  void build() {
    assert(!built);
    built = true;
    for (int dim = 0; dim < D; ++dim) {
      int step = stride[dim + 1];
      int outer_count = stride[0] / stride[dim];
      int dim_len = shape[dim] + 1;
      for (int outer = 0; outer < outer_count; ++outer) {
        int base = outer * stride[dim];
        for (int coord = 1; coord < dim_len; ++coord) {
          int src = base + (coord - 1) * step;
          int dst = base + coord * step;
          for (int inner = 0; inner < step; ++inner) {
            diff[dst + inner] += diff[src + inner];
          }
        }
      }
    }
  }

  T get(const std::vector<int> &p) const {
    assert(built);
    assert((int)p.size() == D);
    for (int k = 0; k < D; ++k) {
      assert(0 <= p[k] && p[k] < shape[k]);
    }
    return diff[flat_idx(p)];
  }
};

// ============================================================================
// 6. 2次元コンビニエンスラッパー (2D Wrappers)
// ============================================================================
template <typename T = long long>
class PrefixSum2D {
private:
  MultiDimensionalPrefixSum<T, 2> ps;
  int H = 0, W = 0;

public:
  PrefixSum2D() {}
  PrefixSum2D(int h, int w) : ps({h, w}), H(h), W(w) {}

  PrefixSum2D(const std::vector<std::vector<T>> &grid) {
    H = (int)grid.size();
    W = (H == 0 ? 0 : (int)grid[0].size());
    ps = MultiDimensionalPrefixSum<T, 2>({H, W});
    for (int r = 0; r < H; ++r) {
      for (int c = 0; c < W; ++c) {
        ps.set({r, c}, grid[r][c]);
      }
    }
    build();
  }

  void set(int r, int c, T val) { ps.set({r, c}, val); }
  void add(int r, int c, T val) { ps.add({r, c}, val); }
  void build() { ps.build(); }

  // 矩形 [r1, r2) x [c1, c2) の総和 (O(1))
  T sum(int r1, int c1, int r2, int c2) const {
    return ps.sum({r1, c1}, {r2, c2});
  }

  // プレフィックス [0, r) x [0, c) の総和 (O(1))
  T prefix_sum(int r, int c) const {
    return ps.prefix_sum({r, c});
  }

  // 点 (r, c) の値を取得 (O(1))
  T get(int r, int c) const {
    return ps.get({r, c});
  }

  T all_sum() const { return ps.all_sum(); }
  int height() const { return H; }
  int width() const { return W; }
};

template <typename T = long long>
class Imos2D {
private:
  MultiDimensionalImos<T, 2> imos;
  int H = 0, W = 0;

public:
  Imos2D() {}
  Imos2D(int h, int w) : imos({h, w}), H(h), W(w) {}

  // 矩形 [r1, r2) x [c1, c2) に val を加算 (O(1))
  void add(int r1, int c1, int r2, int c2, T val) {
    imos.add({r1, c1}, {r2, c2}, val);
  }

  // 累積和を計算して各マスの値を確定 (O(HW))
  void build() { imos.build(); }

  // マス (r, c) の値を取得 (O(1))
  T get(int r, int c) const {
    return imos.get({r, c});
  }

  std::vector<std::vector<T>> to_grid() const {
    std::vector<std::vector<T>> res(H, std::vector<T>(W));
    for (int r = 0; r < H; ++r) {
      for (int c = 0; c < W; ++c) {
        res[r][c] = imos.get({r, c});
      }
    }
    return res;
  }

  int height() const { return H; }
  int width() const { return W; }
};

template <typename T = long long>
class BIT2D {
private:
  MultiDimensionalBIT<T, 2> bit;
  int H = 0, W = 0;

public:
  BIT2D() {}
  BIT2D(int h, int w) : bit({h, w}), H(h), W(w) {}

  void add(int r, int c, T val) {
    bit.add({r, c}, val);
  }

  T sum(int r1, int c1, int r2, int c2) const {
    return bit.sum({r1, c1}, {r2, c2});
  }

  T prefix_sum(int r, int c) const {
    return bit.prefix_sum({r, c});
  }

  int height() const { return H; }
  int width() const { return W; }
};

template <typename T = long long>
class DualBIT2D {
private:
  MultiDimensionalDualBIT<T, 2> dual_bit;
  int H = 0, W = 0;

public:
  DualBIT2D() {}
  DualBIT2D(int h, int w) : dual_bit({h, w}), H(h), W(w) {}

  void add(int r1, int c1, int r2, int c2, T val) {
    dual_bit.add({r1, c1}, {r2, c2}, val);
  }

  T get(int r, int c) const {
    return dual_bit.get({r, c});
  }

  int height() const { return H; }
  int width() const { return W; }
};

// ============================================================================
// 7. 3次元コンビニエンスラッパー (3D Wrappers)
// ============================================================================
template <typename T = long long>
class PrefixSum3D {
private:
  MultiDimensionalPrefixSum<T, 3> ps;
  int X = 0, Y = 0, Z = 0;

public:
  PrefixSum3D() {}
  PrefixSum3D(int x, int y, int z) : ps({x, y, z}), X(x), Y(y), Z(z) {}

  void set(int x, int y, int z, T val) { ps.set({x, y, z}, val); }
  void add(int x, int y, int z, T val) { ps.add({x, y, z}, val); }
  void build() { ps.build(); }

  T sum(int x1, int y1, int z1, int x2, int y2, int z2) const {
    return ps.sum({x1, y1, z1}, {x2, y2, z2});
  }

  T prefix_sum(int x, int y, int z) const {
    return ps.prefix_sum({x, y, z});
  }

  T get(int x, int y, int z) const {
    return ps.get({x, y, z});
  }

  T all_sum() const { return ps.all_sum(); }
};

template <typename T = long long>
class Imos3D {
private:
  MultiDimensionalImos<T, 3> imos;
  int X = 0, Y = 0, Z = 0;

public:
  Imos3D() {}
  Imos3D(int x, int y, int z) : imos({x, y, z}), X(x), Y(y), Z(z) {}

  void add(int x1, int y1, int z1, int x2, int y2, int z2, T val) {
    imos.add({x1, y1, z1}, {x2, y2, z2}, val);
  }

  void build() { imos.build(); }

  T get(int x, int y, int z) const {
    return imos.get({x, y, z});
  }
};

template <typename T = long long>
class BIT3D {
private:
  MultiDimensionalBIT<T, 3> bit;
  int X = 0, Y = 0, Z = 0;

public:
  BIT3D() {}
  BIT3D(int x, int y, int z) : bit({x, y, z}), X(x), Y(y), Z(z) {}

  void add(int x, int y, int z, T val) {
    bit.add({x, y, z}, val);
  }

  T sum(int x1, int y1, int z1, int x2, int y2, int z2) const {
    return bit.sum({x1, y1, z1}, {x2, y2, z2});
  }
};

template <typename T = long long>
class DualBIT3D {
private:
  MultiDimensionalDualBIT<T, 3> dual_bit;
  int X = 0, Y = 0, Z = 0;

public:
  DualBIT3D() {}
  DualBIT3D(int x, int y, int z) : dual_bit({x, y, z}), X(x), Y(y), Z(z) {}

  void add(int x1, int y1, int z1, int x2, int y2, int z2, T val) {
    dual_bit.add({x1, y1, z1}, {x2, y2, z2}, val);
  }

  T get(int x, int y, int z) const {
    return dual_bit.get({x, y, z});
  }
};

// ============================================================================
// 8. 1次元コンビニエンスラッパー (1D Wrappers)
// ============================================================================
template <typename T = long long>
using PrefixSum1D = MultiDimensionalPrefixSum<T, 1>;

template <typename T = long long>
using Imos1D = MultiDimensionalImos<T, 1>;

template <typename T = long long>
using BIT1D = MultiDimensionalBIT<T, 1>;

template <typename T = long long>
using DualBIT1D = MultiDimensionalDualBIT<T, 1>;

// 汎用エイリアス
template <typename T = long long, size_t D = 2>
using PrefixSumND = MultiDimensionalPrefixSum<T, D>;

template <typename T = long long, size_t D = 2>
using ImosND = MultiDimensionalImos<T, D>;

template <typename T = long long, size_t D = 2>
using FenwickTreeND = MultiDimensionalBIT<T, D>;

template <typename T = long long, size_t D = 2>
using DualFenwickTreeND = MultiDimensionalDualBIT<T, D>;
