#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

// 2次元セグメント木 (可換モノイドに対する 2D Segment Tree)
// 注意: 演算 op は可換 (op(a, b) == op(b, a)) である必要があります (例: min, max, +, gcd, XORなど)
// S: 要素の型
// op: 2つの S を結合する可換演算
// e: 単位元を返す関数
template <class S, S (*op)(S, S), S (*e)()> struct SegTree2D {
  int H, W;
  vector<S> dat;

  SegTree2D() : H(0), W(0) {}
  SegTree2D(int h, int w) : H(h), W(w) {
    if (H > 0 && W > 0) {
      dat.assign(4 * H * W, e());
    }
  }

  // 2次元配列 v から O(HW) で構築
  SegTree2D(const vector<vector<S>> &v) {
    build(v);
  }

  // 2次元配列 v から O(HW) で初期化
  void build(const vector<vector<S>> &v) {
    H = v.size();
    W = (H == 0 ? 0 : v[0].size());
    if (H == 0 || W == 0)
      return;
    dat.assign(4 * H * W, e());

    for (int r = 0; r < H; r++) {
      for (int c = 0; c < W; c++) {
        dat[id(r + H, c + W)] = v[r][c];
      }
    }
    for (int r = 0; r < H; r++) {
      for (int c = W - 1; c > 0; c--) {
        dat[id(r + H, c)] =
            op(dat[id(r + H, 2 * c)], dat[id(r + H, 2 * c + 1)]);
      }
    }
    for (int r = H - 1; r > 0; r--) {
      for (int c = 1; c < 2 * W; c++) {
        dat[id(r, c)] = op(dat[id(2 * r, c)], dat[id(2 * r + 1, c)]);
      }
    }
  }

  // (r, c) の要素を val に更新する (O(log H * log W))
  void set(int r, int c, S val) {
    assert(0 <= r && r < H && 0 <= c && c < W);
    int cr = r + H;
    int cc = c + W;
    dat[id(cr, cc)] = val;
    for (int i = cc >> 1; i > 0; i >>= 1) {
      dat[id(cr, i)] = op(dat[id(cr, 2 * i)], dat[id(cr, 2 * i + 1)]);
    }
    for (cr >>= 1; cr > 0; cr >>= 1) {
      dat[id(cr, cc)] = op(dat[id(2 * cr, cc)], dat[id(2 * cr + 1, cc)]);
      for (int i = cc >> 1; i > 0; i >>= 1) {
        dat[id(cr, i)] = op(dat[id(cr, 2 * i)], dat[id(cr, 2 * i + 1)]);
      }
    }
  }

  // (r, c) の要素を取得する (O(1))
  S get(int r, int c) const {
    assert(0 <= r && r < H && 0 <= c && c < W);
    return dat[id(r + H, c + W)];
  }

  // 矩形区間 [r1, r2) × [c1, c2) の演算結果を取得する (O(log H * log W))
  S prod(int r1, int c1, int r2, int c2) const {
    assert(0 <= r1 && r1 <= r2 && r2 <= H);
    assert(0 <= c1 && c1 <= c2 && c2 <= W);
    if (r1 == r2 || c1 == c2)
      return e();
    S res = e();
    for (r1 += H, r2 += H; r1 < r2; r1 >>= 1, r2 >>= 1) {
      if (r1 & 1)
        res = op(res, prod_col(r1++, c1, c2));
      if (r2 & 1)
        res = op(res, prod_col(--r2, c1, c2));
    }
    return res;
  }

private:
  inline int id(int r, int c) const { return r * 2 * W + c; }

  S prod_col(int r, int c1, int c2) const {
    S res = e();
    for (c1 += W, c2 += W; c1 < c2; c1 >>= 1, c2 >>= 1) {
      if (c1 & 1)
        res = op(res, dat[id(r, c1++)]);
      if (c2 & 1)
        res = op(res, dat[id(r, --c2)]);
    }
    return res;
  }
};
