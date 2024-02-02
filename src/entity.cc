module;

import types;
import sparse_set;
import component;

#include <unordered_map>

export module entity;

export using Entity = u32;

export struct EntityManager {

  using pool_type = basic_sparse_set<Entity>;

  Entity create_entity() {
    pool.try_emplace(e_count);
    e_count++;
    return e_count - 1;
  }

  template<typename T>
  void add_component(Entity e, T component) {
    return;
  }

  template<>
  void add_component(Entity e, CTransform t) {
    component_manager.transforms[e] = t;
    e_maps[0].push_back(e);
    return;
  }

  template<>
  void add_component(Entity e, CVelocity v) {
    component_manager.velocities[e] = v;
    e_maps[1].push_back(e);
    return;
  }

  template<typename T>
  std::vector<Entity> get_associated_entities() noexcept {
    if constexpr(std::is_same_v<T, CTransform>) {
      return e_maps[0];
    }
  }


  pool_type pool;
  usize e_count = 1;
  std::unordered_map<int, std::vector<Entity>> e_maps{};
};
