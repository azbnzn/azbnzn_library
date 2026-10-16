#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/* =========================================================================
   [木の直径 (Tree Diameter - 任意重み対応・パス復元・中心取得)]
   ・2回の探索 (BFS/DFS) で木の最遠点対 (直径) を O(N) で求める。
   ・辺の重みが非負の実数/整数 (重みなしは重み1) に対応。
   ・直径の長さ、直径をなすパス (頂点列)、木の中心 (1〜2頂点) を取得可能。
   ========================================================================= */

// ==================== [コピペここから: 木の直径] ====================
template <typename T = ll>
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

    // 無向辺の追加 (重みなしの場合は cost = 1)
    void add_edge(int from, int to, T cost = 1) {
        g[from].push_back({to, cost});
        g[v = to].push_back({from, cost}); // 便宜上一時利用
        g[to].pop_back();                  // 正しい追加
        g[to].push_back({from, cost});
    }

    void build() {
        // 1. 任意の頂点 0 から最遠点 u を探索
        auto [far_u, dist_u, parent_u] = farthest(0);
        u = far_u;

        // 2. 頂点 u から最遠点 v を探索
        auto [far_v, dist_v, parent_v] = farthest(u);
        v = far_v;
        diameter = dist_v;

        // 3. パス復元 (v から u へ親を辿る)
        path.clear();
        for (int cur = v; cur != -1; cur = parent_v[cur]) {
            path.push_back(cur);
        }
        reverse(path.begin(), path.end());
    }

    // 木の中心 (直径パスの中央に位置する頂点 1個または2個)
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
    // start から各頂点への距離と親を求める
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
// ==================== [コピペここまで: 木の直径] ====================


// --- 動作確認テスト ---
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 頂点数 6 の木
    // 0 - 1 (cost 3)
    // 1 - 2 (cost 2)
    // 2 - 3 (cost 5)
    // 2 - 4 (cost 1)
    // 4 - 5 (cost 4)
    //
    // パス 0 - 1 - 2 - 3: 3 + 2 + 5 = 10
    // パス 3 - 2 - 4 - 5: 5 + 1 + 4 = 10
    // パス 0 - 1 - 2 - 4 - 5: 3 + 2 + 1 + 4 = 10 (直径はどれも 10)
    TreeDiameter<ll> td(6);
    td.add_edge(0, 1, 3);
    td.add_edge(1, 2, 2);
    td.add_edge(2, 3, 5);
    td.add_edge(2, 4, 1);
    td.add_edge(4, 5, 4);

    td.build();

    cout << "=== Tree Diameter Test Output ===" << "\n";
    cout << "Diameter: " << td.diameter << "\n";
    cout << "End points: " << td.u << " <-> " << td.v << "\n";
    cout << "Diameter Path: ";
    for (int node : td.path) cout << node << " ";
    cout << "\n";

    cout << "Centers: ";
    for (int c : td.get_centers()) cout << c << " ";
    cout << "\n";

    return 0;
}
