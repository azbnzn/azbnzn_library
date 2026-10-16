#pragma once
#include <algorithm>
#include <numeric>
#include <vector>

// 素集合データ構造 (Union-Find / Disjoint Set Union)
struct UnionFind {
  std::vector<int> parent_or_size;

  UnionFind() : parent_or_size(0) {}
  UnionFind(int n) : parent_or_size(n, -1) {}

  int root(int x) {
    if (parent_or_size[x] < 0)
      return x;
    return parent_or_size[x] = root(parent_or_size[x]);
  }

  // x と y を結合する (併合されたら true, 既に同連結成分なら false)
  bool merge(int x, int y) {
    int rx = root(x), ry = root(y);
    if (rx == ry)
      return false;
    if (-parent_or_size[rx] < -parent_or_size[ry])
      std::swap(rx, ry);
    parent_or_size[rx] += parent_or_size[ry];
    parent_or_size[ry] = rx;
    return true;
  }

  // x と y が同じ連結成分に属するか判定
  bool same(int x, int y) { return root(x) == root(y); }

  // x が属する連結成分のサイズを取得
  int size(int x) { return -parent_or_size[root(x)]; }

  // 全連結成分のグループ一覧を取得 (ACL dsu::groups() 互換)
  std::vector<std::vector<int>> groups() {
    int n = parent_or_size.size();
    std::vector<std::vector<int>> result;
    std::vector<int> leader_buf(n);
    std::vector<int> group_size(n, 0);
    for (int i = 0; i < n; i++) {
      leader_buf[i] = root(i);
      group_size[leader_buf[i]]++;
    }
    std::vector<std::vector<int>> res(n);
    for (int i = 0; i < n; i++) {
      res[i].reserve(group_size[i]);
    }
    for (int i = 0; i < n; i++) {
      res[leader_buf[i]].push_back(i);
    }
    res.erase(
        std::remove_if(res.begin(), res.end(),
                       [&](const std::vector<int> &v) { return v.empty(); }),
        res.end());
    return res;
  }
};
