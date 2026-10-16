# エラトステネスの篩 ＋ 高速素因数分解 (osa_k法)

エラトステネスの篩を応用し、各整数の「最小の素因数（Smallest Prime Factor; SPF）」を前計算しておくことで、任意の数を $O(\log X)$ で高速に素因数分解するテクニックです。

---

## 1. 概要・いつ使うか
- **計算量**:
  - 前計算: $O(MAX \log \log MAX)$ （$MAX = 10^7$ でも 0.1秒未満）
  - 素因数分解クエリ: 1回あたり **$O(\log X)$**
  - 約数列挙クエリ: $O(\text{約数の個数})$
- **使える条件**: 素因数分解したい数 $X$ が前計算サイズ $MAX$ 以下であること（通常 $MAX \le 10^7$ 程度まで適用可能）。
- **いつ使うか**:
  - クエリごとに何度も素因数分解を要求される問題（$Q \le 10^5$ など、通常の方法 $O(\sqrt{X})$ だと TLE になるケース）。
  - 区間内の各値の素因数ごとの重複度や、GCD / LCM を扱う問題。
  - 約数の個数や約数の総和を大量に計算したいとき。

---

## 2. コピペ用コード (C++)

```cpp
struct OsakSieve {
    int max_val;
    vector<int> min_factor;
    vector<int> primes;

    OsakSieve(int n) : max_val(n), min_factor(n + 1, 0) {
        for (int i = 0; i <= n; i++) min_factor[i] = i;
        for (int i = 2; i * i <= n; i++) {
            if (min_factor[i] == i) {
                for (int j = i * i; j <= n; j += i) {
                    if (min_factor[j] == j) {
                        min_factor[j] = i;
                    }
                }
            }
        }
        for (int i = 2; i <= n; i++) {
            if (min_factor[i] == i) primes.push_back(i);
        }
    }

    // 素数判定
    bool is_prime(int x) const {
        if (x < 2) return false;
        return min_factor[x] == x;
    }

    // 素因数分解: {素因数, 指数} の昇順リスト O(log x)
    vector<pair<int, int>> factorize(int x) const {
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

    // 重複なし素因数リスト O(log x)
    vector<int> prime_factors(int x) const {
        vector<int> res;
        while (x > 1) {
            int p = min_factor[x];
            res.push_back(p);
            while (x % p == 0) x /= p;
        }
        return res;
    }

    // 約数列挙 (昇順ソート済み)
    vector<int> divisors(int x) const {
        auto factors = factorize(x);
        vector<int> divs = {1};
        for (auto [p, exp] : factors) {
            int sz = divs.size();
            int cur = 1;
            for (int i = 0; i < exp; i++) {
                cur *= p;
                for (int j = 0; j < sz; j++) {
                    divs.push_back(divs[j] * cur);
                }
            }
        }
        sort(divs.begin(), divs.end());
        return divs;
    }
};
```

---

## 3. 使い方例

```cpp
// 10^6 までの前計算
OsakSieve sieve(1000000);

// 素数判定
if (sieve.is_prime(998244353)) { ... }

// 素因数分解: 60 -> {{2, 2}, {3, 1}, {5, 1}}
auto p_factors = sieve.factorize(60);

// 約数列挙: 12 -> {1, 2, 3, 4, 6, 12}
auto divs = sieve.divisors(12);
```

---

## 4. 注意点
1. **メモリ使用量**:
   - `int` 型の配列長 $N$ は、$N = 10^7$ の場合で約 $40\text{MB}$ です。AtCoder のメモリ制限（通常 1024MB）であれば余裕を持って確保できます。
2. **$X > MAX$ の場合**:
   - 前計算範囲を超える値に対しては、通常の $O(\sqrt{X})$ の素因数分解関数（または Miller-Rabin / Pollard's rho）を使う必要があります。
