#pragma once
#include <cassert>
#include <type_traits>
#include <utility>

using namespace std;

// ============================================================================
// 三分探索 (Ternary Search)
// ============================================================================
// ・下に凸 / 上に凸な単峰性関数 f(x) の極値 (最小値/最大値) を探索する。
// ・実数版: 反復回数固定 (デフォルト 100 回、精度 ~ (2/3)^100 ≈ 2.4e-18)。
// ・整数版: 幅が 2 以下になるまで 3 分割し、残った数点を全探索して境界バグを防止。
//
// 使用例:
//   double x = ternary_search_real(0.0, 10.0, [&](double x) { return (x - 2.5) * (x - 2.5); });
//   long long k = ternary_search_int(-100LL, 100LL, [&](long long x) { return (x - 7) * (x - 7); });
// ============================================================================

// --- 1. 実数版 (最小値) ---
template <typename T1, typename T2, typename F>
inline auto ternary_search_real(T1 low_in, T2 high_in, F f, int iter = 100) {
  using Real = common_type_t<T1, T2, double>;
  Real low = (Real)low_in;
  Real high = (Real)high_in;
  assert(low <= high);
  for (int i = 0; i < iter; ++i) {
    Real m1 = low + (high - low) / (Real)3.0;
    Real m2 = high - (high - low) / (Real)3.0;
    if (f(m1) < f(m2)) {
      high = m2;
    } else {
      low = m1;
    }
  }
  return (low + high) / (Real)2.0;
}

template <typename T1, typename T2, typename F>
inline auto ternary_search_min_pair_real(T1 low_in, T2 high_in, F f,
                                         int iter = 100) {
  auto x = ternary_search_real(low_in, high_in, f, iter);
  return make_pair(x, f(x));
}

// --- 2. 実数版 (最大値・上に凸) ---
template <typename T1, typename T2, typename F>
inline auto ternary_search_max_real(T1 low_in, T2 high_in, F f,
                                    int iter = 100) {
  using Real = common_type_t<T1, T2, double>;
  Real low = (Real)low_in;
  Real high = (Real)high_in;
  assert(low <= high);
  for (int i = 0; i < iter; ++i) {
    Real m1 = low + (high - low) / (Real)3.0;
    Real m2 = high - (high - low) / (Real)3.0;
    if (f(m1) > f(m2)) {
      high = m2;
    } else {
      low = m1;
    }
  }
  return (low + high) / (Real)2.0;
}

template <typename T1, typename T2, typename F>
inline auto ternary_search_max_pair_real(T1 low_in, T2 high_in, F f,
                                         int iter = 100) {
  auto x = ternary_search_max_real(low_in, high_in, f, iter);
  return make_pair(x, f(x));
}

// --- 3. 整数版 (最小値) ---
template <typename Int1, typename Int2, typename F>
inline auto ternary_search_min_pair_int(Int1 low_in, Int2 high_in, F f) {
  using Int = common_type_t<Int1, Int2>;
  Int low = (Int)low_in;
  Int high = (Int)high_in;
  assert(low <= high);
  while ((__int128)high - (__int128)low > 2) {
    __int128 d = ((__int128)high - (__int128)low) / 3;
    Int m1 = (Int)((__int128)low + d);
    Int m2 = (Int)((__int128)high - d);
    if (f(m1) < f(m2)) {
      high = m2;
    } else {
      low = m1;
    }
  }
  Int best_x = low;
  auto best_val = f(low);
  for (Int x = low + 1; x <= high; ++x) {
    auto val = f(x);
    if (val < best_val) {
      best_val = val;
      best_x = x;
    }
  }
  return make_pair(best_x, best_val);
}

template <typename Int1, typename Int2, typename F>
inline auto ternary_search_int(Int1 low_in, Int2 high_in, F f) {
  return ternary_search_min_pair_int(low_in, high_in, f).first;
}

// --- 4. 整数版 (最大値・上に凸) ---
template <typename Int1, typename Int2, typename F>
inline auto ternary_search_max_pair_int(Int1 low_in, Int2 high_in, F f) {
  using Int = common_type_t<Int1, Int2>;
  Int low = (Int)low_in;
  Int high = (Int)high_in;
  assert(low <= high);
  while ((__int128)high - (__int128)low > 2) {
    __int128 d = ((__int128)high - (__int128)low) / 3;
    Int m1 = (Int)((__int128)low + d);
    Int m2 = (Int)((__int128)high - d);
    if (f(m1) > f(m2)) {
      high = m2;
    } else {
      low = m1;
    }
  }
  Int best_x = low;
  auto best_val = f(low);
  for (Int x = low + 1; x <= high; ++x) {
    auto val = f(x);
    if (val > best_val) {
      best_val = val;
      best_x = x;
    }
  }
  return make_pair(best_x, best_val);
}

template <typename Int1, typename Int2, typename F>
inline auto ternary_search_max_int(Int1 low_in, Int2 high_in, F f) {
  return ternary_search_max_pair_int(low_in, high_in, f).first;
}
