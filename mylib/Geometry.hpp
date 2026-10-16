#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

// ====================================================================
// 2次元計算幾何ライブラリ (2D Computational Geometry Library)
// ====================================================================

// --- 1. 基本設定と型定義 ---
using Real = double;
const Real EPS = 1e-9;
const Real PI = acos(-1.0);

// 浮動小数点数の微小な誤差(EPS)を考慮した符号判定
inline int sgn(Real a) {
  if (a < -EPS)
    return -1;
  if (a > EPS)
    return 1;
  return 0;
}

// 2つの実数がほぼ等しいか判定
inline bool equals(Real a, Real b) { return sgn(a - b) == 0; }

// 値を [low, high] の範囲に安全に収める (acos 等の NaN 防止)
inline Real clamp_val(Real x, Real low = -1.0, Real high = 1.0) {
  return max(low, min(high, x));
}

// --- 2. Point (点 / ベクトル) 構造体 ---
struct Point {
  Real x, y;

  Point(Real x = 0.0, Real y = 0.0) : x(x), y(y) {}

  // 単項演算子
  Point operator-() const { return Point(-x, -y); }

  // 複合代入演算子
  Point &operator+=(const Point &p) {
    x += p.x;
    y += p.y;
    return *this;
  }
  Point &operator-=(const Point &p) {
    x -= p.x;
    y -= p.y;
    return *this;
  }
  Point &operator*=(Real k) {
    x *= k;
    y *= k;
    return *this;
  }
  Point &operator/=(Real k) {
    assert(sgn(k) != 0);
    x /= k;
    y /= k;
    return *this;
  }

  // 二項演算子
  Point operator+(const Point &p) const { return Point(*this) += p; }
  Point operator-(const Point &p) const { return Point(*this) -= p; }
  Point operator*(Real k) const { return Point(*this) *= k; }
  Point operator/(Real k) const { return Point(*this) /= k; }

  // 比較演算子 (誤差を考慮)
  bool operator==(const Point &p) const {
    return sgn(x - p.x) == 0 && sgn(y - p.y) == 0;
  }
  bool operator!=(const Point &p) const { return !(*this == p); }

  // 辞書式順序 (X優先、同じならY)
  bool operator<(const Point &p) const {
    if (sgn(x - p.x) != 0)
      return sgn(x - p.x) < 0;
    return sgn(y - p.y) < 0;
  }
  bool operator>(const Point &p) const { return p < *this; }

  // 原点中心に反時計回りに theta ラジアン回転
  Point rotate(Real theta) const {
    return Point(x * cos(theta) - y * sin(theta),
                 x * sin(theta) + y * cos(theta));
  }

  // center を中心に反時計回りに theta ラジアン回転
  Point rotate(Real theta, const Point &center) const {
    return center + (*this - center).rotate(theta);
  }

  // 単位法線ベクトル (反時計回りに90度回転した単位ベクトル)
  Point normal_unit() const;
};

inline Point operator*(Real k, const Point &p) { return p * k; }

istream &operator>>(istream &is, Point &p) { return is >> p.x >> p.y; }
ostream &operator<<(ostream &os, const Point &p) {
  return os << "(" << p.x << ", " << p.y << ")";
}

// --- 3. 基本演算関数 ---

// 内積 (dot product)
inline Real dot(const Point &a, const Point &b) {
  return a.x * b.x + a.y * b.y;
}

// 外積 (cross product / 2D外積)
inline Real cross(const Point &a, const Point &b) {
  return a.x * b.y - a.y * b.x;
}

// ベクトルの長さの2乗 (norm)
inline Real norm(const Point &p) { return p.x * p.x + p.y * p.y; }

// ベクトルの長さ (abs)
inline Real abs(const Point &p) { return sqrt(max((Real)0.0, norm(p))); }

// 2点間のユークリッド距離
inline Real distance(const Point &a, const Point &b) { return abs(a - b); }

inline Point Point::normal_unit() const {
  Real len = abs(*this);
  assert(sgn(len) != 0);
  return Point(-y / len, x / len);
}

// --- 4. 直線 (Line) と 線分 (Segment) ---
struct Line {
  Point a, b;
  Line() : a(Point()), b(Point()) {}
  Line(Point a, Point b) : a(a), b(b) {}
};

struct Segment : Line {
  Segment() : Line() {}
  Segment(Point a, Point b) : Line(a, b) {}
};

// --- 5. 進行方向判定 (CCW) ---
const int CCW_COUNTER_CLOCKWISE = 1; // 反時計回り (左側)
const int CCW_CLOCKWISE = -1;        // 時計回り (右側)
const int CCW_ONLINE_BACK = 2;       // C - A - B の順で一直線上
const int CCW_ONLINE_FRONT = -2;     // A - B - C の順で一直線上
const int CCW_ON_SEGMENT = 0;        // A - C - B の順で線分上

// 線分 a-b を基準とした点 c の位置関係
inline int ccw(const Point &a, const Point &b, const Point &c) {
  Point ab = b - a;
  Point ac = c - a;

  int cross_sgn = sgn(cross(ab, ac));
  if (cross_sgn > 0)
    return CCW_COUNTER_CLOCKWISE;
  if (cross_sgn < 0)
    return CCW_CLOCKWISE;

  if (sgn(dot(ab, ac)) < 0)
    return CCW_ONLINE_BACK;
  if (sgn(norm(ab) - norm(ac)) < 0)
    return CCW_ONLINE_FRONT;

  return CCW_ON_SEGMENT;
}

// --- 6. 射影・反射 ---

// 直線 a-b に対して点 c から下ろした垂線の足 (正射影)
inline Point project(const Point &a, const Point &b, const Point &c) {
  Point ab = b - a;
  Point ac = c - a;
  Real t = dot(ac, ab) / norm(ab);
  return a + ab * t;
}
inline Point project(const Line &l, const Point &c) {
  return project(l.a, l.b, c);
}

// 直線 a-b を対称軸として点 c と線対称な位置にある点 (反射)
inline Point reflect(const Point &a, const Point &b, const Point &c) {
  return c + (project(a, b, c) - c) * 2.0;
}
inline Point reflect(const Line &l, const Point &c) {
  return reflect(l.a, l.b, c);
}

// --- 7. 交差判定 ---

// 線分 a-b と線分 c-d が交差するか判定 (端点共有も真)
inline bool intersect(const Point &a, const Point &b, const Point &c,
                      const Point &d) {
  return ccw(a, b, c) * ccw(a, b, d) <= 0 &&
         ccw(c, d, a) * ccw(c, d, b) <= 0;
}
inline bool intersect(const Segment &s1, const Segment &s2) {
  return intersect(s1.a, s1.b, s2.a, s2.b);
}

// 直線 a-b と点 c が平行/直線上か判定
inline bool intersect_line_point(const Point &a, const Point &b,
                                 const Point &c) {
  return sgn(cross(b - a, c - a)) == 0;
}

// 直線 a-b と直線 c-d が交差するか (平行でないか)
inline bool intersect_line_line(const Point &a, const Point &b, const Point &c,
                                const Point &d) {
  return sgn(cross(b - a, d - c)) != 0;
}

// --- 8. 距離 ---

// 直線 a-b と点 c の最短距離
inline Real distance_line_point(const Point &a, const Point &b,
                                const Point &c) {
  return abs(cross(b - a, c - a)) / distance(a, b);
}

// 線分 a-b と点 c の最短距離
inline Real distance_segment_point(const Point &a, const Point &b,
                                   const Point &c) {
  if (a == b)
    return distance(a, c);
  if (sgn(dot(b - a, c - a)) < 0)
    return distance(a, c);
  if (sgn(dot(a - b, c - b)) < 0)
    return distance(b, c);
  return distance_line_point(a, b, c);
}
inline Real distance_segment_point(const Segment &s, const Point &c) {
  return distance_segment_point(s.a, s.b, c);
}

// 線分 a-b と線分 c-d の最短距離 (交差時は 0.0)
inline Real distance_segment_segment(const Point &a, const Point &b,
                                     const Point &c, const Point &d) {
  if (intersect(a, b, c, d))
    return 0.0;
  return min({distance_segment_point(a, b, c),
              distance_segment_point(a, b, d),
              distance_segment_point(c, d, a),
              distance_segment_point(c, d, b)});
}
inline Real distance_segment_segment(const Segment &s1, const Segment &s2) {
  return distance_segment_segment(s1.a, s1.b, s2.a, s2.b);
}

// --- 9. 交点座標 ---

// 直線 a-b と直線 c-d の交点 (平行でないことを前提とする)
inline Point cross_point_line(const Point &a, const Point &b, const Point &c,
                              const Point &d) {
  Point ab = b - a;
  Point cd = d - c;
  Real d1 = cross(ab, cd);
  assert(sgn(d1) != 0); // 平行なら交点なし
  Real d2 = cross(ab, b - c);
  return c + cd * (d2 / d1);
}
inline Point cross_point_line(const Line &l1, const Line &l2) {
  return cross_point_line(l1.a, l1.b, l2.a, l2.b);
}

// 線分 a-b と線分 c-d の交点座標 (交差していることを前提とする)
inline Point cross_point_segment(const Point &a, const Point &b, const Point &c,
                                 const Point &d) {
  assert(intersect(a, b, c, d));
  return cross_point_line(a, b, c, d);
}
inline Point cross_point_segment(const Segment &s1, const Segment &s2) {
  return cross_point_segment(s1.a, s1.b, s2.a, s2.b);
}

// --- 10. 円 (Circle) ---
struct Circle {
  Point c;
  Real r;
  Circle(Point c = Point(0.0, 0.0), Real r = 0.0) : c(c), r(r) {}
};

// 2つの円の交点座標 (交点なし: 0個, 接する: 1個, 交わる: 2個)
inline vector<Point> cross_point_circles(const Circle &c1, const Circle &c2) {
  Real d = distance(c1.c, c2.c);
  // 同心円の場合
  if (sgn(d) == 0) {
    return {};
  }
  // 離れすぎ、または一方が他方に完全内包されている
  if (sgn(d - (c1.r + c2.r)) > 0 || sgn(d - abs(c1.r - c2.r)) < 0) {
    return {};
  }

  // 余弦定理で角度 a を求める (浮動小数点誤差による NaN を clamp で防止)
  Real cos_a = (c1.r * c1.r + d * d - c2.r * c2.r) / (2.0 * c1.r * d);
  Real a = acos(clamp_val(cos_a, -1.0, 1.0));
  Real t = atan2(c2.c.y - c1.c.y, c2.c.x - c1.c.x);

  Point p1 = c1.c + Point(cos(t + a) * c1.r, sin(t + a) * c1.r);
  Point p2 = c1.c + Point(cos(t - a) * c1.r, sin(t - a) * c1.r);

  if (p1 == p2)
    return {p1};
  return {p1, p2};
}

// 円と直線 a-b の交点座標 (0, 1, または 2 個)
inline vector<Point> cross_point_circle_line(const Circle &c, const Point &a,
                                             const Point &b) {
  Real d = distance_line_point(a, b, c.c);
  if (sgn(d - c.r) > 0)
    return {};

  Point pr = project(a, b, c.c);
  if (sgn(d - c.r) == 0)
    return {pr};

  Point v = (b - a) / distance(a, b);
  Real len = sqrt(max((Real)0.0, c.r * c.r - d * d));
  return {pr - v * len, pr + v * len};
}

// --- 11. 多角形 (Polygon) ---
using Polygon = vector<Point>;

// 多角形の面積 (頂点の並び順によらず正の値を返す)
inline Real area(const Polygon &P) {
  int sz = P.size();
  if (sz < 3)
    return 0.0;
  Real res = 0.0;
  for (int i = 0; i < sz; i++) {
    res += cross(P[i], P[(i + 1) % sz]);
  }
  return abs(res) * 0.5;
}

// 点 p と多角形 P の包含判定 (0: 外部, 1: 辺上, 2: 内部)
inline int contains(const Polygon &P, const Point &p) {
  int sz = P.size();
  bool in = false;
  for (int i = 0; i < sz; i++) {
    Point a = P[i] - p;
    Point b = P[(i + 1) % sz] - p;
    if (a.y > b.y)
      swap(a, b);

    // 点pからX軸正方向への半直線と辺が交差するか
    if (sgn(a.y) <= 0 && sgn(b.y) > 0 && sgn(cross(a, b)) < 0)
      in = !in;
    // 点pが辺上にあるか
    if (sgn(cross(a, b)) == 0 && sgn(dot(a, b)) <= 0)
      return 1;
  }
  return in ? 2 : 0;
}

// 凸多角形判定 (反時計回りに与えられた多角形が凸かどうか)
inline bool is_convex(const Polygon &P) {
  int sz = P.size();
  if (sz < 3)
    return true;
  for (int i = 0; i < sz; i++) {
    if (ccw(P[i], P[(i + 1) % sz], P[(i + 2) % sz]) == CCW_CLOCKWISE) {
      return false;
    }
  }
  return true;
}

// 凸包 (Convex Hull: Andrew's Monotone Chain) O(N log N)
// strict = true: 一直線上の余分な点を除去 (厳密な頂点のみ)
// strict = false: 辺上の点も含める
inline Polygon convex_hull(vector<Point> P, bool strict = true) {
  int n = P.size();
  if (n <= 2)
    return P;
  sort(P.begin(), P.end());

  Polygon ch(2 * n);
  int k = 0;

  Real threshold = strict ? 0.0 : -EPS;

  // 下側凸包
  for (int i = 0; i < n; i++) {
    while (k >= 2 && sgn(cross(ch[k - 1] - ch[k - 2], P[i] - ch[k - 1])) <=
                         threshold) {
      k--;
    }
    ch[k++] = P[i];
  }
  // 上側凸包
  for (int i = n - 2, t = k + 1; i >= 0; i--) {
    while (k >= t && sgn(cross(ch[k - 1] - ch[k - 2], P[i] - ch[k - 1])) <=
                         threshold) {
      k--;
    }
    ch[k++] = P[i];
  }

  ch.resize(k - 1);
  return ch;
}

// 凸多角形の直径 (最遠点対間距離) - キャリパー法 O(N)
inline Real convex_diameter(const Polygon &P) {
  int n = P.size();
  if (n <= 1)
    return 0.0;
  if (n == 2)
    return distance(P[0], P[1]);

  int is = 0, js = 0;
  for (int i = 1; i < n; i++) {
    if (P[i].y > P[is].y)
      is = i;
    if (P[i].y < P[js].y)
      js = i;
  }

  Real max_d = distance(P[is], P[js]);
  int i = is, j = js;
  do {
    int ni = (i + 1) % n;
    int nj = (j + 1) % n;
    if (sgn(cross(P[ni] - P[i], P[nj] - P[j])) >= 0) {
      j = nj;
    } else {
      i = ni;
    }
    max_d = max(max_d, distance(P[i], P[j]));
  } while (i != is || j != js);

  return max_d;
}
