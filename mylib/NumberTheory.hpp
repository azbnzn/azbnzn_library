#pragma once
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

using namespace std;

// 高速数論ライブラリ (Number Theory Library)
// - Miller-Rabin 素数判定法 (2^64 未満で決定論的に O(log N))
// - Pollard's rho 素因数分解法 (N <= 10^18 を O(N^{1/4}))
// - LinearSieve (osa_k法 / 線形篩による O(log x) 素因数分解テーブル)
// - 中国剰余定理 (Extended CRT)
// - 拡張ユークリッド互除法 (extgcd) / mod逆元 (mod_inv)
namespace NumberTheory {

// 2^64 未満の安全な剰余乗算 (a * b) % mod
inline uint64_t mul_mod(uint64_t a, uint64_t b, uint64_t mod) {
  return (uint64_t)((__uint128_t)a * b % mod);
}

// べき乗剰余 (a^b) % mod
inline uint64_t pow_mod(uint64_t a, uint64_t b, uint64_t mod) {
  uint64_t res = 1 % mod;
  a %= mod;
  while (b > 0) {
    if (b & 1)
      res = mul_mod(res, a, mod);
    a = mul_mod(a, a, mod);
    b >>= 1;
  }
  return res;
}

// 拡張ユークリッドの互除法: a*x + b*y = gcd(a, b) を満たす (x, y) を求め gcd(a, b) を返す
inline long long extgcd(long long a, long long b, long long &x, long long &y) {
  if (b == 0) {
    x = 1;
    y = 0;
    return a;
  }
  long long d = extgcd(b, a % b, y, x);
  y -= a / b * x;
  return d;
}

// モジュラ逆元 (gcd(a, mod) == 1 を前提とする)
inline long long mod_inv(long long a, long long mod) {
  long long x, y;
  long long g = extgcd(a, mod, x, y);
  assert(g == 1);
  return (x % mod + mod) % mod;
}

// --- Miller-Rabin 素数判定法 (2^64 未満で決定論的に動作) ---
inline bool is_prime(uint64_t n) {
  if (n <= 1)
    return false;
  if (n == 2 || n == 3 || n == 5 || n == 7)
    return true;
  if (n % 2 == 0 || n % 3 == 0 || n % 5 == 0 || n % 7 == 0)
    return false;
  if (n < 121)
    return true; // 11^2

  uint64_t d = n - 1;
  int s = 0;
  while ((d & 1) == 0) {
    d >>= 1;
    s++;
  }

  // 2^64 未満で誤判定のない基数集合
  static const uint64_t bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
  for (uint64_t a : bases) {
    if (n <= a)
      break;
    uint64_t x = pow_mod(a, d, n);
    if (x == 1 || x == n - 1)
      continue;
    bool composite = true;
    for (int r = 1; r < s; r++) {
      x = mul_mod(x, x, n);
      if (x == n - 1) {
        composite = false;
        break;
      }
    }
    if (composite)
      return false;
  }
  return true;
}

// --- Pollard's rho 法による高速素因数分解 (O(N^{1/4})) ---
namespace internal_rho {
inline uint64_t gcd_u64(uint64_t a, uint64_t b) {
  while (b) {
    a %= b;
    swap(a, b);
  }
  return a;
}

inline uint64_t find_factor(uint64_t n) {
  if (n % 2 == 0)
    return 2;
  if (is_prime(n))
    return n;

  static mt19937_64 rng(
      chrono::steady_clock::now().time_since_epoch().count());
  uniform_int_distribution<uint64_t> dist_c(1, n - 1);
  uniform_int_distribution<uint64_t> dist_x(2, n - 1);

  while (true) {
    uint64_t c = dist_c(rng);
    uint64_t x = dist_x(rng);
    uint64_t y = x;
    uint64_t d = 1;

    auto f = [&](uint64_t v) { return (mul_mod(v, v, n) + c) % n; };

    while (d == 1) {
      x = f(x);
      y = f(f(y));
      d = gcd_u64((x > y ? x - y : y - x), n);
    }
    if (d != n)
      return d;
  }
}

inline void factorize_rec(uint64_t n, vector<uint64_t> &res) {
  if (n <= 1)
    return;
  if (is_prime(n)) {
    res.push_back(n);
    return;
  }
  uint64_t factor = find_factor(n);
  factorize_rec(factor, res);
  factorize_rec(n / factor, res);
}
} // namespace internal_rho

// 巨大整数 n (n <= 10^18) の素因数を昇順ソートして返す (O(n^{1/4}))
inline vector<uint64_t> factorize(uint64_t n) {
  vector<uint64_t> res;
  internal_rho::factorize_rec(n, res);
  sort(res.begin(), res.end());
  return res;
}

// 素因数分解を {素数, 指数} のペアで返す
inline vector<pair<uint64_t, int>> factorize_pairs(uint64_t n) {
  auto factors = factorize(n);
  vector<pair<uint64_t, int>> res;
  for (uint64_t p : factors) {
    if (res.empty() || res.back().first != p) {
      res.emplace_back(p, 1);
    } else {
      res.back().second++;
    }
  }
  return res;
}

// 整数 n (n <= 10^18) の全約数を昇順ソートして返す
inline vector<uint64_t> divisors(uint64_t n) {
  if (n == 0)
    return {};
  auto pairs = factorize_pairs(n);
  vector<uint64_t> res = {1};
  for (const auto &[p, exp] : pairs) {
    int sz = res.size();
    uint64_t mult = 1;
    for (int i = 0; i < exp; i++) {
      mult *= p;
      for (int j = 0; j < sz; j++) {
        res.push_back(res[j] * mult);
      }
    }
  }
  sort(res.begin(), res.end());
  return res;
}

// --- LinearSieve (osa_k法 / エラトステネスの線形篩) ---
// N <= 10^7 程度の前計算を行い、クエリ O(log x) で素因数分解するクラス
struct LinearSieve {
  int n;
  vector<int> min_factor;
  vector<int> primes;

  LinearSieve() : n(0) {}
  explicit LinearSieve(int n) : n(n), min_factor(n + 1, 0) {
    for (int i = 2; i <= n; i++) {
      if (min_factor[i] == 0) {
        min_factor[i] = i;
        primes.push_back(i);
      }
      for (int p : primes) {
        if (p > min_factor[i] || (long long)i * p > n)
          break;
        min_factor[i * p] = p;
      }
    }
  }

  bool is_prime(int x) const {
    assert(1 <= x && x <= n);
    return min_factor[x] == x;
  }

  // x の素因数列を昇順で返す (O(log x))
  vector<int> factorize(int x) const {
    assert(1 <= x && x <= n);
    vector<int> res;
    while (x > 1) {
      res.push_back(min_factor[x]);
      x /= min_factor[x];
    }
    return res;
  }

  // {素数, 指数} のペアで返す (O(log x))
  vector<pair<int, int>> factorize_pairs(int x) const {
    auto f = factorize(x);
    vector<pair<int, int>> res;
    for (int p : f) {
      if (res.empty() || res.back().first != p) {
        res.emplace_back(p, 1);
      } else {
        res.back().second++;
      }
    }
    return res;
  }

  // 約数列挙 (昇順ソート済み)
  vector<int> divisors(int x) const {
    assert(1 <= x && x <= n);
    auto pairs = factorize_pairs(x);
    vector<int> res = {1};
    for (const auto &[p, exp] : pairs) {
      int sz = res.size();
      int mult = 1;
      for (int i = 0; i < exp; i++) {
        mult *= p;
        for (int j = 0; j < sz; j++) {
          res.push_back(res[j] * mult);
        }
      }
    }
    sort(res.begin(), res.end());
    return res;
  }
};

// --- 中国剰余定理 (Extended Chinese Remainder Theorem) ---
// 連立合同式 x ≡ r_i (mod m_i) を解く
// 戻り値: {x, lcm(m_i)} (0 <= x < lcm, 解なしの場合は {0, 0})
inline pair<long long, long long> crt(const vector<long long> &r,
                                     const vector<long long> &m) {
  assert(r.size() == m.size());
  int n = r.size();
  long long r0 = 0, m0 = 1;
  for (int i = 0; i < n; i++) {
    assert(m[i] >= 1);
    long long r1 = (r[i] % m[i] + m[i]) % m[i];
    long long m1 = m[i];
    if (m0 < m1) {
      swap(r0, r1);
      swap(m0, m1);
    }
    if (m0 % m1 == 0) {
      if (r0 % m1 != r1)
        return {0, 0};
      continue;
    }
    long long im, ym;
    long long g = extgcd(m0, m1, im, ym);
    long long u1 = m1 / g;
    if ((r1 - r0) % g != 0)
      return {0, 0};
    long long x = (r1 - r0) / g % u1 * im % u1;
    r0 += x * m0;
    m0 *= u1;
    if (r0 < 0)
      r0 += m0;
  }
  return {r0, m0};
}

} // namespace NumberTheory
