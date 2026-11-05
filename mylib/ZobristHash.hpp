#pragma once
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

// ============================================================================
// 128ビット Zobrist Hash (ゾブリストハッシュ) ライブラリ
// ============================================================================
// 【主な機能】
// 1. Hash128: 64ビット×2の独立した乱数による強固な128ビットハッシュ値構造体
//    ・衝突確率: ~2^-128 (事実上衝突不可能)
//    ・加算 (+, -) による「多重集合 (multiset) ハッシュ」対応
//    ・排他的論理和 (^) による「集合 (set) ハッシュ / 奇偶判定」対応
// 2. ZobristHash: 静的配列に対する O(1) 区間多重集合一致判定・区間順列判定
//    ・same_multiset(l1, r1, l2, r2): 2つの区間の多重集合が同一か O(1) で判定 (ABC367F 等)
//    ・is_permutation(l, r, start): 区間が 1..L の順列か O(1) で判定
//    ・is_all_even(l, r): 区間内の全要素が偶数回出現するか O(1) で判定
// 3. DynamicZobristHash: 点更新 (1点変更) に O(log N) で対応する区間ハッシュ (Fenwick Tree基盤)
// 4. ZobristSet: 動的集合の状態ハッシュ (挿入・削除・反転 O(1))
// 5. ZobristMultiset: 動的多重集合の状態ハッシュ (挿入・削除 O(1))
// 6. ZobristGrid: 2次元盤面・ゲーム木探索用状態ハッシュ (マス更新・スワップ O(1))
// ============================================================================

namespace zobrist_internal {
  static inline uint64_t splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
  }

  // 実行時シード (クロック + アドレスランダム化によるハック攻撃防止)
  static inline uint64_t get_random_seed1() {
    static uint64_t seed = []() {
      uint64_t t = std::chrono::high_resolution_clock::now().time_since_epoch().count();
      uint64_t addr = reinterpret_cast<uint64_t>(&seed);
      return splitmix64(t ^ splitmix64(addr));
    }();
    return seed;
  }

  static inline uint64_t get_random_seed2() {
    static uint64_t seed = splitmix64(get_random_seed1() ^ 0xabcdef0123456789ULL);
    return seed;
  }
} // namespace zobrist_internal

// 128ビットハッシュ値構造体
struct Hash128 {
  uint64_t h1, h2;

  constexpr Hash128() : h1(0), h2(0) {}
  constexpr Hash128(uint64_t v1, uint64_t v2) : h1(v1), h2(v2) {}
  constexpr Hash128(uint64_t v) : h1(v), h2(zobrist_internal::splitmix64(v)) {}

  Hash128 operator^(const Hash128 &o) const { return {h1 ^ o.h1, h2 ^ o.h2}; }
  Hash128 operator+(const Hash128 &o) const { return {h1 + o.h1, h2 + o.h2}; }
  Hash128 operator-(const Hash128 &o) const { return {h1 - o.h1, h2 - o.h2}; }
  Hash128 operator*(uint64_t k) const { return {h1 * k, h2 * k}; }

  Hash128 &operator^=(const Hash128 &o) { h1 ^= o.h1; h2 ^= o.h2; return *this; }
  Hash128 &operator+=(const Hash128 &o) { h1 += o.h1; h2 += o.h2; return *this; }
  Hash128 &operator-=(const Hash128 &o) { h1 -= o.h1; h2 -= o.h2; return *this; }
  Hash128 &operator*=(uint64_t k) { h1 *= k; h2 *= k; return *this; }

  bool operator==(const Hash128 &o) const { return h1 == o.h1 && h2 == o.h2; }
  bool operator!=(const Hash128 &o) const { return !(*this == o); }
  bool operator<(const Hash128 &o) const { return h1 < o.h1 || (h1 == o.h1 && h2 < o.h2); }

  uint64_t get64() const { return h1; }
  explicit operator uint64_t() const { return h1; }
  bool is_zero() const { return h1 == 0 && h2 == 0; }
};

inline std::ostream &operator<<(std::ostream &os, const Hash128 &h) {
  return os << "Hash128(" << h.h1 << ", " << h.h2 << ")";
}

namespace std {
  template <>
  struct hash<Hash128> {
    size_t operator()(const Hash128 &h) const noexcept {
      return h.h1 ^ (h.h2 + 0x9e3779b97f4a7c15ULL + (h.h1 << 6) + (h.h1 >> 2));
    }
  };
} // namespace std

// 任意の値に対する擬似乱数ハッシュ値生成関数 (SplitMix64ベース)
template <typename T>
inline typename enable_if<is_integral<T>::value, Hash128>::type zobrist_hash_value(T x) {
  uint64_t u = static_cast<uint64_t>(x);
  uint64_t v1 = zobrist_internal::splitmix64(u ^ zobrist_internal::get_random_seed1());
  uint64_t v2 = zobrist_internal::splitmix64(u ^ zobrist_internal::get_random_seed2());
  return Hash128(v1, v2);
}

inline Hash128 zobrist_hash_value(const string &s) {
  uint64_t u1 = 0, u2 = 0;
  for (char c : s) {
    u1 = zobrist_internal::splitmix64(u1 ^ (uint8_t)c);
    u2 = zobrist_internal::splitmix64(u2 + (uint8_t)c);
  }
  return Hash128(u1, u2);
}

// 順列ハッシュの事前計算用テーブル
struct ZobristPermutationTable {
  vector<Hash128> pref_add;

  ZobristPermutationTable() {
    pref_add.push_back(Hash128(0, 0));
  }

  void extend(int n) {
    while ((int)pref_add.size() <= n) {
      int idx = pref_add.size();
      Hash128 next_h = pref_add.back() + zobrist_hash_value(idx);
      pref_add.push_back(next_h);
    }
  }

  Hash128 get(long long start, int len) {
    if (len <= 0) return Hash128(0, 0);
    if (start >= 1 && start + len - 1 <= 2000000) {
      extend(start + len - 1);
      return pref_add[start + len - 1] - pref_add[start - 1];
    }
    // start が 1..2000000 の範囲外の場合はループ計算
    Hash128 res(0, 0);
    for (int i = 0; i < len; ++i) {
      res += zobrist_hash_value(start + i);
    }
    return res;
  }
};

inline ZobristPermutationTable &get_global_perm_table() {
  static ZobristPermutationTable table;
  return table;
}

template <typename T> struct DynamicZobristHash;

// ============================================================================
// 1. ZobristHash (静的配列に対する区間ハッシュ・多重集合一致・順列判定)
// ============================================================================
struct ZobristHash {
  int n;
  vector<Hash128> pref_add; // 累積和 (多重集合用)
  vector<Hash128> pref_xor; // 累積XOR (奇偶判定・集合用)

  ZobristHash() : n(0) {}

  template <typename T>
  ZobristHash(const vector<T> &a) {
    build(a);
  }

  ZobristHash(const string &s) {
    vector<int> a(s.begin(), s.end());
    build(a);
  }

  template <typename T>
  void build(const vector<T> &a) {
    n = a.size();
    pref_add.assign(n + 1, Hash128(0, 0));
    pref_xor.assign(n + 1, Hash128(0, 0));
    for (int i = 0; i < n; ++i) {
      Hash128 h = zobrist_hash_value(a[i]);
      pref_add[i + 1] = pref_add[i] + h;
      pref_xor[i + 1] = pref_xor[i] ^ h;
    }
  }

  int size() const { return n; }

  // [l, r) の多重集合ハッシュ (O(1))
  Hash128 get_multiset_hash(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    return pref_add[r] - pref_add[l];
  }

  // [l, r) の XOR ハッシュ (O(1))
  Hash128 get_xor_hash(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    return pref_xor[r] ^ pref_xor[l];
  }

  // 同一配列内の [l1, r1) と [l2, r2) の多重集合が一致するか (O(1))
  bool same_multiset(int l1, int r1, int l2, int r2) const {
    if (r1 - l1 != r2 - l2) return false;
    return get_multiset_hash(l1, r1) == get_multiset_hash(l2, r2);
  }

  // 別配列 other の [l2, r2) との多重集合が一致するか (O(1))
  bool same_multiset(int l1, int r1, const ZobristHash &other, int l2, int r2) const {
    if (r1 - l1 != r2 - l2) return false;
    return get_multiset_hash(l1, r1) == other.get_multiset_hash(l2, r2);
  }

  // DynamicZobristHash との多重集合が一致するか (O(log N))
  template <typename U>
  bool same_multiset(int l1, int r1, const DynamicZobristHash<U> &other, int l2, int r2) const;

  // 同一配列内の [l1, r1) と [l2, r2) の XOR ハッシュが一致するか (O(1))
  bool same_xor(int l1, int r1, int l2, int r2) const {
    return get_xor_hash(l1, r1) == get_xor_hash(l2, r2);
  }

  // 別配列 other の [l2, r2) との XOR ハッシュが一致するか (O(1))
  bool same_xor(int l1, int r1, const ZobristHash &other, int l2, int r2) const {
    return get_xor_hash(l1, r1) == other.get_xor_hash(l2, r2);
  }

  // DynamicZobristHash との XOR ハッシュが一致するか (O(log N))
  template <typename U>
  bool same_xor(int l1, int r1, const DynamicZobristHash<U> &other, int l2, int r2) const;

  // [l, r) 内のすべての要素が偶数回出現するか (O(1))
  bool is_all_even(int l, int r) const {
    return get_xor_hash(l, r).is_zero();
  }

  // [l, r) が {start, start+1, ..., start+(r-l)-1} の順列になっているか (O(1))
  bool is_permutation(int l, int r, long long start = 1) const {
    int len = r - l;
    if (len <= 0) return true;
    Hash128 target = get_global_perm_table().get(start, len);
    return get_multiset_hash(l, r) == target;
  }
};

// ============================================================================
// 2. DynamicZobristHash (点更新クエリ対応の区間ハッシュ・Fenwick Tree基盤)
// ============================================================================
template <typename T = long long>
struct DynamicZobristHash {
  int n;
  vector<T> data;
  vector<Hash128> bit_add;
  vector<Hash128> bit_xor;

  DynamicZobristHash(int n, T default_val = T())
      : n(n), data(n, default_val), bit_add(n + 1, Hash128(0, 0)), bit_xor(n + 1, Hash128(0, 0)) {
    if (default_val != T()) {
      build(data);
    }
  }

  DynamicZobristHash(const vector<T> &a)
      : n(a.size()), data(a), bit_add(n + 1, Hash128(0, 0)), bit_xor(n + 1, Hash128(0, 0)) {
    build(a);
  }

  void build(const vector<T> &a) {
    data = a;
    for (int i = 0; i < n; ++i) {
      Hash128 h = zobrist_hash_value(a[i]);
      bit_add[i + 1] = h;
      bit_xor[i + 1] = h;
    }
    for (int i = 1; i <= n; ++i) {
      int p = i + (i & -i);
      if (p <= n) {
        bit_add[p] += bit_add[i];
        bit_xor[p] ^= bit_xor[i];
      }
    }
  }

  // a[idx] = new_val に更新 (O(log N))
  void set(int idx, T new_val) {
    assert(0 <= idx && idx < n);
    Hash128 old_h = zobrist_hash_value(data[idx]);
    Hash128 new_h = zobrist_hash_value(new_val);
    data[idx] = new_val;
    Hash128 diff_add = new_h - old_h;
    Hash128 diff_xor = new_h ^ old_h;
    for (int i = idx + 1; i <= n; i += i & -i) {
      bit_add[i] += diff_add;
      bit_xor[i] ^= diff_xor;
    }
  }

  T get(int idx) const {
    assert(0 <= idx && idx < n);
    return data[idx];
  }

  int size() const { return n; }

  // [l, r) の多重集合ハッシュ (O(log N))
  Hash128 get_multiset_hash(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    Hash128 res_r(0, 0);
    for (int i = r; i > 0; i -= i & -i) res_r += bit_add[i];
    Hash128 res_l(0, 0);
    for (int i = l; i > 0; i -= i & -i) res_l += bit_add[i];
    return res_r - res_l;
  }

  // [l, r) の XOR ハッシュ (O(log N))
  Hash128 get_xor_hash(int l, int r) const {
    assert(0 <= l && l <= r && r <= n);
    Hash128 res_r(0, 0);
    for (int i = r; i > 0; i -= i & -i) res_r ^= bit_xor[i];
    Hash128 res_l(0, 0);
    for (int i = l; i > 0; i -= i & -i) res_l ^= bit_xor[i];
    return res_r ^ res_l;
  }

  // 同一配列内の [l1, r1) と [l2, r2) の多重集合が一致するか (O(log N))
  bool same_multiset(int l1, int r1, int l2, int r2) const {
    if (r1 - l1 != r2 - l2) return false;
    return get_multiset_hash(l1, r1) == get_multiset_hash(l2, r2);
  }

  // 別配列 other の [l2, r2) との多重集合が一致するか (O(log N))
  bool same_multiset(int l1, int r1, const DynamicZobristHash<T> &other, int l2, int r2) const {
    if (r1 - l1 != r2 - l2) return false;
    return get_multiset_hash(l1, r1) == other.get_multiset_hash(l2, r2);
  }

  // 静的 ZobristHash other の [l2, r2) との多重集合が一致するか (O(log N))
  bool same_multiset(int l1, int r1, const ZobristHash &other, int l2, int r2) const {
    if (r1 - l1 != r2 - l2) return false;
    return get_multiset_hash(l1, r1) == other.get_multiset_hash(l2, r2);
  }

  // 同一配列内の [l1, r1) と [l2, r2) の XOR ハッシュが一致するか (O(log N))
  bool same_xor(int l1, int r1, int l2, int r2) const {
    return get_xor_hash(l1, r1) == get_xor_hash(l2, r2);
  }

  // 別配列 other の [l2, r2) との XOR ハッシュが一致するか (O(log N))
  bool same_xor(int l1, int r1, const DynamicZobristHash<T> &other, int l2, int r2) const {
    return get_xor_hash(l1, r1) == other.get_xor_hash(l2, r2);
  }

  // 静的 ZobristHash other の [l2, r2) との XOR ハッシュが一致するか (O(log N))
  bool same_xor(int l1, int r1, const ZobristHash &other, int l2, int r2) const {
    return get_xor_hash(l1, r1) == other.get_xor_hash(l2, r2);
  }

  // [l, r) 内のすべての要素が偶数回出現するか (O(log N))
  bool is_all_even(int l, int r) const {
    return get_xor_hash(l, r).is_zero();
  }

  // [l, r) が {start, start+1, ..., start+(r-l)-1} の順列になっているか (O(log N))
  bool is_permutation(int l, int r, long long start = 1) const {
    int len = r - l;
    if (len <= 0) return true;
    Hash128 target = get_global_perm_table().get(start, len);
    return get_multiset_hash(l, r) == target;
  }
};

template <typename U>
inline bool ZobristHash::same_multiset(int l1, int r1, const DynamicZobristHash<U> &other, int l2, int r2) const {
  if (r1 - l1 != r2 - l2) return false;
  return get_multiset_hash(l1, r1) == other.get_multiset_hash(l2, r2);
}

template <typename U>
inline bool ZobristHash::same_xor(int l1, int r1, const DynamicZobristHash<U> &other, int l2, int r2) const {
  return get_xor_hash(l1, r1) == other.get_xor_hash(l2, r2);
}

// ============================================================================
// 3. ZobristSet (動的集合の状態ハッシュ)
// ============================================================================
template <typename T = long long>
struct ZobristSet {
  unordered_set<T> st;
  Hash128 current_hash;

  ZobristSet() : current_hash(0, 0) {}

  // 要素 x を追加 (既に存在すれば false を返す) (O(1))
  bool insert(const T &x) {
    if (st.insert(x).second) {
      current_hash ^= zobrist_hash_value(x);
      return true;
    }
    return false;
  }

  // 要素 x を削除 (存在しなかったら false を返す) (O(1))
  bool erase(const T &x) {
    if (st.erase(x)) {
      current_hash ^= zobrist_hash_value(x);
      return true;
    }
    return false;
  }

  // 要素 x の有無を反転 (O(1))
  void toggle(const T &x) {
    if (st.count(x)) {
      st.erase(x);
    } else {
      st.insert(x);
    }
    current_hash ^= zobrist_hash_value(x);
  }

  bool contains(const T &x) const {
    return st.count(x);
  }

  int size() const {
    return st.size();
  }

  bool empty() const {
    return st.empty();
  }

  Hash128 get_hash() const {
    return current_hash;
  }

  uint64_t get64() const {
    return current_hash.get64();
  }

  void clear() {
    st.clear();
    current_hash = Hash128(0, 0);
  }
};

// ============================================================================
// 4. ZobristMultiset (動的多重集合の状態ハッシュ)
// ============================================================================
template <typename T = long long>
struct ZobristMultiset {
  unordered_map<T, long long> counts;
  Hash128 current_hash;
  long long total_count;

  ZobristMultiset() : current_hash(0, 0), total_count(0) {}

  // 要素 x を k 個追加 (O(1))
  void insert(const T &x, long long k = 1) {
    if (k <= 0) return;
    counts[x] += k;
    total_count += k;
    current_hash += zobrist_hash_value(x) * (uint64_t)k;
  }

  // 要素 x を最大 k 個削除 (O(1))
  bool erase(const T &x, long long k = 1) {
    if (k <= 0) return false;
    auto it = counts.find(x);
    if (it == counts.end() || it->second == 0) return false;
    long long actual = min(it->second, k);
    it->second -= actual;
    if (it->second == 0) counts.erase(it);
    total_count -= actual;
    current_hash -= zobrist_hash_value(x) * (uint64_t)actual;
    return true;
  }

  long long count(const T &x) const {
    auto it = counts.find(x);
    return it == counts.end() ? 0 : it->second;
  }

  long long size() const {
    return total_count;
  }

  bool empty() const {
    return total_count == 0;
  }

  Hash128 get_hash() const {
    return current_hash;
  }

  uint64_t get64() const {
    return current_hash.get64();
  }

  void clear() {
    counts.clear();
    current_hash = Hash128(0, 0);
    total_count = 0;
  }
};

// ============================================================================
// 5. ZobristGrid (2次元盤面・ゲーム木・グリッド探索用状態ハッシュ)
// ============================================================================
struct ZobristGrid {
  int h, w, num_states;
  vector<vector<int>> grid;
  vector<vector<vector<Hash128>>> table;
  Hash128 current_hash;

  ZobristGrid(int h, int w, int num_states = 2)
      : h(h), w(w), num_states(num_states), grid(h, vector<int>(w, 0)), current_hash(0, 0) {
    table.resize(h, vector<vector<Hash128>>(w, vector<Hash128>(num_states)));
    for (int r = 0; r < h; ++r) {
      for (int c = 0; c < w; ++c) {
        for (int v = 0; v < num_states; ++v) {
          uint64_t key = ((uint64_t)r << 32) ^ ((uint64_t)c << 16) ^ (uint64_t)v;
          table[r][c][v] = zobrist_hash_value(key);
        }
        current_hash ^= table[r][c][0];
      }
    }
  }

  // (r, c) の値を new_val に更新 (O(1))
  void set(int r, int c, int new_val) {
    assert(0 <= r && r < h && 0 <= c && c < w);
    assert(0 <= new_val && new_val < num_states);
    int old_val = grid[r][c];
    if (old_val == new_val) return;
    grid[r][c] = new_val;
    current_hash ^= table[r][c][old_val];
    current_hash ^= table[r][c][new_val];
  }

  // 2つのマス (r1, c1) と (r2, c2) の値をスワップ (15パズル等) (O(1))
  void swap_cells(int r1, int c1, int r2, int c2) {
    if (r1 == r2 && c1 == c2) return;
    int v1 = grid[r1][c1];
    int v2 = grid[r2][c2];
    set(r1, c1, v2);
    set(r2, c2, v1);
  }

  int get(int r, int c) const {
    assert(0 <= r && r < h && 0 <= c && c < w);
    return grid[r][c];
  }

  Hash128 get_hash() const {
    return current_hash;
  }

  uint64_t get64() const {
    return current_hash.get64();
  }
};
