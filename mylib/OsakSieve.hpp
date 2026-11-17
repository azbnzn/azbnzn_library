#pragma once
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// osa_k法 (最小素因数 SPF による高速素因数分解・約数列挙)
// ============================================================================
// ・前計算 O(N) の線形篩で最小素因数 (SPF) テーブル min_factor[x] を構築し、
//   クエリごとの素因数分解を O(log X) で行う。
// ・x <= max_val の範囲で素数判定・素因数分解 {素数, 指数}・約数列挙・
//   約数個数/総和・オイラーの phi 関数・メビウス関数を提供。
//
// 使用例:
//   OsakSieve sieve(1000000);
//   bool p = sieve.is_prime(998244353 <= 1000000 ? 97 : 2);
//   auto fac = sieve.factorize(72);      // {{2, 3}, {3, 2}}
//   auto pf  = sieve.prime_factors(72);  // {2, 3}
//   auto div = sieve.divisors(72);       // {1, 2, 3, 4, 6, 8, 9, 12, 18, 24, 36, 72}
// ============================================================================

struct OsakSieve {
  int max_val;
  vector<int> min_factor;
  vector<int> primes;

  OsakSieve() : max_val(0), min_factor(1, 0) {}
  explicit OsakSieve(int n) : max_val(n), min_factor(n + 1, 0) {
    assert(n >= 0);
    if (n >= 1)
      min_factor[1] = 1;
    // 線形篩 O(N) で最小素因数を計算
    for (int i = 2; i <= n; ++i) {
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

  // 素数判定 (x <= max_val) O(1)
  bool is_prime(int x) const {
    if (x < 2 || x > max_val)
      return false;
    return min_factor[x] == x;
  }

  // 最小素因数 (SPF) を取得 O(1)
  int spf(int x) const {
    assert(1 <= x && x <= max_val);
    return min_factor[x];
  }

  // 素因数分解: {素因数, 指数} の昇順リスト O(log x)
  vector<pair<int, int>> factorize(int x) const {
    assert(1 <= x && x <= max_val);
    vector<pair<int, int>> res;
    while (x > 1) {
      int p = min_factor[x];
      int exp = 0;
      while (min_factor[x] == p) {
        x /= p;
        exp++;
      }
      res.push_back({p, exp});
    }
    return res;
  }

  // 素因数列 (重複あり昇順) O(log x)
  vector<int> factorize_list(int x) const {
    assert(1 <= x && x <= max_val);
    vector<int> res;
    while (x > 1) {
      res.push_back(min_factor[x]);
      x /= min_factor[x];
    }
    return res;
  }

  // 素因数のみのリスト (重複なし昇順) O(log x)
  vector<int> prime_factors(int x) const {
    assert(1 <= x && x <= max_val);
    vector<int> res;
    while (x > 1) {
      int p = min_factor[x];
      res.push_back(p);
      while (x % p == 0)
        x /= p;
    }
    return res;
  }

  // 約数列挙 (昇順ソート済み)
  vector<int> divisors(int x) const {
    assert(1 <= x && x <= max_val);
    auto factors = factorize(x);
    vector<int> divs = {1};
    for (auto [p, exp] : factors) {
      int sz = (int)divs.size();
      int cur = 1;
      for (int i = 0; i < exp; ++i) {
        cur *= p;
        for (int j = 0; j < sz; ++j) {
          divs.push_back(divs[j] * cur);
        }
      }
    }
    sort(divs.begin(), divs.end());
    return divs;
  }

  // 約数の個数 d(x) O(log x)
  int count_divisors(int x) const {
    auto factors = factorize(x);
    int ans = 1;
    for (auto [p, exp] : factors) {
      (void)p;
      ans *= (exp + 1);
    }
    return ans;
  }

  // 約数の総和 sigma(x) O(log x)
  long long sum_divisors(int x) const {
    auto factors = factorize(x);
    long long ans = 1;
    for (auto [p, exp] : factors) {
      long long cur = 1, mult = 1;
      for (int i = 0; i < exp; ++i) {
        mult *= p;
        cur += mult;
      }
      ans *= cur;
    }
    return ans;
  }

  // オイラーのトーシェント関数 phi(x) O(log x)
  int euler_phi(int x) const {
    assert(1 <= x && x <= max_val);
    int ans = x;
    while (x > 1) {
      int p = min_factor[x];
      ans -= ans / p;
      while (x % p == 0)
        x /= p;
    }
    return ans;
  }

  // メビウス関数 mu(x) O(log x)
  int moebius(int x) const {
    assert(1 <= x && x <= max_val);
    int mu = 1;
    while (x > 1) {
      int p = min_factor[x];
      x /= p;
      if (x % p == 0)
        return 0;
      mu = -mu;
    }
    return mu;
  }
};
