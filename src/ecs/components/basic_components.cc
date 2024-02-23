module;

import types;
#include <raylib.h>
#include <functional>

export module ecs.components.basic;

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


export struct CSprite {

};

export struct CInput {

};

export struct CAttraction {
  Entity attractor;
  f32 gravity;
  Vector2 cache;
};
