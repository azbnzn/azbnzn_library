#pragma once
#include <bits/stdc++.h>

using namespace std;

// 1次元の座標圧縮 (引数と同じ型の vector を返す)
template <typename T = long long>
vector<T> compress(vector<T> vec) {
  vector<T> c = vec;
  sort(c.begin(), c.end());
  c.erase(unique(c.begin(), c.end()), c.end());
  for (auto &i : vec) {
    i = lower_bound(c.begin(), c.end(), i) - c.begin();
  }
  return vec;
}

// 座標圧縮の構造体 (復元や二分探索クエリも行える拡張版)
template <typename T = long long>
struct CoordinateCompression {
  vector<T> vals;

  CoordinateCompression() {}
  CoordinateCompression(const vector<T> &vec) : vals(vec) {
    build();
  }

  void add(const T &x) { vals.push_back(x); }

  void build() {
    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
  }

  // 圧縮後のインデックス (0-indexed)
  int operator()(const T &x) const {
    return lower_bound(vals.begin(), vals.end(), x) - vals.begin();
  }

  // インデックスから元の値を取得
  const T &operator[](int i) const { return vals[i]; }

  int size() const { return vals.size(); }
};
