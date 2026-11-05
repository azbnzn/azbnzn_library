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

// ============================================================================
// 素数・素因数分解・約数・エラトステネス線形篩ライブラリ (Prime Library)
// ============================================================================
// 1. 単体操作 (巨大数 N <= 10^18 に対応):
//    - is_prime(n)          : Miller-Rabin 素数判定 (2^64 未満で決定論的に O(log N))
//    - prime_factorize(n)   : Pollard's rho 法による高速素因数分解 O(N^{1/4}) -> {素数, 指数}
//    - factorize(n)         : 素因数列を昇順で返す (重複あり)
//    - divisors(n)          : 全約数を昇順ソートして返す
//    - count_divisors(n)    : 約数の個数 d(n)
//    - sum_divisors(n)      : 約数の総和 sigma(n)
//    - euler_phi(n)         : オイラーのトーシェント関数 phi(n)
//
// 2. 線形篩・osa_k法構造体 (PrimeSieve):
//    - 前計算 O(MAX_N) でクエリ O(log X) の高速素因数分解・約数列挙
//    - sieve.is_prime(x)       : O(1) 素数判定
//    - sieve.factorize(x)      : O(log x) 素因数分解
//    - sieve.prime_factorize(x): O(log x) {素数, 指数}
//    - sieve.divisors(x)       : O(約数の個数) 約数列挙
//    - sieve.count_divisors(x) : O(log x) 約数の個数
//    - sieve.sum_divisors(x)   : O(log x) 約数の総和
//    - sieve.euler_phi(x)      : O(log x) phi(x)
//    - sieve.moebius(x)        : O(log x) メビウス関数 mu(x) (-1, 0, 1)
//
// 3. 区間篩 (segmented_sieve):
//    - segmented_sieve(L, R) : [L, R) の素数列挙 (R <= 10^12, R - L <= 10^6)
// ============================================================================

namespace prime_internal {

inline uint64_t mul_mod(uint64_t a, uint64_t b, uint64_t mod) {
  return (uint64_t)((__uint128_t)a * b % mod);
}

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

inline uint64_t gcd_u64(uint64_t a, uint64_t b) {
  while (b) {
    a %= b;
    swap(a, b);
  }
  return a;
}

} // namespace prime_internal

// --- 1. Miller-Rabin 素数判定法 (2^64 未満で誤判定なし) ---
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

  // 2^64 未満で誤判定のない決定論的基数集合
  static const uint64_t bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
  for (uint64_t a : bases) {
    if (n <= a)
      break;
    uint64_t x = prime_internal::pow_mod(a, d, n);
    if (x == 1 || x == n - 1)
      continue;
    bool composite = true;
    for (int r = 1; r < s; r++) {
      x = prime_internal::mul_mod(x, x, n);
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

// 符号付き long long 版
inline bool is_prime(long long n) {
  if (n <= 1)
    return false;
  return is_prime((uint64_t)n);
}

// --- 2. Pollard's rho 法による高速素因数分解 (O(N^{1/4})) ---
namespace prime_internal {

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

} // namespace prime_internal

// 巨大整数 n (n <= 10^18) の素因数を昇順ソートして返す (重複あり)
inline vector<long long> factorize(long long n) {
  if (n <= 1)
    return {};
  vector<uint64_t> res_u64;
  prime_internal::factorize_rec((uint64_t)n, res_u64);
  sort(res_u64.begin(), res_u64.end());
  vector<long long> res(res_u64.begin(), res_u64.end());
  return res;
}

// 素因数分解を {素数, 指数} のペアで返す (昇順)
inline vector<pair<long long, int>> prime_factorize(long long n) {
  auto factors = factorize(n);
  vector<pair<long long, int>> res;
  for (long long p : factors) {
    if (res.empty() || res.back().first != p) {
      res.emplace_back(p, 1);
    } else {
      res.back().second++;
    }
  }
  return res;
}

// 整数 n (n <= 10^18) の全約数を昇順ソートして返す
inline vector<long long> divisors(long long n) {
  if (n <= 0)
    return {};
  auto pairs = prime_factorize(n);
  vector<long long> res = {1};
  for (const auto &[p, exp] : pairs) {
    int sz = res.size();
    long long mult = 1;
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

// 約数の個数 d(n)
inline long long count_divisors(long long n) {
  if (n <= 0)
    return 0;
  auto pairs = prime_factorize(n);
  long long ans = 1;
  for (const auto &[p, exp] : pairs) {
    ans *= (exp + 1);
  }
  return ans;
}

// 約数の総和 sigma(n)
inline long long sum_divisors(long long n) {
  if (n <= 0)
    return 0;
  auto pairs = prime_factorize(n);
  long long ans = 1;
  for (const auto &[p, exp] : pairs) {
    long long cur = 1, mult = 1;
    for (int i = 0; i < exp; i++) {
      mult *= p;
      cur += mult;
    }
    ans *= cur;
  }
  return ans;
}

// オイラーのトーシェント関数 phi(n) (n と互いに素な 1 以上 n 以下の整数の個数)
inline long long euler_phi(long long n) {
  if (n <= 0)
    return 0;
  auto pairs = prime_factorize(n);
  long long ans = n;
  for (const auto &[p, exp] : pairs) {
    ans -= ans / p;
  }
  return ans;
}

// --- 3. 線形篩・osa_k法構造体 (PrimeSieve) ---
// N <= 10^7 程度の前計算を行い、クエリ O(log x) で素因数分解するクラス
struct PrimeSieve {
  int n;
  vector<int> min_factor_table;
  vector<int> primes;

  PrimeSieve() : n(0) {}
  explicit PrimeSieve(int n) : n(n), min_factor_table(n + 1, 0) {
    for (int i = 2; i <= n; i++) {
      if (min_factor_table[i] == 0) {
        min_factor_table[i] = i;
        primes.push_back(i);
      }
      for (int p : primes) {
        if (p > min_factor_table[i] || (long long)i * p > n)
          break;
        min_factor_table[i * p] = p;
      }
    }
  }

  // O(1) 素数判定
  bool is_prime(int x) const {
    assert(1 <= x && x <= n);
    return min_factor_table[x] == x;
  }

  // 最小素因数 (SPF: Smallest Prime Factor) を返す (1 ならば 1)
  int min_factor(int x) const {
    assert(1 <= x && x <= n);
    return (x == 1 ? 1 : min_factor_table[x]);
  }

  // x の素因数列を昇順で返す (O(log x))
  vector<int> factorize(int x) const {
    assert(1 <= x && x <= n);
    vector<int> res;
    while (x > 1) {
      res.push_back(min_factor_table[x]);
      x /= min_factor_table[x];
    }
    return res;
  }

  // {素数, 指数} のペアで返す (O(log x))
  vector<pair<int, int>> prime_factorize(int x) const {
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
    auto pairs = prime_factorize(x);
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

  // 約数の個数 d(x) (O(log x))
  int count_divisors(int x) const {
    auto pairs = prime_factorize(x);
    int ans = 1;
    for (const auto &[p, exp] : pairs) {
      ans *= (exp + 1);
    }
    return ans;
  }

  // 約数の総和 sigma(x) (O(log x))
  long long sum_divisors(int x) const {
    auto pairs = prime_factorize(x);
    long long ans = 1;
    for (const auto &[p, exp] : pairs) {
      long long cur = 1, mult = 1;
      for (int i = 0; i < exp; i++) {
        mult *= p;
        cur += mult;
      }
      ans *= cur;
    }
    return ans;
  }

  // オイラーのトーシェント関数 phi(x) (O(log x))
  int euler_phi(int x) const {
    assert(1 <= x && x <= n);
    auto pairs = prime_factorize(x);
    int ans = x;
    for (const auto &[p, exp] : pairs) {
      ans -= ans / p;
    }
    return ans;
  }

  // メビウス関数 mu(x) (平方因子を持てば 0, 相異なる k 個の素因数の積なら (-1)^k) (O(log x))
  int moebius(int x) const {
    assert(1 <= x && x <= n);
    int mu = 1;
    while (x > 1) {
      int p = min_factor_table[x];
      x /= p;
      if (x % p == 0)
        return 0;
      mu = -mu;
    }
    return mu;
  }
};

// --- 4. 区間篩 (Segmented Sieve) ---
// 区間 [L, R) に含まれる素数を昇順で列挙する
// 制約: R <= 10^12, R - L <= 10^6 程度
inline vector<long long> segmented_sieve(long long L, long long R) {
  if (L < 2)
    L = 2;
  if (L >= R)
    return {};

  long long sqrt_R = 1;
  while ((sqrt_R + 1) * (sqrt_R + 1) < R)
    sqrt_R++;

  vector<bool> is_prime_small(sqrt_R + 1, true);
  vector<long long> small_primes;
  for (long long i = 2; i <= sqrt_R; i++) {
    if (is_prime_small[i]) {
      small_primes.push_back(i);
      for (long long j = i * i; j <= sqrt_R; j += i) {
        is_prime_small[j] = false;
      }
    }
  }

  vector<bool> is_prime_range(R - L, true);
  for (long long p : small_primes) {
    long long start = ((L + p - 1) / p) * p;
    if (start < p * p)
      start = p * p;
    for (long long j = start; j < R; j += p) {
      is_prime_range[j - L] = false;
    }
  }

  vector<long long> primes;
  for (long long i = 0; i < R - L; i++) {
    if (is_prime_range[i]) {
      primes.push_back(L + i);
    }
  }
  return primes;
}
