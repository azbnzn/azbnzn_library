#pragma once
#include <bits/stdc++.h>

using namespace std;

// Kahnのアルゴリズムによるトポロジカルソート
// g: 有向グラフの隣接リスト
// 戻り値: トポロジカル順序の頂点列 (閉路が存在しDAGでない場合は空の配列を返す)
vector<int> topological_sort(const vector<vector<int>> &g) {
  int n = g.size();
  vector<int> in_degree(n, 0);
  for (int i = 0; i < n; i++) {
    for (int v : g[i])
      in_degree[v]++;
  }

  queue<int> q;
  for (int i = 0; i < n; i++) {
    if (in_degree[i] == 0)
      q.push(i);
  }

  vector<int> res;
  res.reserve(n);
  while (!q.empty()) {
    int u = q.front();
    q.pop();
    res.push_back(u);
    for (int v : g[u]) {
      in_degree[v]--;
      if (in_degree[v] == 0)
        q.push(v);
    }
  }

  // 閉路が含まれている（DAGでない）場合は空の配列を返す
  if ((int)res.size() != n)
    return {};
  return res;
}
