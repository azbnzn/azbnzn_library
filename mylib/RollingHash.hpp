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

// 2^61 - 1 を法とする安全なダブルローリングハッシュ
struct DoubleRollingHash {
  static const uint64_t mod = (1ULL << 61) - 1;
  uint64_t base1, base2;
  vector<uint64_t> hash1, power1;
  vector<uint64_t> hash2, power2;

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

  DoubleRollingHash() : base1(0), base2(0) {}

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
    int n = v.size();
    hash1.assign(n + 1, 0);
    power1.assign(n + 1, 1);
    hash2.assign(n + 1, 0);
    power2.assign(n + 1, 1);

    for (int i = 0; i < n; i++) {
      uint64_t val = (uint64_t)v[i] % mod;
      hash1[i + 1] = mul(hash1[i], base1) + val;
      if (hash1[i + 1] >= mod)
        hash1[i + 1] -= mod;
      power1[i + 1] = mul(power1[i], base1);

      hash2[i + 1] = mul(hash2[i], base2) + val;
      if (hash2[i + 1] >= mod)
        hash2[i + 1] -= mod;
      power2[i + 1] = mul(power2[i], base2);
    }
  }

  // [l, r) の部分列のハッシュ値をペアで取得 (0-indexed)
  pair<uint64_t, uint64_t> get(int l, int r) const {
    assert(0 <= l && l <= r && r < (int)hash1.size());
    uint64_t res1 = hash1[r] + mod - mul(hash1[l], power1[r - l]);
    if (res1 >= mod)
      res1 -= mod;

    uint64_t res2 = hash2[r] + mod - mul(hash2[l], power2[r - l]);
    if (res2 >= mod)
      res2 -= mod;

    return {res1, res2};
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

  // 2つの文字列 (または同一文字列の異なる区間) の最長共通接頭辞 (LCP) の長さを O(log N) で取得
  int lcp(const DoubleRollingHash &other, int l1 = 0, int l2 = 0) const {
    int max_len = min((int)hash1.size() - 1 - l1, (int)other.hash1.size() - 1 - l2);
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
};

// エイリアス定義
using RollingHash = DoubleRollingHash;
