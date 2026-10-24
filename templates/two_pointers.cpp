#include <bits/stdc++.h>
using namespace std;
using ll = long long;
#define rep(i, n) for (ll i = 0; i < (ll)(n); i++)
template <class T> inline bool chmin(T &a, T b) {
  if (a > b) { a = b; return true; }
  return false;
}
template <class T> inline bool chmax(T &a, T b) {
  if (a < b) { a = b; return true; }
  return false;
}

/* =========================================================================
   [尺取り法 (Two Pointers) - 直書き用テンプレート]
   ・単調性を利用して、区間 [l, r) の両端を O(N) で移動させる。
   ・用途に合わせて以下の 3 パターンを使い分ける：
     1. 左端 l ループ (半開区間 [l, r)): 汎用性最高。違反状態に一瞬でも入れない問題に最適
     2. 右端 r ループ (閉区間 [l, r]): 一旦追加してから違反を解消できる問題に最適 (l==r の分岐不要)
     3. 最短区間を求めるパターン (条件「和 >= K」などを満たす最小長)
   ========================================================================= */

// ==================== [コピペここから 1: 左端 l 固定ループ (半開区間 [l, r) 基本形)] ====================
/*
ll r = 0;
ll sum = 0; // 区間 [l, r) の状態 (合計・種類数・setなど)
ll ans = 0;

rep(l, n) {
  // r を 1 つ進めても条件を満たす限り r を伸ばす
  while (r < n && sum + a[r] <= k) {
    sum += a[r];
    r++;
  }

  // この時点で [l, r) は l を左端とする極大な有効区間 (長さ r - l)
  ans += (r - l);       // 条件を満たす区間の個数を数える場合
  // chmax(ans, r - l); // 条件を満たす最長の区間長を求める場合

  // 次の l へ進むための後処理
  if (l == r) {
    r++; // 区間が空 [l, l) で 1 つも進めなかった場合は r も一緒に進める
  } else {
    sum -= a[l]; // a[l] を区間から外す
  }
}
*/
// ==================== [コピペここまで 1: 左端 l 固定ループ] ====================


// ==================== [コピペここから 2: 右端 r 固定ループ (一旦追加して縮める形)] ====================
/*
// 「まず a[r] を追加し、条件違反している間 l を進めて削る」ができる場合に最も簡潔な書き方
ll l = 0;
ll sum = 0;
ll ans = 0;

rep(r, n) {
  sum += a[r]; // 右端 a[r] を追加

  // 条件に違反している間、左端 l を進めて要素を削る
  while (l <= r && sum > k) {
    sum -= a[l];
    l++;
  }

  // この時点で閉区間 [l, r] (長さ r - l + 1) は条件を満たす極大区間
  ans += (r - l + 1);       // 条件を満たす区間の個数
  // chmax(ans, r - l + 1); // 条件を満たす最大長
}
*/
// ==================== [コピペここまで 2: 右端 r 固定ループ] ====================


// ==================== [コピペここから 3: 条件を満たす「最短」区間を求める形] ====================
/*
// 「区間和 >= k」などの条件を満たす最小の区間長を求める
ll l = 0;
ll sum = 0;
ll ans = (ll)2e18; // INF

rep(r, n) {
  sum += a[r]; // 右端 a[r] を追加

  // 条件を満たしている間、記録を更新しつつ左端 l を詰める
  while (l <= r && sum >= k) {
    chmin(ans, r - l + 1);
    sum -= a[l];
    l++;
  }
}
// (ans が INF のままなら条件を満たす区間は存在しない)
*/
// ==================== [コピペここまで 3: 最短区間] ====================


// --- 動作確認テスト ---
int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  ll n = 6, k = 10;
  vector<ll> a = {3, 1, 4, 1, 5, 9};

  // パターン 1: 和が k (10) 以下の区間数と最大長 (左端 l ループ)
  ll r = 0, sum = 0, cnt1 = 0, max_len1 = 0;
  rep(l, n) {
    while (r < n && sum + a[r] <= k) {
      sum += a[r];
      r++;
    }
    cnt1 += (r - l);
    chmax(max_len1, r - l);
    if (l == r) {
      r++;
    } else {
      sum -= a[l];
    }
  }

  // パターン 2: 和が k (10) 以下の区間数と最大長 (右端 r ループ)
  ll l = 0, cnt2 = 0, max_len2 = 0;
  sum = 0;
  rep(r_idx, n) {
    sum += a[r_idx];
    while (l <= r_idx && sum > k) {
      sum -= a[l];
      l++;
    }
    cnt2 += (r_idx - l + 1);
    chmax(max_len2, r_idx - l + 1);
  }

  // パターン 3: 和が k (10) 以上の最短区間長
  l = 0;
  sum = 0;
  ll min_len = (ll)2e18;
  rep(r_idx, n) {
    sum += a[r_idx];
    while (l <= r_idx && sum >= k) {
      chmin(min_len, r_idx - l + 1);
      sum -= a[l];
      l++;
    }
  }

  cout << "=== Two Pointers Test Output ===" << "\n";
  cout << "Count (sum <= 10) [Pattern 1]: " << cnt1 << "\n";
  cout << "Count (sum <= 10) [Pattern 2]: " << cnt2 << "\n";
  cout << "Max length (sum <= 10): " << max_len1 << " (Expected: 4 for [3,1,4,1] or [1,4,1])" << "\n";
  cout << "Min length (sum >= 10): " << min_len << " (Expected: 2 for [5,9])" << "\n";

  assert(cnt1 == cnt2);
  assert(max_len1 == 4 && max_len2 == 4);
  assert(min_len == 2);

  return 0;
}
