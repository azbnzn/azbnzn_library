#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/* =========================================================================
   [マージテク (Small to Large)]
   1. 基本テク: 集合 (set, map) をマージする際、小さい方を大きい方にマージする。
      swap(a, b) は O(1) なので、全体で O(N log^2 N) に抑えられる。
   2. DSU on Tree (Sack): 木の部分木クエリをグローバル配列1つで O(N log N)
      で処理する超強力な木上マージテクニック。
   ========================================================================= */

// ==================== [コピペここから 1: 基本集合マージ] ====================
// 小さい set を大きい set へマージ (O(min(A, B) * log(max(A, B))))
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
// ==================== [コピペここまで 1] ====================


// ==================== [コピペここから 2: 木上のマージテク (DSU on Tree / Sack)] ====================
// 木の全部分木に対するクエリ (色の種類数、特定値の個数など) を O(N log N) で解く
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
                    heavy[u] = v; // 最もサイズの大きい子
                }
            }
            out[u] = timer;
        };
        timer = 0;
        dfs_size(dfs_size, root, -1);
    }

    // add(u): 頂点 u の情報を集計配列に追加
    // del(u): 頂点 u の情報を集計配列から削除
    // query(u): 部分木 u の答えを記録
    template <typename Add, typename Del, typename Query>
    void run(const Add& add, const Del& del, const Query& query, int root = 0) {
        auto dfs = [&](auto self, int u, int p, bool keep) -> void {
            // 1. 軽い子を探索 (計算後は状態を消去 keep = false)
            for (int v : g[u]) {
                if (v != p && v != heavy[u]) {
                    self(self, v, u, false);
                }
            }
            // 2. 最も重い子を探索 (計算結果を維持 keep = true)
            if (heavy[u] != -1) {
                self(self, heavy[u], u, true);
            }
            // 3. 自分自身と軽い子たちの部分木要素を追加
            add(u);
            for (int v : g[u]) {
                if (v != p && v != heavy[u]) {
                    for (int t = in[v]; t < out[v]; t++) {
                        add(node_by_in[t]);
                    }
                }
            }
            // 4. 部分木 u に対するクエリを処理
            query(u);

            // 5. 軽い子だった場合は状態を元に戻す (del)
            if (!keep) {
                for (int t = in[u]; t < out[u]; t++) {
                    del(node_by_in[t]);
                }
            }
        };

        dfs(dfs, root, -1, false);
    }
};
// ==================== [コピペここまで 2] ====================


// --- 使用例: 各頂点 u の部分木内に存在する「色の種類数」を求める ---
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 頂点数 7 の木
    //       0(c=1)
    //      /      \
    //    1(c=2)   2(c=1)
    //    /    \     \
    //  3(c=2) 4(c=3) 5(c=4)
    //                 \
    //                 6(c=4)
    int n = 7;
    vector<int> color = {1, 2, 1, 2, 3, 4, 4};

    DsuOnTree dot(n);
    vector<pair<int, int>> edges = {{0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}, {5, 6}};
    for (auto& e : edges) dot.add_edge(e.first, e.second);

    dot.build_tree(0);

    vector<int> ans(n, 0);
    vector<int> cnt(10, 0);
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

    cout << "=== DSU on Tree (マージテク) Test Output ===" << "\n";
    for (int i = 0; i < n; i++) {
        cout << "Subtree " << i << " distinct colors: " << ans[i] << "\n";
    }
    // Subtree 0: {1,2,3,4} -> 4種
    // Subtree 1: {2,3} -> 2種
    // Subtree 2: {1,4} -> 2種
    // Subtree 5: {4} -> 1種
    return 0;
}
