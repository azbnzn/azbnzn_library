#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

// 辺の定義
template <typename Cost = long long> struct Edge {
  int src, to;
  Cost cost;
  Edge(int s, int t, Cost c = 1) : src(s), to(t), cost(c) {}
  operator int() const { return to; }
};

// グラフクラス
template <typename Cost = long long> struct Graph : vector<vector<Edge<Cost>>> {
  Graph(int n = 0) : vector<vector<Edge<Cost>>>(n) {}

  // 有向辺の追加 (片方向)
  void add_edge(int s, int t, Cost c = 1) {
    (*this)[s].emplace_back(s, t, c);
  }

  // 無向辺の追加 (両方向)
  void add_undirected_edge(int s, int t, Cost c = 1) {
    (*this)[s].emplace_back(s, t, c);
    (*this)[t].emplace_back(t, s, c);
  }
};

// =========================================================================
// 1. 全方位木DP (6.cpp 互換スタイル)
// =========================================================================
// Cost: 辺の重みの型 (通常 long long や int)
// Data: DPで保持する値の型
// merge: 2つの部分木の結果をマージする関数 (Data, Data) -> Data
// e: マージの単位元を返す関数 () -> Data
// leaf: 葉ノードの初期値を返す関数 () -> Data
// apply: 子部分木の値 memo[d] に辺 (d -> c, cost) を適用して親 c に渡す値を返す関数
//        (Data val, int from, int to, Cost cost) -> Data
template <typename Cost, typename Data,
          Data (*merge)(Data, Data),
          Data (*e)(),
          Data (*leaf)(),
          Data (*apply)(Data, int, int, Cost)>
struct Rerooting : Graph<Cost> {
  vector<Data> dp;   // 各頂点を根としたときの木全体の答え
  vector<Data> memo; // dfs1 での部分木の答え

  Rerooting(int n = 0) : Graph<Cost>(n) {}

  // 全方位木DPを実行し、各頂点を根としたときの答えの配列を返す
  vector<Data> run(int root = 0) {
    int n = this->size();
    if (n == 0)
      return {};
    if (n == 1) {
      dp.assign(1, leaf());
      return dp;
    }
    memo.assign(n, e());
    dp.assign(n, e());
    dfs1(root, -1);
    dfs2(root, -1, e());
    return dp;
  }

  // 互換用エイリアス
  vector<Data> solve(int root = 0) { return run(root); }

private:
  void dfs1(int c, int p) {
    bool upd = false;
    for (Edge<Cost> &d : (*this)[c]) {
      if (d == p)
        continue;
      dfs1(d, c);
      upd = true;
      memo[c] = merge(memo[c], apply(memo[d], d, c, d.cost));
    }
    if (!upd)
      memo[c] = leaf();
  }

  void dfs2(int c, int p, const Data &val) {
    vector<Data> ds{val};
    for (Edge<Cost> &d : (*this)[c]) {
      if (d == p)
        continue;
      ds.push_back(apply(memo[d], d, c, d.cost));
    }
    int n = ds.size(), idx = 1;
    vector<Data> head(n + 1, e()), tail(n + 1, e());
    for (int i = 0; i++ < n;)
      head[i] = merge(head[i - 1], ds[i - 1]);
    for (int i = n; i-- > 0;)
      tail[i] = merge(ds[i], tail[i + 1]);
    dp[c] = head[n];
    for (Edge<Cost> &d : (*this)[c]) {
      if (d == p)
        continue;
      Data sub = merge(head[idx], tail[idx + 1]);
      dfs2(d, c, apply(sub, c, d, d.cost));
      idx++;
    }
  }
};

// =========================================================================
// 2. 汎用全方位木DP (頂点作用 add_node 対応版)
// =========================================================================
// 頂点属性（頂点重み、頂点自身を1つカウントするなど）がある問題でも
// 葉や根を特別扱いすることなく自然かつ正確に解くことができるモデルです。
//
// merge: (Data, Data) -> Data (子同士のマージ)
// e: () -> Data (マージの単位元)
// apply_edge: (Data, int from, int to, Cost cost) -> Data (辺の伝搬)
// add_node: (Data, int v) -> Data (頂点 v の値を反映する。葉は add_node(e(), v) になる)
namespace rerooting_internal {
  template <typename Data>
  inline Data default_add_node(Data d, int) { return d; }
}

template <typename Cost, typename Data,
          Data (*merge)(Data, Data),
          Data (*e)(),
          Data (*apply_edge)(Data, int, int, Cost),
          Data (*add_node)(Data, int) = rerooting_internal::default_add_node<Data>>
struct RerootingTree : Graph<Cost> {
  vector<Data> dp;     // 各頂点を根としたときの木全体の答え
  vector<Data> sub_dp; // dfs1 における部分木の答え

  RerootingTree(int n = 0) : Graph<Cost>(n) {}

  vector<Data> run(int root = 0) {
    int n = this->size();
    if (n == 0) return {};
    if (n == 1) {
      dp.assign(1, add_node(e(), 0));
      return dp;
    }
    dp.assign(n, e());
    sub_dp.assign(n, e());
    dfs1(root, -1);
    dfs2(root, -1, e());
    return dp;
  }

  vector<Data> solve(int root = 0) { return run(root); }

private:
  void dfs1(int c, int p) {
    Data res = e();
    for (Edge<Cost> &d : (*this)[c]) {
      if (d == p) continue;
      dfs1(d, c);
      res = merge(res, apply_edge(sub_dp[d], d, c, d.cost));
    }
    sub_dp[c] = add_node(res, c);
  }

  void dfs2(int c, int p, const Data &val_from_parent) {
    int deg = (*this)[c].size();
    vector<Data> ds;
    ds.reserve(deg);

    for (Edge<Cost> &d : (*this)[c]) {
      if (d == p) {
        ds.push_back(val_from_parent);
      } else {
        ds.push_back(apply_edge(sub_dp[d], d, c, d.cost));
      }
    }

    int sz = ds.size();
    vector<Data> head(sz + 1, e()), tail(sz + 1, e());
    for (int i = 0; i < sz; i++) {
      head[i + 1] = merge(head[i], ds[i]);
    }
    for (int i = sz - 1; i >= 0; i--) {
      tail[i] = merge(ds[i], tail[i + 1]);
    }

    dp[c] = add_node(head[sz], c);

    for (int i = 0; i < sz; i++) {
      Edge<Cost> &d = (*this)[c][i];
      if (d == p) continue;
      Data without_d = merge(head[i], tail[i + 1]);
      Data to_child = apply_edge(add_node(without_d, c), c, d, d.cost);
      dfs2(d, c, to_child);
    }
  }
};
