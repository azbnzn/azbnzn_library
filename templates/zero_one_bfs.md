# 01-BFS

グラフの辺の重みが **0 または 1 のみ** の場合、`std::deque` を用いることで $O(V + E)$ の線形時間で最短路を求めるアルゴリズムです。

---

## 1. 概要・いつ使うか
- **計算量**: $O(V + E)$ （ダイクストラの $O((V + E) \log V)$ より高速で定数倍も軽い）
- **使える条件**: 辺の重みがすべて $0$ または $1$ であること。
- **アルゴリズムの仕組み**:
  - コスト 0 の辺で遷移できる頂点は、現在の距離と同じなので **`deque.push_front()`**（先頭）に追加。
  - コスト 1 の辺で遷移できる頂点は、距離が 1 増えるので **`deque.push_back()`**（末尾）に追加。
  - これにより、`deque` の中身が常に「距離 $d$ の頂点群」と「距離 $d+1$ の頂点群」の 2 種類だけに保たれ、ソート不要で最短順に取り出すことができます。
- **典型問題**:
  - 壁破壊迷路（壁を壊すコスト 1、通路を歩くコスト 0）
  - 移動方向転換（直進はコスト 0、カーブ/方向転換はコスト 1）
  - ワープ装置（通常移動コスト 1、無料ワープコスト 0）

---

## 2. コピペ用コード (C++)

### グラフ版 01-BFS
```cpp
struct Edge01 {
    int to;
    int cost; // 0 または 1
};

const int INF_01 = 1e9;

struct ZeroOneBFS {
    int n;
    vector<vector<Edge01>> g;
    vector<int> dist;
    vector<int> prev;

    ZeroOneBFS(int n) : n(n), g(n), dist(n, INF_01), prev(n, -1) {}

    void add_edge(int from, int to, int cost) {
        g[from].push_back({to, cost});
    }

    void build(int start) {
        dist.assign(n, INF_01);
        prev.assign(n, -1);
        deque<int> deq;

        dist[start] = 0;
        deq.push_back(start);

        while (!deq.empty()) {
            int u = deq.front();
            deq.pop_front();

            for (const auto& e : g[u]) {
                if (dist[e.to] > dist[u] + e.cost) {
                    dist[e.to] = dist[u] + e.cost;
                    prev[e.to] = u;
                    if (e.cost == 0) deq.push_front(e.to);
                    else deq.push_back(e.to);
                }
            }
        }
    }

    vector<int> get_path(int goal) {
        if (dist[goal] == INF_01) return {};
        vector<int> path;
        for (int cur = goal; cur != -1; cur = prev[cur]) path.push_back(cur);
        reverse(path.begin(), path.end());
        return path;
    }
};
```

### グリッド版 01-BFS (壁破壊の例)
```cpp
const int INF_01 = 1e9;

vector<vector<int>> grid_01bfs(int h, int w, int start_r, int start_c,
                               const vector<string>& grid) {
    const vector<int> dr = {0, 0, 1, -1};
    const vector<int> dc = {1, -1, 0, 0};
    vector<vector<int>> dist(h, vector<int>(w, INF_01));
    deque<pair<int, int>> deq;

    dist[start_r][start_c] = 0;
    deq.push_back({start_r, start_c});

    while (!deq.empty()) {
        auto [r, c] = deq.front();
        deq.pop_front();

        for (int k = 0; k < 4; k++) {
            int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nr >= h || nc < 0 || nc >= w) continue;

            int cost = (grid[nr][nc] == '#' ? 1 : 0);
            if (dist[nr][nc] > dist[r][c] + cost) {
                dist[nr][nc] = dist[r][c] + cost;
                if (cost == 0) deq.push_front({nr, nc});
                else deq.push_back({nr, nc});
            }
        }
    }
    return dist;
}
```

---

## 3. 注意点
1. **コストが 2 以上の辺が含まれる場合**:
   - `deque` 内の距離の単調性が崩れるため、ダイクストラ法を使用する必要があります。
2. **`dist` の更新チェック**:
   - `if (dist[to] > dist[u] + cost)` の条件判定を行ってから `push_front` / `push_back` することで、無駄な頂点挿入を防ぎ $O(V + E)$ を保証できます。
