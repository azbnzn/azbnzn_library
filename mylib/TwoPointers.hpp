#pragma once
#include <algorithm>
#include <cassert>
#include <vector>

using namespace std;

// ============================================================================
// 尺取り法 (Two Pointers)
// ============================================================================
// ・単調性のある区間条件に対し、半開区間 [l, r) を O(N) で走査する 3 パターンを提供。
//
// 1. 左端 l 固定 (極大区間 [l, r) / 違反状態に入らない):
//    two_pointers(n, can_add, add, del, eval)
//    two_pointers(n, can_add, add, del) -> 各 l の極大右端 max_r[l]
//
// 2. 右端 r 固定 (極大区間 [l, r + 1) / 追加してから違反分を削る):
//    two_pointers_right(n, add, is_valid, del, eval)
//    two_pointers_right(n, add, is_valid, del) -> 各 r の最小左端 min_l[r]
//
// 3. 条件を満たす最短区間探索 (「和 >= K」等の最小長・全有効極小区間列挙):
//    two_pointers_min(n, add, is_valid, del, eval)
//    two_pointers_min_len(n, add, is_valid, del) -> 最短長 (なければ -1)
// ============================================================================

// --- パターン 1: 左端 l 固定ループ (極大な有効区間 [l, r)) ---
template <typename CanAdd, typename Add, typename Del, typename Eval>
inline void two_pointers(int n, const CanAdd &can_add, const Add &add,
                         const Del &del, const Eval &eval) {
  assert(n >= 0);
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

template <typename CanAdd, typename Add, typename Del>
inline vector<int> two_pointers(int n, const CanAdd &can_add, const Add &add,
                                const Del &del) {
  vector<int> max_r(n);
  two_pointers(n, can_add, add, del, [&](int l, int r) { max_r[l] = r; });
  return max_r;
}

// --- パターン 2: 右端 r 固定ループ (一旦 r を追加し、違反している間 l を進める) ---
// eval(l, r + 1): 半開区間 [l, r + 1) が条件を満たす最大の区間
template <typename Add, typename IsValid, typename Del, typename Eval>
inline void two_pointers_right(int n, const Add &add, const IsValid &is_valid,
                               const Del &del, const Eval &eval) {
  assert(n >= 0);
  int l = 0;
  for (int r = 0; r < n; r++) {
    add(r);
    while (l <= r && !is_valid()) {
      del(l);
      l++;
    }
    eval(l, r + 1);
  }
}

template <typename Add, typename IsValid, typename Del>
inline vector<int> two_pointers_right(int n, const Add &add,
                                      const IsValid &is_valid, const Del &del) {
  vector<int> min_l(n);
  two_pointers_right(n, add, is_valid, del,
                     [&](int l, int r) { min_l[r - 1] = l; });
  return min_l;
}

// --- パターン 3: 条件を満たす「最短」区間探索 ---
// r を追加し、is_valid() が true の間 eval(l, r + 1) を呼んで l を進める
template <typename Add, typename IsValid, typename Del, typename Eval>
inline void two_pointers_min(int n, const Add &add, const IsValid &is_valid,
                             const Del &del, const Eval &eval) {
  assert(n >= 0);
  int l = 0;
  for (int r = 0; r < n; r++) {
    add(r);
    while (l <= r && is_valid()) {
      eval(l, r + 1);
      del(l);
      l++;
    }
  }
}

template <typename Add, typename IsValid, typename Del>
inline int two_pointers_min_len(int n, const Add &add, const IsValid &is_valid,
                                const Del &del) {
  int best = n + 1;
  two_pointers_min(n, add, is_valid, del,
                   [&](int l, int r) { best = min(best, r - l); });
  return (best <= n) ? best : -1;
}
