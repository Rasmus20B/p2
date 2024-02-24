module;

#include <raylib.h>

#include <vector>
#include <variant>
#include <functional>
#include <print>

import types;

export module ecs.component;

export import ecs.components;

export enum class ComponentID {
  Transform2D,
  Velocity,
  Collider,
  BulletInfo,
  Health,
  Parent,
  Children,
  Script,
  Input,
  Sprite,
  Attractor,
  BulletManager,
  BulletPattern,
  Size,
};

export struct ComponentManager {
  ComponentManager() {
    transforms.resize(100000);
    velocities.resize(100000);
    health.resize(100000);
    sprites.resize(100000);
    inputs.resize(100000);
    attractions.resize(100000);
    colliders.resize(100000);
    scripts.resize(100000);
    bullets.resize(100000);
  }
  std::vector<CTransform2D> transforms;
  std::vector<CVelocity> velocities;
  std::vector<CHealth> health;
  std::vector<CSprite> sprites;
  std::vector<CInput> inputs;
  std::vector<CAttraction> attractions;
  std::vector<CCollider> colliders;
  std::vector<CScript> scripts;
  std::vector<CBulletManager> bullets;
};

export inline ComponentManager component_manager{};

export template<typename T>
consteval ComponentID get_component_id() {
  if constexpr(std::is_same_v<T, CTransform2D>) {
    return ComponentID::Transform2D;
  }
  else if constexpr(std::is_same_v<T, CVelocity>) {
    return ComponentID::Velocity;
  }
  else if constexpr (std::is_same_v<T, CScript>) {
    return ComponentID::Script;
  }
  else if constexpr (std::is_same_v<T, CHealth>) {
    return ComponentID::Health;
  }
  else if constexpr (std::is_same_v<T, CSprite>) {
    return ComponentID::Sprite;
  }
  else if constexpr (std::is_same_v<T, CInput>) {
    return ComponentID::Input;
  }
  else if constexpr (std::is_same_v<T, CAttraction>) {
    return ComponentID::Attractor;
  }
};
