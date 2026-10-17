# マージテク (Small to Large)

2つのデータ構造をマージする際、「小さい方を大きい方にマージする（`swap` してから要素を移す）」ことで、全体の計算量を大幅に削減する定石テクニックです。  
構造体を作らず、`solve()` / `main()` 内に直書きできるスタイル（`3.cpp` 準拠）です。

---

## 1. 概要・特徴
- **計算量**: 全体で $O(N \log N)$（要素の移動回数が高々 $\log N$ 回に収まるため）
- **変数名ルール**（`3.cpp` スタイル）:
  - `g`: 各集合に属する要素リスト (`vector<vector<int>> g(n);`)
  - `par`: 各要素の代表元 (`vector<int> par(n);`)
  - 代表元に揃える: `u = par[u]; v = par[v];`
  - サイズ比較とマージ:
    - `if (g[u].size() < g[v].size()) swap(u, v);`
    - 小さい方 (`g[v]`) から大きい方 (`g[u]`) へ `pb` し、`par[i] = u` を更新
    - `g[v].clear();`

---

## 2. コピペ用コード (直書き)

### パターン 1: クエリによる集合マージ (3.cpp スタイル)
```cpp
// 初期化: 各頂点を独立した集合とし、代表元を自分自身に設定
vector<vector<int>> g(n);
vector<int> par(n);
rep(i, n) {
  g[i].pb(i);
  par[i] = i;
}

// 頂点 u と 頂点 v の属する集合をマージする処理 (Union-Find 的な操作)
u = par[u];
v = par[v];
if (u != v) {
  // サイズが小さい方 (v) を大きい方 (u) へマージ
  if (g[u].size() < g[v].size()) {
    swap(u, v);
  }
  for (auto &i : g[v]) {
    g[u].pb(i);
    par[i] = u;
  }
  g[v].clear();
}
```

### パターン 2: 木上の部分木マージ (DFS 直書き)
```cpp
// 各頂点 u の部分木の要素 (色や値など) を葉から親へマージする
vector<vector<int>> tree(n);
vector<set<int>> st(n); // または vector<vector<int>> st(n);
vector<int> ans(n);

auto dfs = [&](auto self, int u, int p) -> void {
  st[u].insert(color[u]);

  for (int v : tree[u]) {
    if (v == p) continue;
    self(self, v, u);

    // 小さい方 (v) を大きい方 (u) へマージ
    if (st[u].size() < st[v].size()) {
      swap(st[u], st[v]);
    }
    for (auto &x : st[v]) {
      st[u].insert(x);
    }
    st[v].clear();
  }

  // 部分木 u に対する答えを記録 (例: 部分木内の色の種類数)
  ans[u] = st[u].size();
};
dfs(dfs, 0, -1);
```

---

## 3. 注意点
- **`std::swap` は $O(1)$**:
  - `vector` や `set` の `swap(a, b)` は内部ポインタの付け替えのみを行うため $O(1)$ で完了します。
- **代表元の更新**:
  - `par[i] = u` を忘れると、移動後の要素が古い代表元を参照してバグになります。
