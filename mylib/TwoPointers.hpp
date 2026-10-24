#pragma once
#include <bits/stdc++.h>

using namespace std;

// ============================================================================
// 尺取り法 (Two Pointers)
// ============================================================================
// ・単調性のある区間条件に対し、各左端 l (0 <= l < n) に対する極大な右端 r (半開区間 [l, r)) を O(N) で求める。
// ・l == r の追い越し防止や境界チェックを内部で自動処理するため、バグ（添字ミスや引き忘れ）を完全に防げる。
//
// 引数:
//   n       : 配列の長さ
//   can_add : [&](int r) -> bool   : 右端 r の要素を現在の区間 [l, r) に追加できるか判定
//   add     : [&](int r) -> void   : 右端 r の要素を区間に追加する処理
//   del     : [&](int l) -> void   : 左端 l の要素を区間から削除する処理
//   eval    : [&](int l, int r) -> void : 各 l に対する極大区間 [l, r) が確定したときの処理 (省略時は max_r 配列を返す)
// ============================================================================

template <typename CanAdd, typename Add, typename Del, typename Eval>
void two_pointers(int n, const CanAdd &can_add, const Add &add, const Del &del,
                  const Eval &eval) {
  int r = 0;
  for (int l = 0; l < n; l++) {
    while (r < n && can_add(r)) {
      add(r);
      r++;
    }
    eval(l, r);
    if (l == r) {
      r++;
    } else {
      del(l);
    }
  }
}

// eval を省略した場合、各 l に対する極大な右端 r (半開区間 [l, r)) の配列 vector<int> を返す
template <typename CanAdd, typename Add, typename Del>
vector<int> two_pointers(int n, const CanAdd &can_add, const Add &add,
                         const Del &del) {
  vector<int> max_r(n);
  two_pointers(
      n, can_add, add, del, [&](int l, int r) { max_r[l] = r; });
  return max_r;
}
