#pragma once
#include <bits/stdc++.h>

using namespace std;

// 一次不定方程式 a*x + b*y = c の整数解を扱う構造体
struct LinearDiophantine {
  long long a, b, c;
  long long g;
  long long x0, y0; // 特解
  long long dx, dy; // 一般解のステップ： x = x0 + k*dx, y = y0 - k*dy
  bool has_sol;

  static constexpr long long INF = 2e18; // 後方互換用定数

  LinearDiophantine(long long a, long long b, long long c) : a(a), b(b), c(c) {
    solve();
  }

  // 負の数も正しく扱う切り捨て除算 (floor(n / d))
  long long floor_div(long long n, long long d) const {
    return (long long)floor_div128((__int128)n, (__int128)d);
  }

  // 負の数も正しく扱う切り上げ除算 (ceil(n / d))
  long long ceil_div(long long n, long long d) const {
    return (long long)ceil_div128((__int128)n, (__int128)d);
  }

  // 解を k ステップ平行移動させる
  void shift(long long k) {
    x0 = (long long)((__int128)x0 + (__int128)k * (__int128)dx);
    y0 = (long long)((__int128)y0 - (__int128)k * (__int128)dy);
  }

  // x >= 0 を満たす最小の x に現在の解 (x0, y0) を調整する
  // 解が存在しない、または dx == 0 で不可能な場合は false / x0 >= 0 を返す
  bool minimize_x_non_negative() {
    if (!has_sol)
      return false;
    if (dx == 0)
      return x0 >= 0;
    __int128 dx128 = dx;
    __int128 x128 = x0;
    __int128 y128 = y0;
    __int128 dy128 = dy;
    __int128 step = dx128 < 0 ? -dx128 : dx128;
    __int128 rem = (x128 % step + step) % step;
    __int128 k = (rem - x128) / dx128;
    x0 = (long long)(x128 + k * dx128);
    y0 = (long long)(y128 - k * dy128);
    return true;
  }

  // 指定範囲 x in [minx, maxx], y in [miny, maxy] に存在する解の個数を O(1) で数える
  long long count_solutions(long long minx, long long maxx, long long miny,
                            long long maxy) const {
    if (!has_sol || minx > maxx || miny > maxy)
      return 0;

    if (dx == 0 && dy == 0) {
      // a == 0 && b == 0 の場合
      if (a == 0 && b == 0 && c == 0) {
        __int128 cnt = ((__int128)maxx - (__int128)minx + 1) *
                       ((__int128)maxy - (__int128)miny + 1);
        return (long long)cnt;
      }
      if (x0 >= minx && x0 <= maxx && y0 >= miny && y0 <= maxy)
        return 1;
      return 0;
    }

    bool has_min = false, has_max = false;
    __int128 k_min = 0, k_max = 0;
    auto tighten_min = [&](__int128 v) {
      if (!has_min || v > k_min) {
        k_min = v;
        has_min = true;
      }
    };
    auto tighten_max = [&](__int128 v) {
      if (!has_max || v < k_max) {
        k_max = v;
        has_max = true;
      }
    };

    // x = x0 + k * dx からの制約
    if (dx > 0) {
      tighten_min(ceil_div128((__int128)minx - (__int128)x0, (__int128)dx));
      tighten_max(floor_div128((__int128)maxx - (__int128)x0, (__int128)dx));
    } else if (dx < 0) {
      tighten_min(ceil_div128((__int128)maxx - (__int128)x0, (__int128)dx));
      tighten_max(floor_div128((__int128)minx - (__int128)x0, (__int128)dx));
    } else { // dx == 0 (b == 0)
      if (x0 < minx || x0 > maxx)
        return 0;
    }

    // y = y0 - k * dy からの制約
    if (dy > 0) {
      tighten_min(ceil_div128((__int128)y0 - (__int128)maxy, (__int128)dy));
      tighten_max(floor_div128((__int128)y0 - (__int128)miny, (__int128)dy));
    } else if (dy < 0) {
      tighten_min(ceil_div128((__int128)y0 - (__int128)miny, (__int128)dy));
      tighten_max(floor_div128((__int128)y0 - (__int128)maxy, (__int128)dy));
    } else { // dy == 0 (a == 0)
      if (y0 < miny || y0 > maxy)
        return 0;
    }

    if (k_min > k_max)
      return 0;
    return (long long)(k_max - k_min + 1);
  }

private:
  __int128 floor_div128(__int128 n, __int128 d) const {
    assert(d != 0);
    __int128 res = n / d;
    __int128 rem = n % d;
    if (rem != 0 && ((n < 0) ^ (d < 0))) {
      res--;
    }
    return res;
  }

  __int128 ceil_div128(__int128 n, __int128 d) const {
    assert(d != 0);
    __int128 res = n / d;
    __int128 rem = n % d;
    if (rem != 0 && !((n < 0) ^ (d < 0))) {
      res++;
    }
    return res;
  }

  __int128 extGCD128(__int128 a, __int128 b, __int128 &x, __int128 &y) {
    if (b == 0) {
      x = 1;
      y = 0;
      return a;
    }
    __int128 x1, y1;
    __int128 d = extGCD128(b, a % b, x1, y1);
    x = y1;
    y = x1 - y1 * (a / b);
    return d;
  }

  void solve() {
    if (a == 0 && b == 0) {
      if (c == 0) {
        has_sol = true;
        x0 = 0;
        y0 = 0;
        g = 0;
        dx = 0;
        dy = 0;
      } else {
        has_sol = false;
        x0 = 0;
        y0 = 0;
        g = 0;
        dx = 0;
        dy = 0;
      }
      return;
    }
    __int128 ua = a < 0 ? -(__int128)a : (__int128)a;
    __int128 ub = b < 0 ? -(__int128)b : (__int128)b;
    __int128 x, y;
    __int128 g128 = extGCD128(ua, ub, x, y);
    g = (long long)g128;
    if ((__int128)c % g128 != 0) {
      has_sol = false;
      x0 = 0;
      y0 = 0;
      dx = 0;
      dy = 0;
      return;
    }
    has_sol = true;
    if (a < 0)
      x = -x;
    if (b < 0)
      y = -y;

    __int128 dx128 = (__int128)b / g128;
    __int128 dy128 = (__int128)a / g128;
    __int128 mult = (__int128)c / g128;

    __int128 x_sol = 0, y_sol = 0;
    if (dx128 != 0) {
      __int128 step = dx128 < 0 ? -dx128 : dx128;
      x_sol = ((x % step) * (mult % step)) % step;
      if (x_sol < 0)
        x_sol += step;
      y_sol = ((__int128)c - (__int128)a * x_sol) / (__int128)b;
      constexpr __int128 ll_max = numeric_limits<long long>::max();
      constexpr __int128 ll_min = numeric_limits<long long>::min();
      auto fits_ll = [&](__int128 xv, __int128 yv) {
        return xv >= ll_min && xv <= ll_max && yv >= ll_min && yv <= ll_max;
      };
      if (!fits_ll(x_sol, y_sol)) {
        __int128 x_alt = x_sol - step;
        __int128 y_alt = ((__int128)c - (__int128)a * x_alt) / (__int128)b;
        if (fits_ll(x_alt, y_alt)) {
          x_sol = x_alt;
          y_sol = y_alt;
        } else {
          x_alt = x_sol + step;
          y_alt = ((__int128)c - (__int128)a * x_alt) / (__int128)b;
          if (fits_ll(x_alt, y_alt)) {
            x_sol = x_alt;
            y_sol = y_alt;
          }
        }
      }
    } else {
      x_sol = (__int128)c / (__int128)a;
      y_sol = 0;
    }

    x0 = (long long)x_sol;
    y0 = (long long)y_sol;
    dx = (long long)dx128;
    dy = (long long)dy128;
  }
};