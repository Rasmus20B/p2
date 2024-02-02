module;

import types;
import sparse_set;
import component;

#include <unordered_map>
#include <unordered_set>

export module entity;

export using Entity = u32;

export struct EntityManager {

  using pool_type = basic_sparse_set<Entity>;
  using map_type = std::unordered_map<int, std::unordered_set<Entity>>;

  Entity create_entity() {
    pool.try_emplace(e_count);
    e_count++;
    return e_count - 1;
  }

  template<typename T>
  void add_component(Entity e, T component) noexcept ;

  template<>
  void add_component(Entity e, CTransform t) noexcept {
    component_manager.transforms[e] = t;
    e_maps[0].insert(e);
    return;
  }

  template<>
  void add_component(Entity e, CVelocity v) noexcept {
    component_manager.velocities[e] = v;
    e_maps[1].insert(e);
    return;
  }

  template<>
  void add_component(Entity e, CInterpreter i) noexcept {
    component_manager.vms[e] = i;
    e_maps[2].insert(e);
    return;
  }

  template<>
  void add_component(Entity e, CHealth h) noexcept {
    component_manager.health[e] = h;
    e_maps[3].insert(e);
    return;
  }

  template<>
  void add_component(Entity e, CInput i) noexcept {
    component_manager.inputs[e] = i;
    e_maps[4].insert(e);
    return;
  }

  template<typename T>
  std::unordered_set<Entity> get_associated_entities() noexcept {
    if constexpr(std::is_same_v<T, CTransform>) {
      return e_maps[0];
    }
    else if (std::is_same_v<T, CVelocity>) {
      return e_maps[1];
    }
    else if (std::is_same_v<T, CInterpreter>) {
      return e_maps[2];
    }
    else if (std::is_same_v<T, CHealth>) {
      return e_maps[3];
    }
    return {};
  }

  pool_type pool;
  usize e_count = 1;
  map_type e_maps{};
};
