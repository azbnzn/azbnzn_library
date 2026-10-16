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

  static constexpr long long INF = 2e18; // 安全な無限大定数 (double精度落ちを防止)

  LinearDiophantine(long long a, long long b, long long c) : a(a), b(b), c(c) {
    solve();
  }

  // 負の数も正しく扱う切り捨て除算 (floor(n / d))
  long long floor_div(long long n, long long d) const {
    assert(d != 0);
    long long res = n / d;
    long long rem = n % d;
    if (rem != 0 && ((n < 0) ^ (d < 0))) {
      res--;
    }
    return res;
  }

  // 負の数も正しく扱う切り上げ除算 (ceil(n / d))
  long long ceil_div(long long n, long long d) const {
    assert(d != 0);
    long long res = n / d;
    long long rem = n % d;
    if (rem != 0 && !((n < 0) ^ (d < 0))) {
      res++;
    }
    return res;
  }

  // 解を k ステップ平行移動させる
  void shift(long long k) {
    x0 += k * dx;
    y0 -= k * dy;
  }

  // x >= 0 を満たす最小の x に現在の解 (x0, y0) を調整する
  // 解が存在しない、または dx == 0 で不可能な場合は false / x0 >= 0 を返す
  bool minimize_x_non_negative() {
    if (!has_sol)
      return false;
    if (dx == 0)
      return x0 >= 0;
    long long step = abs(dx);
    long long rem = (x0 % step + step) % step;
    long long k = (rem - x0) / dx;
    shift(k);
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
        return (maxx - minx + 1) * (maxy - miny + 1);
      }
      if (x0 >= minx && x0 <= maxx && y0 >= miny && y0 <= maxy)
        return 1;
      return 0;
    }

    long long k_min = -INF;
    long long k_max = INF;

    // x = x0 + k * dx からの制約
    if (dx > 0) {
      k_min = max(k_min, ceil_div(minx - x0, dx));
      k_max = min(k_max, floor_div(maxx - x0, dx));
    } else if (dx < 0) {
      k_min = max(k_min, ceil_div(maxx - x0, dx));
      k_max = min(k_max, floor_div(minx - x0, dx));
    } else { // dx == 0 (b == 0)
      if (x0 < minx || x0 > maxx)
        return 0;
    }

    // y = y0 - k * dy からの制約
    if (dy > 0) {
      k_min = max(k_min, ceil_div(y0 - maxy, dy));
      k_max = min(k_max, floor_div(y0 - miny, dy));
    } else if (dy < 0) {
      k_min = max(k_min, ceil_div(y0 - miny, dy));
      k_max = min(k_max, floor_div(y0 - maxy, dy));
    } else { // dy == 0 (a == 0)
      if (y0 < miny || y0 > maxy)
        return 0;
    }

    if (k_min > k_max)
      return 0;
    return k_max - k_min + 1;
  }

private:
  long long extGCD(long long a, long long b, long long &x, long long &y) {
    if (b == 0) {
      x = 1;
      y = 0;
      return a;
    }
    long long x1, y1;
    long long d = extGCD(b, a % b, x1, y1);
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
    long long x, y;
    g = extGCD(abs(a), abs(b), x, y);
    if (c % g != 0) {
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
    x0 = x * (c / g);
    y0 = y * (c / g);
    dx = b / g;
    dy = a / g;
  }
};