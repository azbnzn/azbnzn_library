#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/* =========================================================================
   [黄金分割探索 (Golden Section Search)]
   ・下に凸 (または上に凸) な関数の極値を探索。
   ・三分探索と異なり、前ステップで計算した関数値を再利用できるため、
     毎反復の関数評価が「1回」で済む (関数呼び出しコストを約40%削減)。
   ・計算コストが高い関数 (重いDPやシミュレーション) の最小化に最適。
   ========================================================================= */

// ==================== [コピペここから: 黄金分割探索] ====================

// --- 1. 実数版 (下に凸・最小値探索) ---
// low, high: 探索区間 [low, high]
// f: 評価関数
// iter: ループ回数 (80〜100回で十分高精度)
double golden_section_search_real(double low, double high, const function<double(double)>& f, int iter = 100) {
    const double phi = (1.0 + sqrt(5.0)) / 2.0;
    const double r = 1.0 / phi; // ≈ 0.6180339887...

    double x1 = high - r * (high - low);
    double x2 = low + r * (high - low);
    double f1 = f(x1);
    double f2 = f(x2);

    for (int i = 0; i < iter; i++) {
        if (f1 < f2) {
            // 最小値は [low, x2]
            high = x2;
            x2 = x1;
            f2 = f1; // 再利用!
            x1 = high - r * (high - low);
            f1 = f(x1);
        } else {
            // 最小値は [x1, high]
            low = x1;
            x1 = x2;
            f1 = f2; // 再利用!
            x2 = low + r * (high - low);
            f2 = f(x2);
        }
    }
    return (low + high) / 2.0;
}

// --- 2. 整数版 (下に凸・最小値をとる x を探索) ---
ll golden_section_search_int(ll low, ll high, const function<ll(ll)>& f) {
    const double r = (sqrt(5.0) - 1.0) / 2.0; // ≈ 0.618
    while (high - low > 4) {
        ll len = high - low;
        ll x1 = low + (ll)round((1.0 - r) * len);
        ll x2 = low + (ll)round(r * len);
        if (x1 == x2) x2++;
        if (f(x1) < f(x2)) {
            high = x2;
        } else {
            low = x1;
        }
    }
    // 残りの狭い区間は直接全探索して安全に特定
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
// ==================== [コピペここまで: 黄金分割探索] ====================


// --- 動作確認テスト ---
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int call_count = 0;
    auto f_real = [&](double x) {
        call_count++;
        return (x - 3.14159) * (x - 3.14159) + 42.0;
    };

    double opt_x = golden_section_search_real(-10.0, 10.0, f_real, 80);
    cout << "=== Golden Section Search Test Output ===" << "\n";
    cout << fixed << setprecision(6);
    cout << "Real optimum x: " << opt_x << " (Expected: 3.141590)" << "\n";
    cout << "Function evaluations: " << call_count << " (Ternary search would take ~160)" << "\n";

    auto f_int = [](ll x) {
        return (x - 42) * (x - 42) + 10;
    };
    ll opt_int = golden_section_search_int(0, 100, f_int);
    cout << "Integer optimum x: " << opt_int << " (Expected: 42)" << "\n";

    return 0;
}
