module;

#include <raylib.h>

#include <vector>

import types;

export module component;

export struct CTransform {
  Vector2 position;
  Vector2 scale;
  f32 rotation;
};

export struct CInput {

};

export struct CVelocity {
  Vector2 velocity;
};

export struct CHealth {
  i32 health;
};

export struct CInterpreter {

};

export struct CSprite {

};

export struct ComponentManager {

  ComponentManager() {
    transforms.resize(10000);
    velocities.resize(10000);
    transforms.resize(10000);
    transforms.resize(10000);
    transforms.resize(10000);
  }
  std::vector<CTransform> transforms;
  std::vector<CVelocity> velocities;
  std::vector<CHealth> health;
  std::vector<CSprite> sprites;
  std::vector<CInterpreter> vms;
  std::vector<CInput> inputs;
};

export inline ComponentManager component_manager;
