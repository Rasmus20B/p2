module;

#include <raylib.h>

#include <vector>
#include <variant>
#include <functional>
#include <print>

import types;

export module ecs.component;

export import ecs.components;

export template<typename T>
concept is_component = requires {
  requires get_component_id<T>() != ComponentID::Size;
};

export template<typename... C>
requires(is_component<C>, ...)
struct ComponentManager {
  ComponentManager() {
    std::apply([](auto&... c) {
        (c.resize(10000), ...);
        }, comps);
  }

  template<typename V>
  requires(is_component<V>)
  V& get(size_t idx) {
    return std::get<std::vector<V>>(comps)[idx]; 
  }

  template<typename V>
  requires(is_component<V>)
  std::vector<V>& get() {
    return std::get<std::vector<V>>(comps); 
  }
  std::tuple<std::vector<C>...> comps;
};

export inline ComponentManager<CTransform2D,
                               CVelocity,
                               CScript,
                               CHealth,
                               CSprite,
                               CInput,
                               CAttraction,
                               CCollider,
                               CBulletManager> component_manager{};

