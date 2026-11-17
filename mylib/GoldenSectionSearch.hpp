#pragma once
#include <cassert>
#include <cmath>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// 黄金分割探索 / フィボナッチ探索 (Golden Section Search)
// ============================================================================
// ・三分探索と同じく単峰性関数の極値を求めるが、前回計算した評価値を再利用することで
//   各ステップの関数呼び出し回数を「1回」に抑える (関数評価コストを約 40〜50% 削減)。
// ・整数版では厳密な離散版であるフィボナッチ探索を用い、すべての整数点を最大 1 回のみ評価する。
//
// 使用例:
//   double x = golden_section_search_real(-10.0, 10.0, f, 80);
//   long long k = golden_section_search_int(0LL, 1000000LL, f_int);
// ============================================================================

namespace gss_internal {

template <typename Real, typename F, bool Maximize>
inline auto run_real(Real low, Real high, F f, int iter) {
  assert(low <= high);
  const Real phi = ((Real)1.0 + sqrt((Real)5.0)) / (Real)2.0;
  const Real r = (Real)1.0 / phi;

  Real x1 = high - r * (high - low);
  Real x2 = low + r * (high - low);
  auto f1 = f(x1);
  auto f2 = f(x2);

  for (int i = 0; i < iter; ++i) {
    bool go_left = Maximize ? (f1 > f2) : (f1 < f2);
    if (go_left) {
      high = x2;
      x2 = x1;
      f2 = f1;
      x1 = high - r * (high - low);
      f1 = f(x1);
    } else {
      low = x1;
      x1 = x2;
      f1 = f2;
      x2 = low + r * (high - low);
      f2 = f(x2);
    }
  }
  return (low + high) / (Real)2.0;
}

template <typename Int, typename F, bool Maximize>
inline auto run_int(Int low, Int high, F f) {
  assert(low <= high);
  __int128 n = (__int128)high - (__int128)low + 1;
  if (n <= 1) {
    return make_pair(low, f(low));
  }

  vector<__int128> fib = {0, 1, 1};
  while (fib.back() < n + 1) {
    fib.push_back(fib.back() + fib[fib.size() - 2]);
  }

  int k = (int)fib.size() - 1; // k >= 4
  __int128 L = -1;
  __int128 m1 = L + fib[k - 2];
  __int128 m2 = L + fib[k - 1];

  auto f1 = f((Int)((__int128)low + m1));
  using Val = decltype(f1);
  Val f2{};
  bool has_f2 = (m2 < n);
  if (has_f2) {
    f2 = f((Int)((__int128)low + m2));
  }

  Int best_x = (Int)((__int128)low + m1);
  Val best_val = f1;
  if (has_f2) {
    bool better = Maximize ? (f2 > best_val) : (f2 < best_val);
    if (better) {
      best_val = f2;
      best_x = (Int)((__int128)low + m2);
    }
  }

  while (k > 3) {
    bool go_left = !has_f2 || (Maximize ? (f1 > f2) : (f1 < f2));
    k--;
    if (go_left) {
      m2 = m1;
      f2 = f1;
      has_f2 = (m2 < n);
      m1 = L + fib[k - 2];
      if (k > 3) {
        f1 = f((Int)((__int128)low + m1));
        bool better = Maximize ? (f1 > best_val) : (f1 < best_val);
        if (better) {
          best_val = f1;
          best_x = (Int)((__int128)low + m1);
        }
      }
    } else {
      L = m1;
      m1 = m2;
      f1 = f2;
      m2 = L + fib[k - 1];
      has_f2 = (m2 < n);
      if (k > 3 && has_f2) {
        f2 = f((Int)((__int128)low + m2));
        bool better = Maximize ? (f2 > best_val) : (f2 < best_val);
        if (better) {
          best_val = f2;
          best_x = (Int)((__int128)low + m2);
        }
      }
    }
  }
  return make_pair(best_x, best_val);
}

} // namespace gss_internal

// --- 1. 実数版 (最小値) ---
template <typename T1, typename T2, typename F>
inline auto golden_section_search_real(T1 low, T2 high, F f, int iter = 100) {
  using Real = common_type_t<T1, T2, double>;
  return gss_internal::run_real<Real, F, false>((Real)low, (Real)high, f, iter);
}

template <typename T1, typename T2, typename F>
inline auto golden_section_search_min_pair_real(T1 low, T2 high, F f,
                                                int iter = 100) {
  auto x = golden_section_search_real(low, high, f, iter);
  return make_pair(x, f(x));
}

// --- 2. 実数版 (最大値・上に凸) ---
template <typename T1, typename T2, typename F>
inline auto golden_section_search_max_real(T1 low, T2 high, F f,
                                           int iter = 100) {
  using Real = common_type_t<T1, T2, double>;
  return gss_internal::run_real<Real, F, true>((Real)low, (Real)high, f, iter);
}

template <typename T1, typename T2, typename F>
inline auto golden_section_search_max_pair_real(T1 low, T2 high, F f,
                                                int iter = 100) {
  auto x = golden_section_search_max_real(low, high, f, iter);
  return make_pair(x, f(x));
}

// --- 3. 整数版 (最小値 / フィボナッチ探索) ---
template <typename Int1, typename Int2, typename F>
inline auto golden_section_search_min_pair_int(Int1 low, Int2 high, F f) {
  using Int = common_type_t<Int1, Int2>;
  return gss_internal::run_int<Int, F, false>((Int)low, (Int)high, f);
}

template <typename Int1, typename Int2, typename F>
inline auto golden_section_search_int(Int1 low, Int2 high, F f) {
  return golden_section_search_min_pair_int(low, high, f).first;
}

// --- 4. 整数版 (最大値・上に凸 / フィボナッチ探索) ---
template <typename Int1, typename Int2, typename F>
inline auto golden_section_search_max_pair_int(Int1 low, Int2 high, F f) {
  using Int = common_type_t<Int1, Int2>;
  return gss_internal::run_int<Int, F, true>((Int)low, (Int)high, f);
}

template <typename Int1, typename Int2, typename F>
inline auto golden_section_search_max_int(Int1 low, Int2 high, F f) {
  return golden_section_search_max_pair_int(low, high, f).first;
}
