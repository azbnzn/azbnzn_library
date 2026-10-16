#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/* =========================================================================
   [ダイクストラ法 (Dijkstra)]
   ・非負の辺重みを持つグラフでの単一始点最短路アルゴリズム。
   ・計算量: O((V + E) log V)
   ・経路復元、グリッドダイクストラに対応。
   ========================================================================= */

// ==================== [コピペここから: グラフ版ダイクストラ] ====================
struct Edge {
    int to;
    ll cost;
};

const ll INF = 4e18; // 十分に大きい値 (オーバーフローに注意)

struct Dijkstra {
    int n;
    vector<vector<Edge>> g;
    vector<ll> dist;
    vector<int> prev; // 経路復元用

    Dijkstra(int n) : n(n), g(n), dist(n, INF), prev(n, -1) {}

    // 有向辺の追加 (無向辺の場合は双方向に追加)
    void add_edge(int from, int to, ll cost) {
        g[from].push_back({to, cost});
    }

    void build(int start) {
        dist.assign(n, INF);
        prev.assign(n, -1);
        
        // {距離, 頂点番号} の最小ヒープ
        priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
        dist[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (d > dist[u]) continue; // 既に最短距離が更新されているならスキップ

            for (const auto& e : g[u]) {
                if (dist[e.to] > dist[u] + e.cost) {
                    dist[e.to] = dist[u] + e.cost;
                    prev[e.to] = u;
                    pq.push({dist[e.to], e.to});
                }
            }
        }
    }

    // 経路復元: start から goal への頂点列 (到達不能なら空配列)
    vector<int> get_path(int goal) {
        if (dist[goal] == INF) return {};
        vector<int> path;
        for (int cur = goal; cur != -1; cur = prev[cur]) {
            path.push_back(cur);
        }
        reverse(path.begin(), path.end());
        return path;
    }
};
// ==================== [コピペここまで: グラフ版] ====================


// ==================== [コピペここから: グリッド版ダイクストラ] ====================
// H x W グリッド上の最短路 (dx, dy 4方向移動)
vector<vector<ll>> grid_dijkstra(int h, int w, int start_r, int start_c,
                                 const vector<vector<ll>>& grid_cost) {
    const vector<int> dr = {0, 0, 1, -1};
    const vector<int> dc = {1, -1, 0, 0};
    vector<vector<ll>> dist(h, vector<ll>(w, INF));
    
    // {距離, {r, c}}
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
// ==================== [コピペここまで: グリッド版] ====================


// --- 動作確認テスト ---
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 頂点数 5, 辺数 6
    // 0 -> 1 (cost 10)
    // 0 -> 2 (cost 3)
    // 2 -> 1 (cost 2)
    // 1 -> 3 (cost 1)
    // 2 -> 3 (cost 8)
    // 3 -> 4 (cost 7)
    Dijkstra dij(5);
    dij.add_edge(0, 1, 10);
    dij.add_edge(0, 2, 3);
    dij.add_edge(2, 1, 2);
    dij.add_edge(1, 3, 1);
    dij.add_edge(2, 3, 8);
    dij.add_edge(3, 4, 7);

    dij.build(0);

    cout << "=== Dijkstra Test Output ===" << "\n";
    cout << "Dist to 4: " << dij.dist[4] << "\n"; // 0 -> 2 -> 1 -> 3 -> 4 (cost 3 + 2 + 1 + 7 = 13)
    
    auto path = dij.get_path(4);
    cout << "Path to 4: ";
    for (int v : path) cout << v << " ";
    cout << "\n";

    return 0;
}
