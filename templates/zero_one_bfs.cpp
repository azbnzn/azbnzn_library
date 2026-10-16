#include <bits/stdc++.h>
using namespace std;

/* =========================================================================
   [01-BFS (コスト 0/1 の最短路アルゴリズム)]
   ・辺の重みが 0 または 1 のみの場合、deque を使うことで O(V + E) で解ける。
   ・コスト 0 の遷移は push_front()、コスト 1 の遷移は push_back()。
   ・ダイクストラの O((V + E) log V) より圧倒的に高速。
   ========================================================================= */

// ==================== [コピペここから: グラフ版 01-BFS] ====================
struct Edge01 {
    int to;
    int cost; // 0 or 1
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
                    if (e.cost == 0) {
                        deq.push_front(e.to); // コスト0は最優先で展開
                    } else {
                        deq.push_back(e.to);  // コスト1は後ろに追加
                    }
                }
            }
        }
    }

    // 経路復元
    vector<int> get_path(int goal) {
        if (dist[goal] == INF_01) return {};
        vector<int> path;
        for (int cur = goal; cur != -1; cur = prev[cur]) path.push_back(cur);
        reverse(path.begin(), path.end());
        return path;
    }
};
// ==================== [コピペここまで: グラフ版] ====================


// ==================== [コピペここから: グリッド版 01-BFS] ====================
// 例: '.' はコスト0で移動可能, '#' は壁を壊すコスト1で移動可能
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
// ==================== [コピペここまで: グリッド版] ====================


// --- 動作確認テスト ---
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 頂点 4 個のグラフ
    ZeroOneBFS bfs(4);
    bfs.add_edge(0, 1, 1);
    bfs.add_edge(0, 2, 0);
    bfs.add_edge(2, 3, 0);
    bfs.add_edge(1, 3, 0);

    bfs.build(0);

    cout << "=== 01-BFS Test Output ===" << "\n";
    cout << "Dist to 3: " << bfs.dist[3] << "\n"; // 0 -> 2 -> 3 (cost 0)
    auto path = bfs.get_path(3);
    cout << "Path to 3: ";
    for (int v : path) cout << v << " ";
    cout << "\n";

    // グリッドテスト (3x3)
    vector<string> grid = {
        ".#.",
        "#.#",
        "..#"
    };
    auto gdist = grid_01bfs(3, 3, 0, 0, grid);
    cout << "Grid dist to (2, 2): " << gdist[2][2] << "\n";

    return 0;
}
