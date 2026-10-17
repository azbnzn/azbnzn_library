#include <bits/stdc++.h>
using namespace std;
using ll = long long;
#define rep(i, n) for (ll i = 0; i < (ll)(n); i++)
#define pb push_back

/* =========================================================================
   [01-BFS - 直書き用テンプレート]
   ・ダイクストラ (1.cpp) と同じ変数名・雰囲気 (todo, dist, c, t, w, i)
   ・重みが 0 と 1 のみの場合、deque を使って O(V + E) で高速に解く。
   ・コスト 0 は push_front、コスト 1 は push_back。
   ========================================================================= */

// ==================== [コピペここから: 通常のグラフ] ====================
/*
// グラフの用意 (重み w: 0 or 1, 行き先 i)
vector<vector<pair<ll, ll>>> g(n);
// rep(i, m) { ll u, v, w; cin >> u >> v >> w; g[u].pb({w, v}); g[v].pb({w, u}); }

// 01-BFS (始点 s)
deque<pair<ll, int>> todo;
todo.push_back({0, s});
vector<ll> dist(n, -1);

while (!todo.empty()) {
  auto [c, t] = todo.front();
  todo.pop_front();
  if (dist[t] != -1) {
    continue;
  }
  dist[t] = c;
  for (auto [w, i] : g[t]) {
    if (dist[i] == -1) {
      if (w == 0) {
        todo.push_front({c, i});
      } else {
        todo.push_back({c + 1, i});
      }
    }
  }
}
*/
// ==================== [コピペここまで: 通常のグラフ] ====================


// ==================== [コピペここから: 経路復元付き] ====================
/*
vector<int> prev_node(n, -1);
deque<pair<ll, int>> todo;
todo.push_back({0, s});
vector<ll> dist(n, -1);

while (!todo.empty()) {
  auto [c, t] = todo.front();
  todo.pop_front();
  if (dist[t] != -1) {
    continue;
  }
  dist[t] = c;
  for (auto [w, i] : g[t]) {
    if (dist[i] == -1) {
      prev_node[i] = t;
      if (w == 0) {
        todo.push_front({c, i});
      } else {
        todo.push_back({c + 1, i});
      }
    }
  }
}

// goal への経路復元 (s -> ... -> goal)
vector<int> path;
for (int cur = goal; cur != -1; cur = prev_node[cur]) path.pb(cur);
reverse(path.begin(), path.end());
*/
// ==================== [コピペここまで: 経路復元付き] ====================


// ==================== [コピペここから: グリッド版 (例: 壁破壊/ワープ)] ====================
/*
const vector<int> dx = {1, 0, -1, 0};
const vector<int> dy = {0, 1, 0, -1};

// {コスト, {x, y}}
deque<pair<ll, pair<int, int>>> todo;
todo.push_back({0, {sx, sy}});
vector<vector<ll>> dist(h, vector<ll>(w, -1));

while (!todo.empty()) {
  auto [c, pos] = todo.front();
  auto [x, y] = pos;
  todo.pop_front();
  if (dist[x][y] != -1) {
    continue;
  }
  dist[x][y] = c;

  rep(k, 4) {
    int nx = x + dx[k], ny = y + dy[k];
    if (nx < 0 || nx >= h || ny < 0 || ny >= w) continue;
    if (dist[nx][ny] == -1) {
      // 例: 通路 '.' ならコスト 0、壁 '#' を壊すならコスト 1
      if (grid[nx][ny] == '.') {
        todo.push_front({c, {nx, ny}});
      } else {
        todo.push_back({c + 1, {nx, ny}});
      }
    }
  }
}
*/
// ==================== [コピペここまで: グリッド版] ====================


// --- 動作確認テスト ---
int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n = 4;
  vector<vector<pair<ll, ll>>> g(n);
  // 0 -> 1 (w=1), 0 -> 2 (w=0), 2 -> 3 (w=0), 1 -> 3 (w=0)
  g[0].pb({1, 1});
  g[0].pb({0, 2});
  g[2].pb({0, 3});
  g[1].pb({0, 3});

  int s = 0;
  deque<pair<ll, int>> todo;
  todo.push_back({0, s});
  vector<ll> dist(n, -1);

  while (!todo.empty()) {
    auto [c, t] = todo.front();
    todo.pop_front();
    if (dist[t] != -1) {
      continue;
    }
    dist[t] = c;
    for (auto [w, i] : g[t]) {
      if (dist[i] == -1) {
        if (w == 0) {
          todo.push_front({c, i});
        } else {
          todo.push_back({c + 1, i});
        }
      }
    }
  }

  cout << "=== 01-BFS Test Output ===" << "\n";
  rep(i, n) {
    cout << "dist to " << i << " = " << dist[i] << "\n";
  }
  return 0;
}
