#pragma once
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

// 動的文字列平衡二分木 (Dynamic String Treap)
// 文字列に対する任意位置への文字挿入・削除・更新・取得、
// および区間内の異なる文字種数 (アルファベット小文字 'a'-'z') のカウントを O(log N) で処理する。
struct DynamicStringTreap {
  struct Node {
    char val;
    uint32_t priority;
    int size;
    int mask; // 出現文字のビットマスク (1 << (c - 'a'))
    int l, r; // 0 を null 扱いとするインデックス
  };

  vector<Node> tree;
  int root = 0;

  static inline uint32_t xor128() {
    static uint32_t x = 123456789, y = 362436069, z = 521288629,
                    w = 88675123;
    uint32_t t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
  }

  int new_node(char c) {
    int idx = tree.size();
    int mask = 0;
    if (c >= 'a' && c <= 'z') {
      mask = 1 << (c - 'a');
    }
    tree.push_back({c, xor128(), 1, mask, 0, 0});
    return idx;
  }

  void update(int t) {
    if (!t)
      return;
    tree[t].size = 1 + tree[tree[t].l].size + tree[tree[t].r].size;
    int m = 0;
    if (tree[t].val >= 'a' && tree[t].val <= 'z') {
      m = 1 << (tree[t].val - 'a');
    }
    tree[t].mask = m | tree[tree[t].l].mask | tree[tree[t].r].mask;
  }

  // [0, k) と [k, n) に分割
  void split(int t, int k, int &l, int &r) {
    if (!t) {
      l = r = 0;
      return;
    }
    int left_sz = tree[tree[t].l].size;
    if (k <= left_sz) {
      split(tree[t].l, k, l, tree[t].l);
      r = t;
      update(r);
    } else {
      split(tree[t].r, k - left_sz - 1, tree[t].r, r);
      l = t;
      update(l);
    }
  }

  // l と r を結合
  void merge(int &t, int l, int r) {
    if (!l || !r) {
      t = l ? l : r;
      return;
    }
    if (tree[l].priority > tree[r].priority) {
      merge(tree[l].r, tree[l].r, r);
      t = l;
    } else {
      merge(tree[r].l, l, tree[r].l);
      t = r;
    }
    update(t);
  }

  DynamicStringTreap() {
    clear();
  }

  explicit DynamicStringTreap(const string &s) {
    clear();
    tree.reserve(s.size() + 1);
    for (char c : s) {
      merge(root, root, new_node(c));
    }
  }

  void clear() {
    tree.clear();
    // 0 番ノードは null (ダミー) として確保
    tree.push_back({'\0', 0, 0, 0, 0, 0});
    root = 0;
  }

  int size() const { return tree[root].size; }
  bool empty() const { return size() == 0; }

  // i 番目の文字の直後に c を挿入 (i=0 のときは先頭に挿入) (O(log N))
  void insert(int i, char c) {
    assert(0 <= i && i <= size());
    int l = 0, r = 0;
    split(root, i, l, r);
    merge(l, l, new_node(c));
    merge(root, l, r);
  }

  // 末尾に文字 c を追加 (O(log N))
  void push_back(char c) { insert(size(), c); }

  // i 文字目を c に変更 (0-indexed) (O(log N))
  void set(int i, char c) {
    assert(0 <= i && i < size());
    int l = 0, m = 0, r = 0;
    split(root, i, l, r);
    split(r, 1, m, r);
    tree[m].val = c;
    update(m);
    merge(r, m, r);
    merge(root, l, r);
  }

  // i 文字目を取得 (0-indexed) (O(log N))
  char get(int i) {
    assert(0 <= i && i < size());
    int l = 0, m = 0, r = 0;
    split(root, i, l, r);
    split(r, 1, m, r);
    char res = tree[m].val;
    merge(r, m, r);
    merge(root, l, r);
    return res;
  }

  // i 文字目を削除 (0-indexed) (O(log N))
  void erase(int i) {
    assert(0 <= i && i < size());
    int l = 0, m = 0, r = 0;
    split(root, i, l, r);
    split(r, 1, m, r);
    // m (i文字目) はマージせずに破棄
    merge(root, l, r);
  }

  // 区間 [l, r) の文字種類数を取得 (0-indexed, 半開区間) (O(log N))
  int count_distinct(int l, int r) {
    if (l >= r || !root)
      return 0;
    assert(0 <= l && r <= size());
    int left_part = 0, mid = 0, right_part = 0;
    split(root, r, left_part, right_part);
    split(left_part, l, left_part, mid);

    int res = __builtin_popcount(tree[mid].mask);

    merge(left_part, left_part, mid);
    merge(root, left_part, right_part);
    return res;
  }

  // 現在の文字列全体を取得 (O(N))
  string to_string() {
    string res;
    res.reserve(size());
    dfs_dump(root, res);
    return res;
  }

  // 部分文字列 [l, r) を取得 (O(log N + (r - l)))
  string substr(int l, int r) {
    if (l >= r || !root)
      return "";
    assert(0 <= l && r <= size());
    int left_part = 0, mid = 0, right_part = 0;
    split(root, r, left_part, right_part);
    split(left_part, l, left_part, mid);

    string res;
    res.reserve(r - l);
    dfs_dump(mid, res);

    merge(left_part, left_part, mid);
    merge(root, left_part, right_part);
    return res;
  }

private:
  void dfs_dump(int t, string &res) const {
    if (!t)
      return;
    dfs_dump(tree[t].l, res);
    res += tree[t].val;
    dfs_dump(tree[t].r, res);
  }
};
