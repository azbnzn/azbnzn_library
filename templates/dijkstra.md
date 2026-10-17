# ダイクストラ法 (Dijkstra)

辺の重みが非負であるグラフにおいて、単一始点から他のすべての頂点への最短距離を求めるアルゴリズムです。  
構造体を作らず、`solve()` / `main()` 内にそのまま貼り付けて使える直書きテンプレートです。

---

## 1. 概要・特徴
- **計算量**: $O((V + E) \log V)$
- **変数名ルール**（`1.cpp` スタイル）:
  - `todo`: 優先度付きキュー (`priority_queue`)
  - `dist`: 距離配列（未到達は `-1`、確定時に代入）
  - `auto [c, t] = todo.top();`: `c` は現在の最短コスト、`t` は対象頂点
  - `g[t]` の各要素 `[w, i]`: `w` は移動重み、`i` は遷移先頂点

---

## 2. コピペ用コード (直書き)

### 通常のグラフ
```cpp
// グラフの用意 (重み w, 行き先 i)
vector<vector<pair<ll, ll>>> g(n);
// rep(i, m) { ll u, v, w; cin >> u >> v >> w; g[u].pb({w, v}); g[v].pb({w, u}); }

// ダイクストラ法 (始点 s)
priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> todo;
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
```

### 経路復元付き
```cpp
vector<int> prev_node(n, -1);
priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> todo;
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
      prev_node[i] = t;
      todo.push({c + w, i});
    }
  }
}

// goal への経路復元 (s -> ... -> goal)
vector<int> path;
for (int cur = goal; cur != -1; cur = prev_node[cur]) path.pb(cur);
reverse(path.begin(), path.end());
```

### グリッド版
```cpp
const vector<int> dx = {1, 0, -1, 0};
const vector<int> dy = {0, 1, 0, -1};

// {コスト, {x, y}}
priority_queue<pair<ll, pair<int, int>>, vector<pair<ll, pair<int, int>>>, greater<pair<ll, pair<int, int>>>> todo;
todo.push({0, {sx, sy}});
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
      ll cost = 1; // マスへの進入コスト
      todo.push({c + cost, {nx, ny}});
    }
  }
}
```

---

## 3. 注意点
- **`dist[t] != -1` による重複スキップ**:
  - キューから取り出した時点で `dist[t] != -1` であれば、すでに最短距離が確定済みのため `continue` します。
- **重みの型**:
  - コストの加算でオーバーフローを防ぐため、`c` や `dist` は `ll` (`long long`) を推奨します。
