#pragma once
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <random>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// 2^61 - 1 を法とする安全なダブルローリングハッシュ (DoubleRollingHash)
// ============================================================================
// ・静的文字列に対しては O(1) で区間ハッシュ・LCP を取得可能
// ・set(idx, c) / update(idx, c) により、文字列の一部を変更するクエリに O(log N) で対応
// ・get_with_change(l, r, idx, c) により、1文字変更を仮定したハッシュ値を O(1) で取得可能
// ・get_reversed(l, r) / is_palindrome(l, r) により、点更新付き回文判定にも対応
// ============================================================================

struct DoubleRollingHash {
  static const uint64_t mod = (1ULL << 61) - 1;
  uint64_t base1, base2;
  vector<uint64_t> hash1, power1;
  vector<uint64_t> hash2, power2;
  vector<uint64_t> origin;
  int n;

  // セグメント木用ノード (点更新クエリ対応用)
  struct SegNode {
    uint64_t h1, h2;   // 正順ハッシュ
    uint64_t rh1, rh2; // 逆順ハッシュ (回文判定用)
    int len;           // 区間長
  };
  vector<SegNode> seg;
  int seg_sz;
  bool seg_built;

  // 2^61 - 1 を法とする安全かつ高速な乗算
  static inline uint64_t mul(uint64_t a, uint64_t b) {
    __uint128_t t = (__uint128_t)a * b;
    t = (t >> 61) + (t & mod);
    if (t >= mod)
      t -= mod;
    return (uint64_t)t;
  }

  // 乱数による基数生成
  static inline uint64_t generate_base() {
    static mt19937_64 mt(
        chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<uint64_t> rand(2, mod - 2);
    return rand(mt);
  }

  // クラス全体で共有するデフォルト基数 (複数インスタンス間でハッシュを比較可能にする)
  static inline pair<uint64_t, uint64_t> get_default_bases() {
    static uint64_t b1 = 0, b2 = 0;
    if (b1 == 0) {
      b1 = generate_base();
      b2 = generate_base();
      while (b1 == b2) {
        b2 = generate_base();
      }
    }
    return {b1, b2};
  }

  DoubleRollingHash()
      : base1(0), base2(0), n(0), seg_sz(0), seg_built(false) {}

  // 文字列から構築 (共有基数を使用)
  DoubleRollingHash(const string &s) {
    auto [b1, b2] = get_default_bases();
    init(s, b1, b2);
  }

  // 基数を明示的に指定して構築
  DoubleRollingHash(const string &s, uint64_t b1, uint64_t b2) {
    init(s, b1, b2);
  }

  // 数列 vector<T> から構築 (共有基数を使用)
  template <typename T> DoubleRollingHash(const vector<T> &v) {
    auto [b1, b2] = get_default_bases();
    init(v, b1, b2);
  }

  template <typename T>
  DoubleRollingHash(const vector<T> &v, uint64_t b1, uint64_t b2) {
    init(v, b1, b2);
  }

  void init(const string &s, uint64_t b1, uint64_t b2) {
    vector<uint64_t> v(s.begin(), s.end());
    init(v, b1, b2);
  }

  template <typename T>
  void init(const vector<T> &v, uint64_t b1, uint64_t b2) {
    base1 = b1;
    base2 = b2;
    n = v.size();
    origin.resize(n);
    for (int i = 0; i < n; i++) {
      origin[i] = (uint64_t)v[i] % mod;
    }

    hash1.assign(n + 1, 0);
    power1.assign(n + 1, 1);
    hash2.assign(n + 1, 0);
    power2.assign(n + 1, 1);

    for (int i = 0; i < n; i++) {
      uint64_t val = origin[i];
      hash1[i + 1] = mul(hash1[i], base1) + val;
      if (hash1[i + 1] >= mod)
        hash1[i + 1] -= mod;
      power1[i + 1] = mul(power1[i], base1);

      hash2[i + 1] = mul(hash2[i], base2) + val;
      if (hash2[i + 1] >= mod)
        hash2[i + 1] -= mod;
      power2[i + 1] = mul(power2[i], base2);
    }
    seg_built = false;
  }

  // [l, r) の部分列のハッシュ値をペアで取得 (0-indexed 半開区間)
  // ※ set による更新前は O(1)、更新後は O(log N)
  pair<uint64_t, uint64_t> get(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    if (l == r)
      return {0, 0};
    if (!seg_built) {
      uint64_t res1 = hash1[r] + mod - mul(hash1[l], power1[r - l]);
      if (res1 >= mod)
        res1 -= mod;

      uint64_t res2 = hash2[r] + mod - mul(hash2[l], power2[r - l]);
      if (res2 >= mod)
        res2 -= mod;

      return {res1, res2};
    }

    SegNode L_res{0, 0, 0, 0, 0}, R_res{0, 0, 0, 0, 0};
    int L = l + seg_sz, R = r + seg_sz;
    while (L < R) {
      if (L & 1)
        L_res = merge_node(L_res, seg[L++]);
      if (R & 1)
        R_res = merge_node(seg[--R], R_res);
      L >>= 1;
      R >>= 1;
    }
    SegNode total = merge_node(L_res, R_res);
    return {total.h1, total.h2};
  }

  // 位置 idx の文字/要素を val に変更する (O(log N))
  template <typename T> void set(int idx, T val) {
    assert(0 <= idx && idx < n);
    ensure_seg_tree();
    uint64_t v = (uint64_t)val % mod;
    origin[idx] = v;
    int k = seg_sz + idx;
    seg[k] = make_leaf(v);
    k >>= 1;
    while (k > 0) {
      seg[k] = merge_node(seg[2 * k], seg[2 * k + 1]);
      k >>= 1;
    }
  }

  // set の別名
  template <typename T> void update(int idx, T val) { set(idx, val); }

  // [l, r) において、位置 idx の文字を new_val に変更した場合のハッシュ値を O(1) で取得
  // (idx が [l, r) の範囲外の場合は通常の get(l, r) と同じ結果を返す)
  template <typename T>
  pair<uint64_t, uint64_t> get_with_change(int l, int r, int idx,
                                          T new_val) const {
    assert(0 <= l && l <= r && r <= n);
    auto [h1, h2] = get(l, r);
    if (idx < l || idx >= r)
      return {h1, h2};

    uint64_t old_v = origin[idx];
    uint64_t new_v = (uint64_t)new_val % mod;
    int shift = r - 1 - idx;

    uint64_t diff1 = mul((new_v + mod - old_v) % mod, power1[shift]);
    h1 += diff1;
    if (h1 >= mod)
      h1 -= mod;

    uint64_t diff2 = mul((new_v + mod - old_v) % mod, power2[shift]);
    h2 += diff2;
    if (h2 >= mod)
      h2 -= mod;

    return {h1, h2};
  }

  // [l, r) の反転部分列のハッシュ値を取得 (O(log N))
  pair<uint64_t, uint64_t> get_reversed(int l, int r) {
    assert(0 <= l && l <= r && r <= n);
    if (l == r)
      return {0, 0};
    ensure_seg_tree();
    SegNode L_res{0, 0, 0, 0, 0}, R_res{0, 0, 0, 0, 0};
    int L = l + seg_sz, R = r + seg_sz;
    while (L < R) {
      if (L & 1)
        L_res = merge_node(L_res, seg[L++]);
      if (R & 1)
        R_res = merge_node(seg[--R], R_res);
      L >>= 1;
      R >>= 1;
    }
    SegNode total = merge_node(L_res, R_res);
    return {total.rh1, total.rh2};
  }

  // [l, r) が回文かどうか判定 (O(log N))
  bool is_palindrome(int l, int r) {
    if (l >= r)
      return true;
    return get(l, r) == get_reversed(l, r);
  }

  // ハッシュ同士の結合 (h1 の後ろに長さ len2 の h2 を連結)
  pair<uint64_t, uint64_t> concat(pair<uint64_t, uint64_t> h1,
                                 pair<uint64_t, uint64_t> h2, int len2) const {
    uint64_t res1 = mul(h1.first, power1[len2]) + h2.first;
    if (res1 >= mod)
      res1 -= mod;

    uint64_t res2 = mul(h1.second, power2[len2]) + h2.second;
    if (res2 >= mod)
      res2 -= mod;

    return {res1, res2};
  }

  // 2つの文字列 (または同一文字列の異なる区間) の最長共通接頭辞 (LCP) の長さを二分探索で取得
  int lcp(const DoubleRollingHash &other, int l1 = 0, int l2 = 0) const {
    int max_len = min(n - l1, other.n - l2);
    int low = 0, high = max_len + 1;
    while (high - low > 1) {
      int mid = (low + high) / 2;
      if (get(l1, l1 + mid) == other.get(l2, l2 + mid)) {
        low = mid;
      } else {
        high = mid;
      }
    }
    return low;
  }

private:
  SegNode make_leaf(uint64_t val) const {
    val %= mod;
    return {val, val, val, val, 1};
  }

  SegNode merge_node(const SegNode &L, const SegNode &R) const {
    if (L.len == 0)
      return R;
    if (R.len == 0)
      return L;

    uint64_t h1 = mul(L.h1, power1[R.len]) + R.h1;
    if (h1 >= mod)
      h1 -= mod;
    uint64_t h2 = mul(L.h2, power2[R.len]) + R.h2;
    if (h2 >= mod)
      h2 -= mod;

    uint64_t rh1 = mul(R.rh1, power1[L.len]) + L.rh1;
    if (rh1 >= mod)
      rh1 -= mod;
    uint64_t rh2 = mul(R.rh2, power2[L.len]) + L.rh2;
    if (rh2 >= mod)
      rh2 -= mod;

    return {h1, h2, rh1, rh2, L.len + R.len};
  }

  void ensure_seg_tree() {
    if (seg_built)
      return;
    seg_sz = 1;
    while (seg_sz < n)
      seg_sz <<= 1;
    seg.assign(2 * seg_sz, {0, 0, 0, 0, 0});
    for (int i = 0; i < n; i++) {
      seg[seg_sz + i] = make_leaf(origin[i]);
    }
    for (int i = seg_sz - 1; i >= 1; i--) {
      seg[i] = merge_node(seg[2 * i], seg[2 * i + 1]);
    }
    seg_built = true;
  }
};

// 互換用エイリアス
using RollingHash = DoubleRollingHash;
