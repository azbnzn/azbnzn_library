# 01-BFS

辺の重みが **0 または 1 のみ** のグラフにおいて、`std::deque` を用いることで $O(V + E)$ で最短路を求めるアルゴリズムです。  
ダイクストラ（`1.cpp`）と**全く同じ変数名・同じ雰囲気**で直書きできるように設計されています。

---

## 1. 概要・特徴
- **計算量**: $O(V + E)$ （ダイクストラの $\log$ が消えて超高速）
- **変数名ルール**（ダイクストラ `1.cpp` と統一）:
  - `todo`: 両端キュー (`std::deque`)
  - `dist`: 距離配列（未到達は `-1`、確定時に代入）
  - `auto [c, t] = todo.front();`: `c` はコスト、`t` は対象頂点
  - `g[t]` の各要素 `[w, i]`: `w` は重み (0 or 1)、`i` は遷移先頂点
  - 遷移時: **`w == 0` なら `push_front`、`w == 1` なら `push_back`**

---

## 2. コピペ用コード (直書き)

### 通常のグラフ
```cpp
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
```

### 経路復元付き
```cpp
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
```

### グリッド版 (例: 壁破壊/ワープ)
```cpp
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
      // 例: 通路 '.' はコスト 0、壁 '#' の破壊はコスト 1
      if (grid[nx][ny] == '.') {
        todo.push_front({c, {nx, ny}});
      } else {
        todo.push_back({c + 1, {nx, ny}});
      }
    }
  }
}
```

---

## 3. 注意点
- **コストが 0 または 1 のみであること**:
  - コストが 2 以上の辺が含まれるとキュー内の単調性が崩れるため、ダイクストラ法を使用してください。
- **ダイクストラとの使い分け**:
  - 重みが 0/1 のときは、ダイクストラの `priority_queue` を `deque` に、`top` を `front` に、`push` を `push_front`/`push_back` に変えるだけでそのまま動きます。
