#pragma once
#include <bits/stdc++.h>

using namespace std;

// ============================================================================
// Mo's Algorithm (クエリ平方分割)
// ============================================================================
// ・オフライン区間クエリ [l, r) に対して、両端点を 1 つずつ伸縮させて状態を更新する。
// ・ブロック幅 B = max(1, N / sqrt(Q * 2 / 3)) 程度とし、偶奇ソートで定数倍高速化。
// ・計算量: O(N * sqrt(Q))
//
// 使用例:
//   MosAlgorithm mo(n);
//   for (auto [l, r] : queries) mo.add_query(l, r); // 半開区間 [l, r)
//
//   vector<int> ans(q);
//   auto add = [&](int idx) { ... };
//   auto del = [&](int idx) { ... };
//   auto out = [&](int q_id) { ans[q_id] = ...; };
//
//   mo.run(add, del, out);
// ============================================================================

struct MosAlgorithm {
  int n;
  struct Query {
    int l, r, id;
  };
  vector<Query> queries;
  int bucket_size;

  // n: 配列の長さ
  // bucket_size: 1ブロックの幅 (-1 の場合はクエリ数に応じて自動決定)
  MosAlgorithm(int n = 0, int bucket_size = -1)
      : n(n), bucket_size(bucket_size) {}

  // 半開区間 [l, r) のクエリを追加 (0-indexed)
  void add_query(int l, int r) {
    queries.push_back({l, r, (int)queries.size()});
  }

  // add(idx), del(idx), out(q_id) を渡して実行
  // add: インデックス idx の要素を区間に加える
  // del: インデックス idx の要素を区間から除外する
  // out: クエリ q_id の解答を出力・記録する
  template <typename Add, typename Del, typename Out>
  void run(const Add &add, const Del &del, const Out &out) {
    int q = queries.size();
    if (q == 0)
      return;

    int bs = bucket_size;
    if (bs <= 0) {
      bs = max(1, (int)(n / max(1.0, sqrt(q * 2.0 / 3.0))));
    }

    // 偶奇ソート (奇数ブロックは r を降順にして右端点の往復移動を削減)
    sort(queries.begin(), queries.end(), [&](const Query &a, const Query &b) {
      int ablock = a.l / bs, bblock = b.l / bs;
      if (ablock != bblock)
        return ablock < bblock;
      return (ablock & 1) ? (a.r > b.r) : (a.r < b.r);
    });

    int cur_l = 0, cur_r = 0;
    for (const auto &qry : queries) {
      while (cur_l > qry.l) {
        cur_l--;
        add(cur_l);
      }
      while (cur_r < qry.r) {
        add(cur_r);
        cur_r++;
      }
      while (cur_l < qry.l) {
        del(cur_l);
        cur_l++;
      }
      while (cur_r > qry.r) {
        cur_r--;
        del(cur_r);
      }
      out(qry.id);
    }
  }

  // 左右で端点追加・削除の挙動が異なる場合
  template <typename AddL, typename AddR, typename DelL, typename DelR,
            typename Out>
  void run(const AddL &add_l, const AddR &add_r, const DelL &del_l,
           const DelR &del_r, const Out &out) {
    int q = queries.size();
    if (q == 0)
      return;

    int bs = bucket_size;
    if (bs <= 0) {
      bs = max(1, (int)(n / max(1.0, sqrt(q * 2.0 / 3.0))));
    }

    sort(queries.begin(), queries.end(), [&](const Query &a, const Query &b) {
      int ablock = a.l / bs, bblock = b.l / bs;
      if (ablock != bblock)
        return ablock < bblock;
      return (ablock & 1) ? (a.r > b.r) : (a.r < b.r);
    });

    int cur_l = 0, cur_r = 0;
    for (const auto &qry : queries) {
      while (cur_l > qry.l) {
        cur_l--;
        add_l(cur_l);
      }
      while (cur_r < qry.r) {
        add_r(cur_r);
        cur_r++;
      }
      while (cur_l < qry.l) {
        del_l(cur_l);
        cur_l++;
      }
      while (cur_r > qry.r) {
        cur_r--;
        del_r(cur_r);
      }
      out(qry.id);
    }
  }
};

using Mo = MosAlgorithm;
