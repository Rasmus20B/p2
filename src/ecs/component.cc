module;

#include <raylib.h>

#include <vector>
#include <variant>
#include <functional>

import types;

export module ecs.component;

export enum class ComponentID {
  Transform2D,
  Velocity,
  Collider,
  BulletInfo,
  Health,
  Parent,
  Children,
  Interpreter,
  Input,
  Sprite,
  Attractor,
  Size,
};

export struct CTransform2D {
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
  std::vector<StackSlot> memory;
};

export struct CSprite {

};

export struct CInput {

};

export struct CAttraction {
  Entity attractor;
  f32 gravity;
};

export struct ComponentManager {
  ComponentManager() {
    transforms.resize(100000);
    velocities.resize(100000);
    colliders.resize(100000);
    health.resize(100000);
    sprites.resize(100000);
    vms.resize(100000);
    inputs.resize(100000);
    attractions.resize(100000);
  }
  std::vector<CTransform2D> transforms;
  std::vector<CVelocity> velocities;
  std::vector<CHealth> health;
  std::vector<CSprite> sprites;
  std::vector<CInterpreter> vms;
  std::vector<CInput> inputs;
  std::vector<CAttraction> attractions;
  std::vector<CCollider> colliders;
};

export inline ComponentManager component_manager{};
