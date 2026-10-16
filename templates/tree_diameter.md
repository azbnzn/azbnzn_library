# 木の直径 (Tree Diameter)

木構造において、最も遠い 2 頂点間の距離（最遠頂点間距離）を $O(N)$ で求めるアルゴリズムです。

---

## 1. 概要・いつ使うか
- **計算量**: $O(N)$（BFS / DFS を 2 回行うのみ）
- **使える条件**: 辺の重みが非負（重みなし木の場合はすべての重みを 1 とみなす）。
- **アルゴリズムの流れ**:
  1. 任意の頂点（例: 頂点 0）から最も遠い頂点 $u$ を探索。
  2. 頂点 $u$ から最も遠い頂点 $v$ を探索。
  3. $u$ と $v$ の距離が**木の直径**であり、$u, v$ は**直径の両端点**となります。
- **取得できる情報**:
  - `diameter`: 直径（最遠距離）
  - `u, v`: 直径の両端点
  - `path`: 直径を構成する頂点列（$u$ から $v$ までのパス）
  - `get_centers()`: 木の中心（パスの中央にある頂点、1個または2個）

---

## 2. コピペ用コード (C++)

```cpp
template <typename T = long long>
struct TreeDiameter {
    struct Edge {
        int to;
        T cost;
    };

    int n;
    vector<vector<Edge>> g;
    T diameter;
    int u, v; // 直径の両端点
    vector<int> path; // 直径を構成する頂点列 (u -> ... -> v)

    TreeDiameter(int n) : n(n), g(n), diameter(0), u(0), v(0) {}

    // 無向辺の追加 (重みなしの場合は省略して cost = 1)
    void add_edge(int from, int to, T cost = 1) {
        g[from].push_back({to, cost});
        g[to].push_back({from, cost});
    }

    void build() {
        // 1. 頂点 0 から最遠点 u を探索
        auto [far_u, dist_u, parent_u] = farthest(0);
        u = far_u;

        // 2. 頂点 u から最遠点 v を探索
        auto [far_v, dist_v, parent_v] = farthest(u);
        v = far_v;
        diameter = dist_v;

        // 3. パス復元
        path.clear();
        for (int cur = v; cur != -1; cur = parent_v[cur]) {
            path.push_back(cur);
        }
        reverse(path.begin(), path.end());
    }

    // 木の中心 (直径パスの中央の頂点 1個または2個)
    vector<int> get_centers() {
        if (path.empty()) build();
        int sz = path.size();
        if (sz % 2 == 1) {
            return {path[sz / 2]};
        } else {
            return {path[sz / 2 - 1], path[sz / 2]};
        }
    }

private:
    tuple<int, T, vector<int>> farthest(int start) {
        vector<T> dist(n, -1);
        vector<int> parent(n, -1);
        queue<int> que;

        dist[start] = 0;
        que.push(start);

        int far_node = start;
        T max_dist = 0;

        while (!que.empty()) {
            int cur = que.front();
            que.pop();

            if (dist[cur] > max_dist) {
                max_dist = dist[cur];
                far_node = cur;
            }

            for (const auto& e : g[cur]) {
                if (dist[e.to] == -1) {
                    dist[e.to] = dist[cur] + e.cost;
                    parent[e.to] = cur;
                    que.push(e.to);
                }
            }
        }
        return {far_node, max_dist, parent};
    }
};
```

---

## 3. 使い方例

```cpp
int n;
cin >> n;
TreeDiameter<long long> td(n);

for (int i = 0; i < n - 1; i++) {
    int a, b;
    long long w;
    cin >> a >> b >> w;
    a--; b--; // 0-indexed
    td.add_edge(a, b, w);
}

td.build();

cout << "直径の長さ: " << td.diameter << "\n";
cout << "端点: " << td.u << " と " << td.v << "\n";

// 直径を構成する頂点列
for (int node : td.path) {
    cout << node + 1 << " ";
}
cout << "\n";
```

---

## 4. 応用
1. **木の同型判定 (Tree Isomorphism)**:
   - 木の中心を根（中心が2個ある場合は仮想頂点か2通り試す）にして木ハッシュを計算することで、木の形状が一致するかを判定できます。
2. **すべての頂点に対する最遠点（木の高さ）**:
   - 頂点 $u$ からの距離配列と頂点 $v$ からの距離配列をそれぞれ求めると、任意の頂点 $x$ に対する最遠点距離は $\max(\text{dist}(u, x), \text{dist}(v, x))$ となります。
