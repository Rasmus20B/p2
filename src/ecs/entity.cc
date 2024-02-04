module;

import types;
import sparse_set;
import ecs.component;

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <iostream>
#include <queue>
#include <ranges>

export module ecs.entity;

export inline std::queue<Entity> reuse;
export inline std::queue<Entity> deads;

export struct EntityManager {

  using pool_type = basic_sparse_set<Entity>;
  using map_type = std::unordered_map<int, basic_sparse_set<Entity>>;

  Entity create_entity() {
    if(!reuse.empty()) {
      auto id = reuse.front();
      reuse.pop();
      pool.try_emplace(id);
      e_count++;
      return id;
    } else {
      pool.try_emplace(e_cur);
      e_count++;
      e_cur++;
      return e_cur - 1;
    }
  }

  void delete_entity(Entity e) {
    for(auto i : std::views::iota(0, std::to_underlying(ComponentID::Size) -1)) {
      e_maps[i].remove(e);
    }
    pool.remove(e);
    e_count--;
  }

  template<typename ...T>
  void add_components(Entity e, T&&... components) {
    (add_component(e, (components)),...);
  }

  template<typename T>
  void add_component(Entity e, T component);

  template<>
  void add_component(Entity e, CTransform t)  {
    component_manager.transforms[e] = t;
    e_maps[std::to_underlying(ComponentID::Transform)].try_emplace(e);
    return;
  }

  template<>
  void add_component(Entity e, CVelocity v)  {
    component_manager.velocities[e] = v;
    e_maps[std::to_underlying(ComponentID::Velocity)].try_emplace(e);
    return;
  }

  template<>
  void add_component(Entity e, CInterpreter i)  {
    component_manager.vms[e] = i;
    e_maps[std::to_underlying(ComponentID::Interpreter)].try_emplace(e);
    return;
  }

  template<>
  void add_component(Entity e, CHealth h)  {
    component_manager.health[e] = h;
    e_maps[std::to_underlying(ComponentID::Health)].try_emplace(e);
    return;
  }

  template<>
  void add_component(Entity e, CInput i)  {
    component_manager.inputs[e] = i;
    e_maps[std::to_underlying(ComponentID::Input)].try_emplace(e);
    return;
  }

  template<typename T>
  std::vector<Entity> get_associated_entities() noexcept {
    if constexpr(std::is_same_v<T, CTransform>) {
      return std::vector<Entity>(e_maps[0].begin(), e_maps[0].end());
    }
    else if constexpr(std::is_same_v<T, CVelocity>) {
      return std::vector<Entity>(e_maps[1].begin(), e_maps[1].end());
    }
    else if constexpr (std::is_same_v<T, CInterpreter>) {
      return std::vector<Entity>(e_maps[2].begin(), e_maps[2].end());
    }
    else if constexpr (std::is_same_v<T, CHealth>) {
      return std::vector<Entity>(e_maps[3].begin(), e_maps[3].end());
    }
    else if constexpr (std::is_same_v<T, CInput>) {
      return std::vector<Entity>(e_maps[4].begin(), e_maps[4].end());
    }
    return {};
  }

  pool_type pool;
  usize e_count = 1;
  usize e_cur = 1;
  map_type e_maps{};
};

