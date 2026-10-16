# マージテク (Small to Large / DSU on Tree)

2つのデータ構造をマージする際、「小さい方を大きい方にマージする（`swap` してから要素を移す）」ことで、全体の計算量を大幅に削減する定石テクニックです。

---

## 1. 概要・いつ使うか
- **使える場面**:
  1. **集合（`set`, `map`, `vector`）のマージ**:
     - 木の葉から根に向かって情報を統合するとき、ナイーブにマージすると最悪 $O(N^2)$ になりますが、`if (a.size() < b.size()) swap(a, b);` とすることで各要素の移動回数が高々 $O(\log N)$ 回に抑えられ、全体で $O(N \log^2 N)$（またはハッシュで $O(N \log N)$）になります。
  2. **木上のマージテク (DSU on Tree / Sack)**:
     - 根付き木の全頂点 $u$ について、「$u$ の部分木内に条件を満たす頂点が何個あるか？」に答えるアルゴリズムです。
     - セグメント木や Heavy-Light Decomposition (HLD) を使わずに、グローバルな集計配列 1 つと再帰だけで $O(N \log N)$ で解くことができます。

---

## 2. コピペ用コード (C++)

### パターン 1: 単純な set / map のマージ
```cpp
// 小さい set を大きい set へマージ (全体で O(N log^2 N))
template <typename T>
void merge_set(set<T>& big, set<T>& small) {
    if (big.size() < small.size()) swap(big, small);
    for (const auto& elem : small) {
        big.insert(elem);
    }
    small.clear();
}

// 小さい map を大きい map へマージ (頻度集計など)
template <typename Key, typename Val>
void merge_map(map<Key, Val>& big, map<Key, Val>& small) {
    if (big.size() < small.size()) swap(big, small);
    for (const auto& p : small) {
        big[p.first] += p.second;
    }
    small.clear();
}
```

### パターン 2: 木上のマージテク (DSU on Tree / Sack)
```cpp
struct DsuOnTree {
    int n;
    vector<vector<int>> g;
    vector<int> sz, heavy, in, out, node_by_in;
    int timer;

    DsuOnTree(int n) : n(n), g(n), sz(n), heavy(n, -1), in(n), out(n), node_by_in(n), timer(0) {}

    void add_edge(int u, int v) {
        g[u].push_back(v);
        g[v].push_back(u);
    }

    void build_tree(int root = 0) {
        auto dfs_size = [&](auto self, int u, int p) -> void {
            sz[u] = 1;
            in[u] = timer;
            node_by_in[timer] = u;
            timer++;
            int max_sub = -1;
            for (int v : g[u]) {
                if (v == p) continue;
                self(self, v, u);
                sz[u] += sz[v];
                if (sz[v] > max_sub) {
                    max_sub = sz[v];
                    heavy[u] = v; // 最もサイズが大きい子
                }
            }
            out[u] = timer;
        };
        timer = 0;
        dfs_size(dfs_size, root, -1);
    }

    // add(u): 頂点 u を集計配列に追加
    // del(u): 頂点 u を集計配列から削除
    // query(u): 部分木 u の答えを記録
    template <typename Add, typename Del, typename Query>
    void run(const Add& add, const Del& del, const Query& query, int root = 0) {
        auto dfs = [&](auto self, int u, int p, bool keep) -> void {
            // 1. 軽い子を探索 (計算後に状態をクリア keep = false)
            for (int v : g[u]) {
                if (v != p && v != heavy[u]) self(self, v, u, false);
            }
            // 2. 最も重い子を探索 (計算結果を維持 keep = true)
            if (heavy[u] != -1) self(self, heavy[u], u, true);
            
            // 3. 自分と軽い子の部分木を追加
            add(u);
            for (int v : g[u]) {
                if (v != p && v != heavy[u]) {
                    for (int t = in[v]; t < out[v]; t++) add(node_by_in[t]);
                }
            }
            // 4. 部分木 u に対する集計・クエリ処理
            query(u);

            // 5. 軽い子だった場合は状態を元に戻す
            if (!keep) {
                for (int t = in[u]; t < out[u]; t++) del(node_by_in[t]);
            }
        };

        dfs(dfs, root, -1, false);
    }
};
```

---

## 3. DSU on Tree の使い方 (部分木の色種類数カウント例)

```cpp
int n;
cin >> n;
vector<int> color(n);
for (int i = 0; i < n; i++) cin >> color[i];

DsuOnTree dot(n);
for (int i = 0; i < n - 1; i++) {
    int u, v;
    cin >> u >> v;
    u--; v--; // 0-indexed
    dot.add_edge(u, v);
}
dot.build_tree(0);

vector<int> ans(n);
vector<int> cnt(1000005, 0);
int distinct_colors = 0;

auto add = [&](int u) {
    int c = color[u];
    if (cnt[c] == 0) distinct_colors++;
    cnt[c]++;
};

auto del = [&](int u) {
    int c = color[u];
    cnt[c]--;
    if (cnt[c] == 0) distinct_colors--;
};

auto query = [&](int u) {
    ans[u] = distinct_colors;
};

dot.run(add, del, query, 0);

for (int i = 0; i < n; i++) {
    cout << ans[i] << "\n";
}
```

---

## 4. 注意点・Tips
- **C++ の `std::swap`**:
  - `std::vector`, `std::set`, `std::map` の `swap(a, b)` はポインタの付け替えのみを行うため $O(1)$ です。
- **DSU on Tree の強み**:
  - 重い子（Heavy Child）の計算結果をそのまま残すため、不要な削除・再追加が発生せず、各頂点は高々 $O(\log N)$ 回しか追加されません。
  - セグ木が乗らない複雑な集計（最頻値の個数、MEX、特定深さの要素数など）も配列や変数を直接更新できるため非常に書きやすいです。
