#pragma once
#include <iostream>
#include <utility>

// 平衡二分探索木 (Treap) による順序付き集合
template <typename T> class OrderStatisticSet {
private:
  // クラス内部に隠蔽した軽量乱数生成器 (Xorshift)
  class XorShift {
    uint64_t state;

  public:
    explicit XorShift(uint64_t seed = 88172645463325252ULL) : state(seed) {
      if (state == 0)
        state = 1;
    }
    uint32_t operator()() {
      state ^= state << 13;
      state ^= state >> 7;
      state ^= state << 17;
      return static_cast<uint32_t>(state);
    }
  };

  struct Node {
    T key;
    uint32_t priority;
    int size;
    Node *left = nullptr;
    Node *right = nullptr;

    Node(T k, uint32_t p) : key(std::move(k)), priority(p), size(1) {}
  };

  Node *root = nullptr;
  XorShift rng;

  // nullptr を安全に処理する内部用ヘルパー関数
  int get_size(Node *t) const { return t ? t->size : 0; }

  void update_size(Node *t) {
    if (t) {
      t->size = 1 + get_size(t->left) + get_size(t->right);
    }
  }

  void clear(Node *t) {
    if (!t)
      return;
    clear(t->left);
    clear(t->right);
    delete t;
  }

  Node *clone(Node *t) const {
    if (!t)
      return nullptr;
    Node *node = new Node(t->key, t->priority);
    node->size = t->size;
    node->left = clone(t->left);
    node->right = clone(t->right);
    return node;
  }

  std::pair<Node *, Node *> split_less(Node *t, const T &key) {
    if (!t)
      return {nullptr, nullptr};
    if (t->key < key) {
      auto [l, r] = split_less(t->right, key);
      t->right = l;
      update_size(t);
      return {t, r};
    } else {
      auto [l, r] = split_less(t->left, key);
      t->left = r;
      update_size(t);
      return {l, t};
    }
  }

  std::pair<Node *, Node *> split_less_equal(Node *t, const T &key) {
    if (!t)
      return {nullptr, nullptr};
    if (key < t->key) {
      auto [l, r] = split_less_equal(t->left, key);
      t->left = r;
      update_size(t);
      return {l, t};
    } else {
      auto [l, r] = split_less_equal(t->right, key);
      t->right = l;
      update_size(t);
      return {t, r};
    }
  }

  Node *merge(Node *l, Node *r) {
    if (!l || !r)
      return l ? l : r;
    if (l->priority > r->priority) {
      l->right = merge(l->right, r);
      update_size(l);
      return l;
    } else {
      r->left = merge(l, r->left);
      update_size(r);
      return r;
    }
  }

public:
  OrderStatisticSet() : rng() {}
  explicit OrderStatisticSet(uint64_t seed) : rng(seed) {}

  ~OrderStatisticSet() { clear(root); }

  OrderStatisticSet(const OrderStatisticSet &other) : rng(other.rng) {
    root = clone(other.root);
  }

  OrderStatisticSet &operator=(const OrderStatisticSet &other) {
    if (this != &other) {
      Node *new_root = clone(other.root);
      clear(root);
      root = new_root;
      rng = other.rng;
    }
    return *this;
  }

  OrderStatisticSet(OrderStatisticSet &&other) noexcept
      : root(other.root), rng(std::move(other.rng)) {
    other.root = nullptr;
  }

  OrderStatisticSet &operator=(OrderStatisticSet &&other) noexcept {
    if (this != &other) {
      clear(root);
      root = other.root;
      rng = std::move(other.rng);
      other.root = nullptr;
    }
    return *this;
  }

  // --- 公開インターフェース ---

  // 1. 全体の要素数
  int size() const { return get_size(root); }
  bool empty() const { return size() == 0; }

  // 2. 要素の追加・削除
  bool insert(T key) {
    auto [l, r] = split_less(root, key);
    auto [mid, r2] = split_less_equal(r, key);

    bool inserted = false;
    if (!mid) {
      try {
        mid = new Node(std::move(key), rng());
        inserted = true;
      } catch (...) {
        root = merge(l, r2);
        throw;
      }
    }
    root = merge(l, merge(mid, r2));
    return inserted;
  }

  bool erase(const T &key) {
    auto [l, r] = split_less(root, key);
    auto [mid, r2] = split_less_equal(r, key);

    bool erased = (mid != nullptr);
    clear(mid);
    root = merge(l, r2);
    return erased;
  }

  // 3. 値 x のインデックス (x未満の要素数)
  int get_index(const T &x) const {
    Node *curr = root;
    int index = 0;
    while (curr) {
      if (curr->key < x) {
        index += get_size(curr->left) + 1;
        curr = curr->right;
      } else {
        curr = curr->left;
      }
    }
    return index;
  }

  // 4. 下から k 番目の要素 (0-indexed)
  const T *get_from_bottom(int k) const {
    if (k < 0 || k >= size())
      return nullptr;
    Node *curr = root;
    while (curr) {
      int left_size = get_size(curr->left);
      if (k == left_size)
        return &(curr->key);
      if (k < left_size) {
        curr = curr->left;
      } else {
        k -= left_size + 1;
        curr = curr->right;
      }
    }
    return nullptr;
  }

  // 5. 上から k 番目の要素 (0-indexed)
  const T *get_from_top(int k) const {
    if (k < 0 || k >= size())
      return nullptr;
    return get_from_bottom(size() - 1 - k);
  }

  // 6. x 以上の最小の要素
  const T *lower_bound(const T &x) const {
    Node *curr = root;
    const T *res = nullptr;
    while (curr) {
      if (!(curr->key < x)) {
        res = &(curr->key);
        curr = curr->left;
      } else {
        curr = curr->right;
      }
    }
    return res;
  }

  // 7. x より大きい最小の要素
  const T *upper_bound(const T &x) const {
    Node *curr = root;
    const T *res = nullptr;
    while (curr) {
      if (x < curr->key) {
        res = &(curr->key);
        curr = curr->left;
      } else {
        curr = curr->right;
      }
    }
    return res;
  }
};

// エイリアス定義
template <typename T> using OSS = OrderStatisticSet<T>;
