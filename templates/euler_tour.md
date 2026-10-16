# 木のオイラーツアー (Euler Tour)

木構造を深さ優先探索（DFS）順で走査し、各頂点の訪問開始時刻・終了時刻を記録することで、木を 1 次元の配列（区間）に平坦化するテクニックです。

---

## 1. 概要・いつ使うか
- **計算量**: 構築 $O(N)$
- **最大のメリット**:
  - 頂点 $u$ の「部分木全体」が、オイラーツアー配列上の**連続する半開区間 `[in[u], out[u])`** に一対一対応します。
  - これにより、**木に対するクエリ（部分木の和、部分木の最大値、部分木全体への一括加算など）を、そのままセグメント木や BIT の「区間クエリ」として処理できるようになります**。
- **典型問題**:
  - 部分木更新・部分木取得（セグ木 / 遅延セグ木と併用）
  - 部分木内の色の種類数（Mo法と併用）
  - 根からのパスに対する累積値クエリ

---

## 2. コピペ用コード (C++)

```cpp
struct EulerTour {
    int n;
    vector<vector<int>> g;
    vector<int> in, out;          // 部分木 u に対応する区間 [in[u], out[u])
    vector<int> tour;             // 訪問順の頂点列 (サイズ n)
    vector<int> depth;            // 各頂点の深さ (根は 0)
    vector<int> parent;           // 親頂点 (根は -1)
    int timer;

    EulerTour(int n) : n(n), g(n), in(n), out(n), tour(n), depth(n, 0), parent(n, -1), timer(0) {}

    void add_edge(int u, int v) {
        g[u].push_back(v);
        g[v].push_back(u);
    }

    void build(int root = 0) {
        timer = 0;
        dfs(root, -1, 0);
    }

    // 部分木 u の区間 [l, r) を取得
    pair<int, int> subtree_range(int u) const {
        return {in[u], out[u]};
    }

private:
    void dfs(int u, int p, int d) {
        parent[u] = p;
        depth[u] = d;
        in[u] = timer;
        tour[timer] = u;
        timer++;

        for (int v : g[u]) {
            if (v == p) continue;
            dfs(v, u, d + 1);
        }
        out[u] = timer;
    }
};
```

---

## 3. セグメント木 / BIT との連携パターン (部分木クエリ)

AtCoder Library の `fenwick_tree` や `segtree` と組み合わせる定番パターンです：

```cpp
#include <atcoder/fenwicktree>
using namespace atcoder;

int n, q;
cin >> n >> q;
vector<long long> val(n);
for (int i = 0; i < n; i++) cin >> val[i];

EulerTour et(n);
for (int i = 0; i < n - 1; i++) {
    int u, v; cin >> u >> v;
    u--; v--;
    et.add_edge(u, v);
}
et.build(0); // 頂点 0 を根として構築

// オイラーツアー順の BIT を作成
fenwick_tree<long long> bit(n);
for (int u = 0; u < n; u++) {
    bit.add(et.in[u], val[u]);
}

// クエリ処理
while (q--) {
    int type; cin >> type;
    if (type == 1) {
        // 頂点 u の値を x 加算
        int u; long long x;
        cin >> u >> x;
        u--;
        bit.add(et.in[u], x);
    } else {
        // 部分木 u の総和を取得
        int u; cin >> u;
        u--;
        auto [l, r] = et.subtree_range(u);
        cout << bit.sum(l, r) << "\n";
    }
}
```

---

## 4. 注意点
1. **頂点 $u$ 単体 vs 部分木 $u$**:
   - 頂点 $u$ 単体の位置は `et.in[u]` です。
   - 頂点 $u$ を根とする部分木全体の区間は `[et.in[u], et.out[u])` です。
2. **葉頂点の場合**:
   - 葉頂点 $u$ の場合、`out[u] = in[u] + 1` となり、区間長は 1 になります。
