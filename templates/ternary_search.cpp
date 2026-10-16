#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/* =========================================================================
   [三分探索 (Ternary Search)]
   ・下に凸 (または上に凸) な関数の最小値・最大値を求める。
   ・実数版: 反復回数を固定 (例: 100回) して精度を保証。
   ・整数版: 区間幅が小さくなったら直接全探索して極値を確定する安全設計。
   ========================================================================= */

// ==================== [コピペここから: 三分探索] ====================

// --- 1. 実数版 (下に凸な関数の最小値探索) ---
// low, high: 探索区間 [low, high]
// f: 評価関数 (double -> double)
// iter: ループ回数 (80〜100回で十分な精度 double 精度 1e-12 程度)
double ternary_search_real(double low, double high, const function<double(double)>& f, int iter = 100) {
    for (int i = 0; i < iter; i++) {
        double m1 = low + (high - low) / 3.0;
        double m2 = high - (high - low) / 3.0;
        if (f(m1) < f(m2)) {
            high = m2; // 最小値は [low, m2] に存在
        } else {
            low = m1;  // 最小値は [m1, high] に存在
        }
    }
    return (low + high) / 2.0;
}

// --- 2. 整数版 (下に凸な関数の最小値をとる x を探索) ---
// low, high: 探索区間 [low, high] (両端点を含む)
// f: 評価関数 (ll -> ll / double)
// 戻り値: f(x) が最小となる x (複数ある場合はいずれか)
ll ternary_search_int(ll low, ll high, const function<ll(ll)>& f) {
    while (high - low > 2) {
        ll m1 = low + (high - low) / 3;
        ll m2 = high - (high - low) / 3;
        if (f(m1) < f(m2)) {
            high = m2;
        } else {
            low = m1;
        }
    }
    // 残った候補 (low, low+1, ..., high) を直接全探索して確実に最小値を返す
    ll best_x = low;
    ll min_val = f(low);
    for (ll x = low + 1; x <= high; x++) {
        ll val = f(x);
        if (val < min_val) {
            min_val = val;
            best_x = x;
        }
    }
    return best_x;
}
// ==================== [コピペここまで: 三分探索] ====================


// --- 動作確認テスト ---
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 実数テスト: f(x) = (x - 2.5)^2 + 10 の最小値 -> x = 2.5
    auto f_real = [](double x) {
        return (x - 2.5) * (x - 2.5) + 10.0;
    };
    double opt_x = ternary_search_real(0.0, 10.0, f_real);
    cout << "=== Ternary Search Test Output ===" << "\n";
    cout << fixed << setprecision(6);
    cout << "Real optimum x: " << opt_x << " (Expected: 2.5)" << "\n";

    // 整数テスト: f(x) = (x - 7)^2 + 3 の最小値 -> x = 7
    auto f_int = [](ll x) {
        return (x - 7) * (x - 7) + 3;
    };
    ll opt_int = ternary_search_int(-100, 100, f_int);
    cout << "Integer optimum x: " << opt_int << " (Expected: 7)" << "\n";

    return 0;
}
