# 黄金分割探索 (Golden Section Search)

下に凸（または上に凸）な 1 峰性関数の極値を求める探索アルゴリズムです。三分探索と同様の目的で使われますが、黄金比を利用することで**前のステップの関数評価値を再利用でき、関数呼び出し回数を約 40% 削減**できます。

---

## 1. 概要・いつ使うか
- **計算量**: 反復回数を $K$ 回とすると、関数評価回数は $K + 1$ 回（三分探索の $2K$ 回に比べて半分近く少ない）。
- **いつ使うか**:
  - **評価関数 $f(x)$ の計算が重い場合**（関数内部でダイクストラ、シミュレーション、グラフ探索、$O(N)$ の集計処理などを行っている場合）。
  - 時間制限（2.0秒など）が厳しく、三分探索だと TLE する恐れがある場合。

---

## 2. コピペ用コード (C++)

### 1. 実数版 (下に凸・最小値探索)
```cpp
// [low, high] の区間から最小値をとる x を求める (反復 iter 回)
double golden_section_search_real(double low, double high, const function<double(double)>& f, int iter = 100) {
    const double phi = (1.0 + sqrt(5.0)) / 2.0;
    const double r = 1.0 / phi; // ≈ 0.6180339887...

    double x1 = high - r * (high - low);
    double x2 = low + r * (high - low);
    double f1 = f(x1);
    double f2 = f(x2);

    for (int i = 0; i < iter; i++) {
        if (f1 < f2) {
            high = x2;
            x2 = x1;
            f2 = f1; // 評価値を再利用
            x1 = high - r * (high - low);
            f1 = f(x1);
        } else {
            low = x1;
            x1 = x2;
            f1 = f2; // 評価値を再利用
            x2 = low + r * (high - low);
            f2 = f(x2);
        }
    }
    return (low + high) / 2.0;
}
```

### 2. 整数版 (下に凸・最小値探索・安全設計)
```cpp
ll golden_section_search_int(ll low, ll high, const function<ll(ll)>& f) {
    const double r = (sqrt(5.0) - 1.0) / 2.0;
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
```

---

## 3. 最大値（上に凸）を探したい場合
`f1 < f2` を **`f1 > f2`** に変更するか、評価関数の戻り値の符号を反転（`-f(x)` を最小化）してください。
