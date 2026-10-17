#include <bits/stdc++.h>
using namespace std;
using ll = long long;
#define rep(i, n) for (ll i = 0; i < (ll)(n); i++)
#define pb push_back

/* =========================================================================
   [ダイクストラ法 (Dijkstra) - 直書き用テンプレート]
   ・変数名: todo, dist, c, t, w, i (1.cpp と完全一致)
   ・未訪問判定: dist[t] == -1
   ・計算量: O((V + E) log V)
   ========================================================================= */

// ==================== [コピペここから: 通常のグラフ] ====================
/*
// グラフの用意 (重み w, 行き先 i)
vector<vector<pair<ll, int>>> g(n);
// rep(i, m) { ll u, v, w; cin >> u >> v >> w; g[u].pb({w, v}); g[v].pb({w, u});
}

// ダイクストラ法 (始点 s)
priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>>
todo; todo.push({0, s}); vector<ll> dist(n, -1);

while (!todo.empty()) {
  auto [c, t] = todo.top();
  todo.pop();
  if (dist[t] != -1) {
    continue;
  }
  dist[t] = c;
  for (auto [w, i] : g[t]) {
    if (dist[i] == -1) {
      todo.push({c + w, i});
    }
  }
}
*/
// ==================== [コピペここまで: 通常のグラフ] ====================

// ==================== [コピペここから: 経路復元付き] ====================
/*
vector<int> prev_node(n, -1);
priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>>
todo; todo.push({0, s}); vector<ll> dist(n, -1);

while (!todo.empty()) {
  auto [c, t] = todo.top();
  todo.pop();
  if (dist[t] != -1) {
    continue;
  }
  dist[t] = c;
  for (auto [w, i] : g[t]) {
    if (dist[i] == -1) {
      prev_node[i] = t;
      todo.push({c + w, i});
    }
  }
}

// goal への経路復元 (s -> ... -> goal)
vector<int> path;
for (int cur = goal; cur != -1; cur = prev_node[cur]) path.pb(cur);
reverse(path.begin(), path.end());
*/
// ==================== [コピペここまで: 経路復元付き] ====================

// ==================== [コピペここから: グリッド版] ====================
/*
const vector<int> dx = {1, 0, -1, 0};
const vector<int> dy = {0, 1, 0, -1};

// {コスト, {x, y}}
priority_queue<pair<ll, pair<int, int>>, vector<pair<ll, pair<int, int>>>,
greater<pair<ll, pair<int, int>>>> todo; todo.push({0, {sx, sy}});
vector<vector<ll>> dist(h, vector<ll>(w, -1));

while (!todo.empty()) {
  auto [c, pos] = todo.top();
  auto [x, y] = pos;
  todo.pop();
  if (dist[x][y] != -1) {
    continue;
  }
  dist[x][y] = c;

  rep(k, 4) {
    int nx = x + dx[k], ny = y + dy[k];
    if (nx < 0 || nx >= h || ny < 0 || ny >= w) continue;
    // if (grid[nx][ny] == '#') continue; // 壁判定
    if (dist[nx][ny] == -1) {
      ll cost = 1; // 移動コスト
      todo.push({c + cost, {nx, ny}});
    }
  }
}
*/
// ==================== [コピペここまで: グリッド版] ====================

// --- 動作確認テスト ---
int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n = 5;
  vector<vector<pair<ll, int>>> g(n);
  // 0 -> 1 (cost 10), 0 -> 2 (cost 3), 2 -> 1 (cost 2), 1 -> 3 (cost 1), 3 -> 4
  // (cost 7)
  g[0].pb({10, 1});
  g[0].pb({3, 2});
  g[2].pb({2, 1});
  g[1].pb({1, 3});
  g[3].pb({7, 4});

  int s = 0;
  priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>>
      todo;
  todo.push({0, s});
  vector<ll> dist(n, -1);

  while (!todo.empty()) {
    auto [c, t] = todo.top();
    todo.pop();
    if (dist[t] != -1) {
      continue;
    }
    dist[t] = c;
    for (auto [w, i] : g[t]) {
      if (dist[i] == -1) {
        todo.push({c + w, i});
      }
    }
  }

  cout << "=== Dijkstra Test Output ===" << "\n";
  rep(i, n) { cout << "dist to " << i << " = " << dist[i] << "\n"; }
  return 0;
}
