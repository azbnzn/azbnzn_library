#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

using namespace std;

// ポテンシャル付き Union-Find (Weighted Union-Find)
// 各頂点 x にポテンシャル V[x] を持たせ、辺 (x, y, w) で V[y] - V[x] = w の関係を管理する。
//
// Type: ポテンシャルの型 (通常 long long や int, double など)
template <typename Type = long long> struct WeightedUnionFind {
  int n;
  vector<int> parent_or_size;
  vector<Type> diff_weight; // 親ノードに対する相対ポテンシャル: V[x] - V[parent[x]]

  WeightedUnionFind() : n(0) {}
  explicit WeightedUnionFind(int n)
      : n(n), parent_or_size(n, -1), diff_weight(n, 0) {}

  // 代表元を返す (経路圧縮を行いながら親への重み差を累積更新)
  int root(int x) {
    assert(0 <= x && x < n);
    if (parent_or_size[x] < 0)
      return x;
    int r = root(parent_or_size[x]);
    diff_weight[x] += diff_weight[parent_or_size[x]];
    return parent_or_size[x] = r;
  }

  // 根からの相対ポテンシャル V[x] - V[root(x)] を取得
  Type weight(int x) {
    root(x);
    return diff_weight[x];
  }

  // x と y が同じ連結成分に属するか判定
  bool same(int x, int y) { return root(x) == root(y); }

  // V[y] - V[x] を取得 (同じ連結成分に属している必要がある)
  Type diff(int x, int y) {
    assert(same(x, y));
    return weight(y) - weight(x);
  }

  // V[y] - V[x] = w となるように x と y を結合する
  // 既に同じ連結成分に属している場合:
  // - 既存の diff(x, y) == w ならば true を返す (矛盾なし)
  // - 既存の diff(x, y) != w ならば false を返す (矛盾検出)
  // 新しく結合された場合は true を返す
  bool merge(int x, int y, Type w) {
    assert(0 <= x && x < n);
    assert(0 <= y && y < n);

    // root(x) と root(y) の差に換算:
    // w_root = V[root(y)] - V[root(x)] = weight(x) + w - weight(y)
    Type w_root = weight(x) + w - weight(y);
    int rx = root(x);
    int ry = root(y);

    if (rx == ry) {
      // 矛盾判定
      return diff(x, y) == w;
    }

    // サイズによる結合 (Union by size)
    if (-parent_or_size[rx] < -parent_or_size[ry]) {
      swap(rx, ry);
      w_root = -w_root;
    }

    parent_or_size[rx] += parent_or_size[ry];
    parent_or_size[ry] = rx;
    diff_weight[ry] = w_root;
    return true;
  }

  // x が属する連結成分のサイズ
  int size(int x) { return -parent_or_size[root(x)]; }

  // 全連結成分のグループ一覧
  vector<vector<int>> groups() {
    vector<int> leader_buf(n);
    vector<int> group_size(n, 0);
    for (int i = 0; i < n; i++) {
      leader_buf[i] = root(i);
      group_size[leader_buf[i]]++;
    }
    vector<vector<int>> res(n);
    for (int i = 0; i < n; i++) {
      res[i].reserve(group_size[i]);
    }
    for (int i = 0; i < n; i++) {
      res[leader_buf[i]].push_back(i);
    }
    res.erase(
        remove_if(res.begin(), res.end(),
                  [&](const vector<int> &v) { return v.empty(); }),
        res.end());
    return res;
  }
};
