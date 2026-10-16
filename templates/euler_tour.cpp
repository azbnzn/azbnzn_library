#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/* =========================================================================
   [木のオイラーツアー (Euler Tour)]
   ・木構造を DFS で辿り、1次元配列 (訪問時刻) に平坦化する。
   ・部分木 u に含まれる頂点は、半開区間 [in[u], out[u]) に一対一対応する。
   ・セグ木や BIT と組み合わせることで、部分木加算・部分木和取得が O(log N) で可能。
   ========================================================================= */

// ==================== [コピペここから: オイラーツアー] ====================
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
// ==================== [コピペここまで: オイラーツアー] ====================


// --- 動作確認テスト: 部分木の要素和クエリ ---
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 頂点数 5 の木
    //       0 (val=10)
    //      / \
    // (val=20)1   2 (val=30)
    //    / \
    // (40)3   4 (50)
    int n = 5;
    vector<ll> val = {10, 20, 30, 40, 50};

    EulerTour et(n);
    et.add_edge(0, 1);
    et.add_edge(0, 2);
    et.add_edge(1, 3);
    et.add_edge(1, 4);

    et.build(0);

    // オイラーツアー順に並べた配列を作成 (BIT やセグ木に乗せる)
    vector<ll> flattened_val(n);
    for (int u = 0; u < n; u++) {
        flattened_val[et.in[u]] = val[u];
    }

    // 累積和で部分木和を検証
    vector<ll> pref(n + 1, 0);
    for (int i = 0; i < n; i++) pref[i + 1] = pref[i] + flattened_val[i];

    auto query_subtree_sum = [&](int u) {
        auto [l, r] = et.subtree_range(u);
        return pref[r] - pref[l];
    };

    cout << "=== Euler Tour Test Output ===" << "\n";
    cout << "Subtree 0 sum: " << query_subtree_sum(0) << " (Expected: 150)" << "\n";
    cout << "Subtree 1 sum: " << query_subtree_sum(1) << " (Expected: 110: 20+40+50)" << "\n";
    cout << "Subtree 2 sum: " << query_subtree_sum(2) << " (Expected: 30)" << "\n";
    cout << "Subtree 3 sum: " << query_subtree_sum(3) << " (Expected: 40)" << "\n";

    return 0;
}
