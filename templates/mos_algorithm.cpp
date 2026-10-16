#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/* =========================================================================
   [Mo's Algorithm (平方分割によるオフライン区間クエリ)]
   ・区間クエリ [l, r) に対して、両端点を 1 つずつ伸縮させて状態を更新する。
   ・ブロック幅 B = N / min(N, sqrt(Q)) 程度にし、偶奇ソートで定数倍高速化。
   ・計算量: O(N * sqrt(Q))
   ========================================================================= */

// ==================== [コピペここから] ====================
struct Mo {
    int n;
    struct Query {
        int l, r, id;
    };
    vector<Query> queries;

    Mo(int n) : n(n) {}

    // 半開区間 [l, r) を追加 (0-indexed)
    void add_query(int l, int r) {
        queries.push_back({l, r, (int)queries.size()});
    }

    // add(idx), del(idx), get_ans() を渡して実行
    // 各関数はラムダ式等で渡す
    template <typename Add, typename Del, typename Out>
    void run(const Add& add, const Del& del, const Out& out) {
        int q = queries.size();
        if (q == 0) return;

        int bs = max(1, (int)(n / max(1.0, sqrt(q * 2.0 / 3.0))));
        
        // 偶奇ソート (奇数ブロックは r を降順にして右端点の折り返し移動を削減)
        sort(queries.begin(), queries.end(), [&](const Query& a, const Query& b) {
            int ablock = a.l / bs, bblock = b.l / bs;
            if (ablock != bblock) return ablock < bblock;
            return (ablock & 1) ? (a.r > b.r) : (a.r < b.r);
        });

        int cur_l = 0, cur_r = 0;
        for (const auto& qry : queries) {
            while (cur_l > qry.l) { cur_l--; add(cur_l); }
            while (cur_r < qry.r) { add(cur_r); cur_r++; }
            while (cur_l < qry.l) { del(cur_l); cur_l++; }
            while (cur_r > qry.r) { cur_r--; del(cur_r); }
            out(qry.id);
        }
    }

    // 左右で操作が異なる場合用 (add_l, add_r, del_l, del_r)
    template <typename AddL, typename AddR, typename DelL, typename DelR, typename Out>
    void run_lr(const AddL& add_l, const AddR& add_r,
                const DelL& del_l, const DelR& del_r, const Out& out) {
        int q = queries.size();
        if (q == 0) return;

        int bs = max(1, (int)(n / max(1.0, sqrt(q * 2.0 / 3.0))));
        sort(queries.begin(), queries.end(), [&](const Query& a, const Query& b) {
            int ablock = a.l / bs, bblock = b.l / bs;
            if (ablock != bblock) return ablock < bblock;
            return (ablock & 1) ? (a.r > b.r) : (a.r < b.r);
        });

        int cur_l = 0, cur_r = 0;
        for (const auto& qry : queries) {
            while (cur_l > qry.l) { cur_l--; add_l(cur_l); }
            while (cur_r < qry.r) { add_r(cur_r); cur_r++; }
            while (cur_l < qry.l) { del_l(cur_l); cur_l++; }
            while (cur_r > qry.r) { cur_r--; del_r(cur_r); }
            out(qry.id);
        }
    }
};
// ==================== [コピペここまで] ====================

// --- 使用例: 区間内の相異なる値の個数を答えるクエリ ---
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n = 8, q = 4;
    vector<int> a = {1, 2, 1, 3, 2, 4, 1, 2};
    // クエリ: [0, 4) -> {1,2,1,3} -> 3種
    //        [2, 6) -> {1,3,2,4} -> 4種
    //        [1, 3) -> {2,1}     -> 2種
    //        [4, 8) -> {2,4,1,2} -> 3種
    vector<pair<int, int>> qs = {{0, 4}, {2, 6}, {1, 3}, {4, 8}};

    Mo mo(n);
    for (auto& p : qs) mo.add_query(p.first, p.second);

    vector<int> ans(q);
    vector<int> cnt(10, 0);
    int distinct_count = 0;

    auto add = [&](int idx) {
        if (cnt[a[idx]] == 0) distinct_count++;
        cnt[a[idx]]++;
    };

    auto del = [&](int idx) {
        cnt[a[idx]]--;
        if (cnt[a[idx]] == 0) distinct_count--;
    };

    auto out = [&](int q_id) {
        ans[q_id] = distinct_count;
    };

    mo.run(add, del, out);

    cout << "=== Mo's Algorithm Test Output ===" << "\n";
    for (int i = 0; i < q; i++) {
        cout << "Query " << i << " [" << qs[i].first << ", " << qs[i].second << "): " << ans[i] << "\n";
    }
    return 0;
}
