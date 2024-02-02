module;

#include <concepts>
#include <vector>

import types;

export module sparse_set;

export template<typename T>
concept is_valid_ent_type = (std::is_same_v<T, u32> || std::is_same_v<T, u64>);

export template<typename Entity = std::uint32_t, typename Allocator = std::allocator<Entity>>
requires(is_valid_ent_type<Entity>)
struct basic_sparse_set {
  using sparse_container_t = std::vector<Entity, Allocator>;
  using dense_container_t = std::vector<Entity, Allocator>;

  [[nodiscard]] inline bool contains(const Entity e) {
    auto idx = sparse[e];
    return idx < head && dense[idx] == e;
  }

  void remove(const Entity e) {
    if(!contains(e)) {
      return;
    }
    head--;
    auto idx = sparse[e];
    dense[idx] = dense[head];
    sparse[dense[head]] = idx;
  }

  bool try_emplace(const Entity e) {
    if(sparse.size() < e) {
      sparse.resize(e + 1);
    }
    if(contains(e)) {
      return false;
    }
    sparse[e] = head;
    if(head + 1 >= dense.size()) {
      dense.resize(dense.size() + head + 1);
    }
    dense[head] = e;
    head++;
    return true;
  }

  sparse_container_t sparse;
  dense_container_t dense;
  Entity head = 0;
};
