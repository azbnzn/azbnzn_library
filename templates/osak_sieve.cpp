#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/* =========================================================================
   [エラトステネスの篩 ＋ 高速素因数分解 (osa_k法)]
   ・min_factor[x] (最小素因数) を前計算することで、1回あたり O(log X) で素因数分解。
   ・前計算: O(MAX log log MAX)
   ・クエリ: O(log X) で素因数分解、約数列挙
   ========================================================================= */

// ==================== [コピペここから: osa_k法] ====================
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

    // 素数判定 (x <= max_val)
    bool is_prime(int x) const {
        if (x < 2) return false;
        return min_factor[x] == x;
    }

    // 素因数分解: {素因数, 個数} の昇順リスト O(log x)
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

    // 素因数のみのリスト (重複なし)
    vector<int> prime_factors(int x) const {
        vector<int> res;
        while (x > 1) {
            int p = min_factor[x];
            res.push_back(p);
            while (x % p == 0) x /= p;
        }
        return res;
    }

    // 約数列挙 (ソート済み)
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
// ==================== [コピペここまで: osa_k法] ====================


// --- 動作確認テスト ---
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int max_n = 1000000;
    OsakSieve sieve(max_n);

    cout << "=== osa_k Sieve Test Output ===" << "\n";
    cout << "Primes up to 30: ";
    for (int p : sieve.primes) {
        if (p > 30) break;
        cout << p << " ";
    }
    cout << "\n";

    // 72 の素因数分解: 2^3 * 3^2
    int x = 72;
    cout << "Factorize " << x << ": ";
    for (auto [p, exp] : sieve.factorize(x)) {
        cout << p << "^" << exp << " ";
    }
    cout << "\n";

    // 72 の約数列挙
    cout << "Divisors of " << x << ": ";
    for (int d : sieve.divisors(x)) cout << d << " ";
    cout << "\n";

    return 0;
}
