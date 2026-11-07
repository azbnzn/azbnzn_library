#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

// ============================================================================
// 多次元累積和 & 多次元いもす法 (3次元以上 / Multi-Dimensional Prefix Sum & Imos)
// ============================================================================
// 【概要】
// 3次元以上の空間（D >= 3）に対する累積和（Prefix Sum）およびいもす法（Imos Method）
// を提供します。
//
// 【提供クラス】
// 1. 静的一点加算・超直方体区間和取得 (Prefix Sum):
//    - MultiDimensionalPrefixSum<T, D = 3>: テンプレート次元 D (D >= 3)
//      各軸ごとの走査構築 O(D * prod N_k) -> 包除原理による超直方体区間和取得 O(2^D)
//    - DynamicPrefixSumND<T>: 実行時に次元数を指定できる動的次元版 (D >= 3)
//    - PrefixSum3D<T>: 座標配列を作らず (x, y, z) で直接呼べる 3次元専用ラッパー
//
// 2. 静的区間更新・一点取得 (Imos Method):
//    - MultiDimensionalImos<T, D = 3>: テンプレート次元 D (D >= 3)
//      超直方体加算 O(2^D) -> 各軸ごとの走査構築 O(D * prod N_k) -> 各マス最終値取得 O(1)
//    - DynamicImosND<T>: 実行時次元指定版 (D >= 3)
//    - Imos3D<T>: 座標配列を作らず (x, y, z) で直接呼べる 3次元専用ラッパー
//
// ※すべての区間は 0-indexed 半開区間 [l, r) です。
//   超直方体: [l_0, r_0) x [l_1, r_1) x ... x [l_{D-1}, r_{D-1})
// ============================================================================

// ----------------------------------------------------------------------------
// 1. MultiDimensionalPrefixSum<T, D> (静的一点加算・超直方体区間和取得, D >= 3)
// ----------------------------------------------------------------------------
template <typename T = long long, size_t D = 3>
class MultiDimensionalPrefixSum {
  static_assert(D >= 3, "MultiDimensionalPrefixSum is intended for D >= 3.");

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
// 2. MultiDimensionalImos<T, D> (静的区間更新・一点取得, D >= 3)
// ----------------------------------------------------------------------------
template <typename T = long long, size_t D = 3>
class MultiDimensionalImos {
  static_assert(D >= 3, "MultiDimensionalImos is intended for D >= 3.");

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
// 3. DynamicPrefixSumND<T> & DynamicImosND<T> (実行時任意多次元版, D >= 3)
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
    assert(D >= 3 && "DynamicPrefixSumND is intended for D >= 3.");
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
    assert(D >= 3 && "DynamicImosND is intended for D >= 3.");
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
// 4. 3次元コンビニエンスラッパー (3D Wrappers)
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

  // 直方体 [x1, x2) x [y1, y2) x [z1, z2) の総和 (O(1))
  T sum(int x1, int y1, int z1, int x2, int y2, int z2) const {
    return ps.sum({x1, y1, z1}, {x2, y2, z2});
  }

  // プレフィックス [0, x) x [0, y) x [0, z) の総和 (O(1))
  T prefix_sum(int x, int y, int z) const {
    return ps.prefix_sum({x, y, z});
  }

  // 点 (x, y, z) の値を取得 (O(1))
  T get(int x, int y, int z) const {
    return ps.get({x, y, z});
  }

  T all_sum() const { return ps.all_sum(); }
  int size_x() const { return X; }
  int size_y() const { return Y; }
  int size_z() const { return Z; }
};

template <typename T = long long>
class Imos3D {
private:
  MultiDimensionalImos<T, 3> imos;
  int X = 0, Y = 0, Z = 0;

public:
  Imos3D() {}
  Imos3D(int x, int y, int z) : imos({x, y, z}), X(x), Y(y), Z(z) {}

  // 直方体 [x1, x2) x [y1, y2) x [z1, z2) に val を加算 (O(1))
  void add(int x1, int y1, int z1, int x2, int y2, int z2, T val) {
    imos.add({x1, y1, z1}, {x2, y2, z2}, val);
  }

  // 累積和を計算して各マスの値を確定 (O(XYZ))
  void build() { imos.build(); }

  // 点 (x, y, z) の値を取得 (O(1))
  T get(int x, int y, int z) const {
    return imos.get({x, y, z});
  }

  int size_x() const { return X; }
  int size_y() const { return Y; }
  int size_z() const { return Z; }
};

// 汎用エイリアス
template <typename T = long long, size_t D = 3>
using PrefixSumND = MultiDimensionalPrefixSum<T, D>;

template <typename T = long long, size_t D = 3>
using ImosND = MultiDimensionalImos<T, D>;
