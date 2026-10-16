# ダイクストラ法 (Dijkstra)

辺の重みが非負であるグラフにおいて、単一の始点から他のすべての頂点への最短距離を求めるアルゴリズムです。

---

## 1. 概要・いつ使うか
- **計算量**: $O((V + E) \log V)$
- **使える条件**: すべての辺の重み（コスト）が **0 以上** であること（負の重みがある場合はベルマンフォード法等が必要）。
- **特徴**:
  - `priority_queue`（最小ヒープ）を用いて、未確定頂点のうち距離が最小のものを順次確定させていきます。
  - 経路復元用配列 `prev` を保持することで、最短経路上の頂点列を復元できます。

---

## 2. コピペ用コード (C++)

### グラフ版ダイクストラ (経路復元対応)
```cpp
using ll = long long;
const ll INF = 4e18; // オーバーフロー防止のため 4e18 を推奨

struct Edge {
    int to;
    ll cost;
};

struct Dijkstra {
    int n;
    vector<vector<Edge>> g;
    vector<ll> dist;
    vector<int> prev;

    Dijkstra(int n) : n(n), g(n), dist(n, INF), prev(n, -1) {}

    // 有向辺の追加 (無向辺は双方向に追加)
    void add_edge(int from, int to, ll cost) {
        g[from].push_back({to, cost});
    }

    void build(int start) {
        dist.assign(n, INF);
        prev.assign(n, -1);
        
        priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
        dist[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (d > dist[u]) continue;

            for (const auto& e : g[u]) {
                if (dist[e.to] > dist[u] + e.cost) {
                    dist[e.to] = dist[u] + e.cost;
                    prev[e.to] = u;
                    pq.push({dist[e.to], e.to});
                }
            }
        }
    }

    // start から goal への経路を復元 (到達不可なら空配列)
    vector<int> get_path(int goal) {
        if (dist[goal] == INF) return {};
        vector<int> path;
        for (int cur = goal; cur != -1; cur = prev[cur]) path.push_back(cur);
        reverse(path.begin(), path.end());
        return path;
    }
};
```

### グリッド版ダイクストラ (マス目の移動コスト)
```cpp
using ll = long long;
const ll INF = 4e18;

// H x W グリッドの最短経路
vector<vector<ll>> grid_dijkstra(int h, int w, int start_r, int start_c,
                                 const vector<vector<ll>>& grid_cost) {
    const vector<int> dr = {0, 0, 1, -1};
    const vector<int> dc = {1, -1, 0, 0};
    vector<vector<ll>> dist(h, vector<ll>(w, INF));
    
    using State = pair<ll, pair<int, int>>;
    priority_queue<State, vector<State>, greater<State>> pq;

    dist[start_r][start_c] = 0;
    pq.push({0, {start_r, start_c}});

    while (!pq.empty()) {
        auto [d, pos] = pq.top();
        auto [r, c] = pos;
        pq.pop();

        if (d > dist[r][c]) continue;

        for (int k = 0; k < 4; k++) {
            int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nr >= h || nc < 0 || nc >= w) continue;
            
            ll next_cost = grid_cost[nr][nc];
            if (dist[nr][nc] > dist[r][c] + next_cost) {
                dist[nr][nc] = dist[r][c] + next_cost;
                pq.push({dist[nr][nc], {nr, nc}});
            }
        }
    }
    return dist;
}
```

---

## 3. 注意点
1. **INF の値とオーバーフロー**:
   - `INT_MAX` だと `dist[u] + cost` でオーバーフローします。`long long` を使い、`4e18` など十分大きい値を設定してください。
2. **`if (d > dist[u]) continue;` を忘れない**:
   - これがないと、同じ頂点がヒープから何度も無駄に取り出され、最悪 $O(E \log V)$ ではなく $O(E^2)$ や TLE の原因になります。
3. **コストが 0 または 1 だけの場合**:
   - ダイクストラ法よりも **01-BFS**（`std::deque`）を使うほうが $O(V + E)$ で圧倒的に高速です。
