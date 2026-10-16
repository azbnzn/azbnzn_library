#pragma once
#include <bits/stdc++.h>

using namespace std;

// T: 扱う値の型 (long long や int など)
template <typename T = long long> struct MapTrie {
  struct Node {
    map<T, int> nex;    // 遷移先（mapなので自動でキーの昇順＝辞書順になる）
    vector<int> accept; // このノードを終端とする数列のIDを格納
    int pass = 0;       // このノードを通過した回数
  };

  vector<Node> nodes;
  vector<int> id_to_node; // 数列ID -> Trie木上のノードID のマッピング

  // コンストラクタ（最大ノード数を渡してMLE/TLEを防止）
  MapTrie(int max_nodes = 300005) {
    nodes.reserve(max_nodes);
    id_to_node.reserve(max_nodes);

    nodes.emplace_back();    // 根ノード (ノードID: 0)
    id_to_node.push_back(0); // 初期状態 (数列ID: 0) は根ノードにいる
  }

  // 【操作1】既存の数列(parent_id)の末尾に値(val)を追加し、新しい数列(new_id)とする
  void append(int parent_id, T val, int new_id) {
    assert(0 <= parent_id && parent_id < (int)id_to_node.size());
    int cur = id_to_node[parent_id];
    assert(cur != -1);

    // 遷移先がなければ新しくノードを作る
    if (!nodes[cur].nex.count(val)) {
      int next_id = nodes.size();
      nodes[cur].nex[val] = next_id;
      nodes.emplace_back();
    }

    int nex_node = nodes[cur].nex[val];
    nodes[nex_node].pass++;
    nodes[nex_node].accept.push_back(new_id); // 添え字の小さい順に追加される

    // id_to_node のサイズを自動拡張して記録
    if (new_id >= (int)id_to_node.size()) {
      id_to_node.resize(new_id + 1, -1);
    }
    id_to_node[new_id] = nex_node;
  }

  // 【操作2】配列を丸ごと追加する（普通のTrie木としての使い方）
  void insert(const vector<T> &seq, int seq_id) {
    int cur = 0;
    nodes[0].pass++;
    for (const auto &val : seq) {
      if (!nodes[cur].nex.count(val)) {
        int next_id = nodes.size();
        nodes[cur].nex[val] = next_id;
        nodes.emplace_back();
      }
      cur = nodes[cur].nex[val];
      nodes[cur].pass++;
    }
    nodes[cur].accept.push_back(seq_id);
    if (seq_id >= (int)id_to_node.size()) {
      id_to_node.resize(seq_id + 1, -1);
    }
    id_to_node[seq_id] = cur;
  }

  // 内部用のDFS関数（辞書順に数列IDを回収）
  void dfs(int cur, vector<int> &res) const {
    for (int seq_id : nodes[cur].accept) {
      res.push_back(seq_id);
    }
    for (const auto &[val, nex_node] : nodes[cur].nex) {
      dfs(nex_node, res);
    }
  }

  // 辞書順に並べた数列のIDリストを取得
  vector<int> get_lexicographical_order() const {
    vector<int> res;
    res.reserve(nodes.size());
    dfs(0, res);
    return res;
  }
};

// 小文字アルファベット 'a'-'z' 用の Trie木 / Aho-Corasick 多相オートマトン
struct Trie {
  struct Node {
    vector<int> next;
    int fail = 0;
    bool isOk = true; // パターン終端でない(禁止文字列を含まない)フラグ
    int count = 0;   // このノードを終端とするパターンの個数
    Node() {
      next.assign(26, -1);
    }
  };

  vector<Node> nodes;
  int root = 0;

  Trie() { nodes.emplace_back(); }

  // 文字列パターンの追加
  void add(const string &s) {
    int id = 0;
    for (char c : s) {
      assert('a' <= c && c <= 'z');
      int idx = c - 'a';
      if (nodes[id].next[idx] == -1) {
        int next_id = nodes.size();
        nodes[id].next[idx] = next_id;
        nodes.emplace_back();
      }
      id = nodes[id].next[idx];
    }
    nodes[id].isOk = false;
    nodes[id].count++;
  }

  // Aho-Corasick automaton の構築 (fail辺と遷移関数の補完)
  void build() {
    queue<int> que;
    for (int i = 0; i < 26; i++) {
      if (nodes[0].next[i] != -1) {
        que.push(nodes[0].next[i]);
      } else {
        nodes[0].next[i] = 0;
      }
    }

    while (!que.empty()) {
      int t = que.front();
      que.pop();

      for (int i = 0; i < 26; i++) {
        if (nodes[t].next[i] != -1) {
          int v = nodes[t].next[i];
          nodes[v].fail = nodes[nodes[t].fail].next[i];
          nodes[v].isOk &= nodes[nodes[v].fail].isOk;
          nodes[v].count += nodes[nodes[v].fail].count;
          que.push(v);
        } else {
          nodes[t].next[i] = nodes[nodes[t].fail].next[i];
        }
      }
    }
  }

  // 文字列 text 内に登録されたいずれかのパターンが含まれているか判定
  bool contains_any(const string &text) const {
    int now = 0;
    for (char c : text) {
      if (c < 'a' || c > 'z') {
        now = 0; // 小文字アルファベット以外（空白や記号など）はスキップして根に戻る
        continue;
      }
      now = nodes[now].next[c - 'a'];
      if (!nodes[now].isOk)
        return true;
    }
    return false;
  }
};
