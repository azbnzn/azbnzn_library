# Mo's Algorithm (Mo法)

区間クエリをオフライン（先読み）で並び替え、区間の両端点を少しずつ伸縮させながら $O(N \sqrt{Q})$ で効率的に処理するアルゴリズムです。

---

## 1. 概要・いつ使うか
- **使える条件**:
  1. 配列の値の更新クエリがない（オフラインで全クエリを先読み可能）。
  2. 区間 $[l, r)$ の計算結果が分かっているとき、$[l-1, r), [l+1, r), [l, r-1), [l, r+1)$ の状態を $O(1)$ または $O(\log N)$ で遷移できる。
- **典型問題**:
  - 区間内の相異なる要素数
  - 区間内の各値の出現頻度の二乗和（$c^2$ や $c \times x$）
  - 区間内の転倒数（BIT と併用して $O(N \sqrt{Q} \log N)$）
  - 木上のパス・部分木に対する Mo（オイラーツアーと併用）

---

## 2. 計算量
- **時間計算量**: $O(N \sqrt{Q})$ （状態遷移が $O(1)$ の場合）
  - 偶奇ソート（ブロック番号が奇数のときに右端点を降順にソート）により、右端点の折り返し無駄移動をなくし定数倍を約半分に削減しています。
- **空間計算量**: $O(N + Q)$

---

## 3. コピペ用コード (C++)

```cpp
struct Mo {
    int n;
    struct Query {
        int l, r, id;
    };
    vector<Query> queries;

    Mo(int n) : n(n) {}

    // 半開区間 [l, r) を追加 (0-indexed)
    void add_query(int l, int r) {
        queries.push_back({l, r, (int)queries.size()});
    }

    // add(idx), del(idx), out(query_id) を渡して実行
    template <typename Add, typename Del, typename Out>
    void run(const Add& add, const Del& del, const Out& out) {
        int q = queries.size();
        if (q == 0) return;

        int bs = max(1, (int)(n / max(1.0, sqrt(q * 2.0 / 3.0))));
        
        // 偶奇ソートで定数倍高速化
        sort(queries.begin(), queries.end(), [&](const Query& a, const Query& b) {
            int ablock = a.l / bs, bblock = b.l / bs;
            if (ablock != bblock) return ablock < bblock;
            return (ablock & 1) ? (a.r > b.r) : (a.r < b.r);
        });

        int cur_l = 0, cur_r = 0;
        for (const auto& qry : queries) {
            while (cur_l > qry.l) { cur_l--; add(cur_l); }
            while (cur_r < qry.r) { add(cur_r); cur_r++; }
            while (cur_l < qry.l) { del(cur_l); cur_l++; }
            while (cur_r > qry.r) { cur_r--; del(cur_r); }
            out(qry.id);
        }
    }
};
```

---

## 4. 使い方と実装テンプレート

```cpp
int n, q;
cin >> n >> q;
vector<int> a(n);
for (int i = 0; i < n; i++) cin >> a[i];

Mo mo(n);
for (int i = 0; i < q; i++) {
    int l, r;
    cin >> l >> r;
    l--; // 0-indexed [l, r) に変換
    mo.add_query(l, r);
}

vector<long long> ans(q);
vector<int> cnt(1000005, 0); // 座標圧縮が必要な場合は事前に行う
int distinct_kinds = 0;

auto add = [&](int idx) {
    int val = a[idx];
    if (cnt[val] == 0) distinct_kinds++;
    cnt[val]++;
};

auto del = [&](int idx) {
    int val = a[idx];
    cnt[val]--;
    if (cnt[val] == 0) distinct_kinds--;
};

auto out = [&](int q_id) {
    ans[q_id] = distinct_kinds;
};

mo.run(add, del, out);

for (int i = 0; i < q; i++) {
    cout << ans[i] << "\n";
}
```

---

## 5. 注意点
1. **0-indexed 半開区間 `[l, r)` で渡す**:
   - 1-indexed の問題では `add_query(l - 1, r)` のように変換してください。
2. **値域が大きい場合**:
   - `a[i]` の値が大きい場合は、あらかじめ座標圧縮（`CoordinateCompression`）を行って `0 <= a[i] < N` に収めておくと、出現回数配列 `cnt` が高速にアクセスできます。
3. **左右で操作が非対称な場合**:
   - `mos_algorithm.cpp` に用意してある `run_lr(add_l, add_r, del_l, del_r, out)` を利用してください。
