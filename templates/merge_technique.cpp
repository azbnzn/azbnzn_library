#include <bits/stdc++.h>
using namespace std;
using ll = long long;
#define rep(i, n) for (ll i = 0; i < (ll)(n); i++)
#define pb push_back

/* =========================================================================
   [マージテク (Small to Large) - 直書き用テンプレート]
   ・変数名: g, par, u, v, i (3.cpp と完全一致)
   ・小さい集合を大きい集合へ swap してマージすることで全体 O(N log N) に収める。
   ========================================================================= */

// ==================== [コピペここから 1: クエリによる集合マージ (3.cppスタイル)] ====================
/*
// 初期化: 各頂点を独立した集合とし、代表元を自分自身に設定
vector<vector<int>> g(n);
vector<int> par(n);
rep(i, n) {
  g[i].pb(i);
  par[i] = i;
}

// 頂点 u と 頂点 v の属する集合をマージする処理 (Union-Find 的な操作)
u = par[u];
v = par[v];
if (u != v) {
  // サイズが小さい方 (v) を大きい方 (u) へマージ
  if (g[u].size() < g[v].size()) {
    swap(u, v);
  }
  for (auto &i : g[v]) {
    g[u].pb(i);
    par[i] = u;
  }
  g[v].clear();
}
*/
// ==================== [コピペここまで 1: 集合マージ] ====================


// ==================== [コピペここから 2: 木上の部分木マージ (DFS直書き)] ====================
/*
// 各頂点 u の部分木の要素 (色や値など) を葉から親へマージする
vector<vector<int>> tree(n);
vector<set<int>> st(n); // または vector<vector<int>> st(n);
vector<int> ans(n);

auto dfs = [&](auto self, int u, int p) -> void {
  st[u].insert(color[u]);

  for (int v : tree[u]) {
    if (v == p) continue;
    self(self, v, u);

    // 小さい方 (v) を大きい方 (u) へマージ
    if (st[u].size() < st[v].size()) {
      swap(st[u], st[v]);
    }
    for (auto &x : st[v]) {
      st[u].insert(x);
    }
    st[v].clear();
  }

  // 部分木 u に対する答えを記録 (例: 部分木内の色の種類数)
  ans[u] = st[u].size();
};
dfs(dfs, 0, -1);
*/
// ==================== [コピペここまで 2: 木上マージ] ====================


// --- 動作確認テスト (3.cpp と同等の動作) ---
int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n = 5;
  vector<vector<int>> g(n);
  vector<int> par(n);
  rep(i, n) {
    g[i].pb(i);
    par[i] = i;
  }

  // クエリ操作の例
  auto unite = [&](int u, int v) {
    u = par[u];
    v = par[v];
    if (u == v) return;

    if (g[u].size() < g[v].size()) {
      swap(u, v);
    }
    for (auto &i : g[v]) {
      g[u].pb(i);
      par[i] = u;
    }
    g[v].clear();
  };

  unite(0, 1);
  unite(2, 3);
  unite(1, 3);

  cout << "=== Merge Technique Test Output ===" << "\n";
  rep(i, n) {
    cout << "Node " << i << " leader = " << par[i] << "\n";
  }
  cout << "Group of 0 contains: ";
  for (int x : g[par[0]]) cout << x << " ";
  cout << "\n";

  return 0;
}
