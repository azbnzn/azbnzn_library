#pragma once
#include <bits/stdc++.h>

using namespace std;

// ダイクストラ法 (単一始点最短経路)
// g: 隣接リスト g[from] = {{to, cost}, ...}
// start_pos: 始点
// 戻り値: 各頂点への最短距離 (到達不可は -1)
vector<long long> dijkstra(const vector<vector<pair<int, long long>>> &g,
                           int start_pos) {
  int n = g.size();
  vector<long long> dist(n, -1);
  // {コスト, 頂点} の昇順優先度付きキュー (コストは long long)
  using P = pair<long long, int>;
  priority_queue<P, vector<P>, greater<P>> todo;

  todo.push({0, start_pos});
  while (!todo.empty()) {
    auto [c, p] = todo.top();
    todo.pop();
    if (dist[p] != -1) {
      continue;
    }
    dist[p] = c;
    for (const auto &edge : g[p]) {
      int to = edge.first;
      long long cost = edge.second;
      if (dist[to] == -1) {
        todo.push({c + cost, to});
      }
    }
  }
  return dist;
}

// スペルミス互換用エイリアス
inline vector<long long> dijksta(const vector<vector<pair<int, long long>>> &g,
                                 int start_pos) {
  return dijkstra(g, start_pos);
}
