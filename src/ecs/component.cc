module;

#include <raylib.h>

#include <vector>
#include <variant>
#include <functional>

import types;

export module ecs.component;

export enum class ComponentID {
  Transform,
  Velocity,
  Collider,
  BulletInfo,
  Health,
  Parent,
  Children,
  Interpreter,
  Input,
  Sprite,
  Size,
};

export struct CTransform {
  Vector2 position;
  Vector2 scale;
  f32 rotation;
};

export struct CVelocity {
  Vector2 velocity;
};

export struct CCollider {
  std::function<void(Entity self, Entity collided)> callback;
};

export struct CBulletInfo {

};

export struct CHealth {
  i32 health;
};

export struct CParent {
  Entity parent;
};

export struct CChildren {
  std::vector<Entity> children;
};

export struct CInterpreter {
  enum DataType {
    Integer,
    Float,
  };
  struct StackSlot {
    void print() {
      if(type == Integer) {
        i32 val = std::get<i32>(data);
      } else if(type == Float) {
        f32 val = std::get<f32>(data);
      }
    }
    std::variant<i32, f32> data;
    DataType type;
  };
  u16 pc = 0;
  u16 sp = 0;
  std::vector<u8> memory;
};

export struct CSprite {

};

export struct CInput {

};

export struct ComponentManager {

  ComponentManager() {
    transforms.resize(10000);
    velocities.resize(10000);
    health.resize(10000);
    sprites.resize(10000);
    vms.resize(10000);
    inputs.resize(10000);
  }
  std::vector<CTransform> transforms;
  std::vector<CVelocity> velocities;
  std::vector<CHealth> health;
  std::vector<CSprite> sprites;
  std::vector<CInterpreter> vms;
  std::vector<CInput> inputs;
};

export inline ComponentManager component_manager{};
