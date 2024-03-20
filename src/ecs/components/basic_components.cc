module;

import types;
#include <raylib.h>
#include <functional>

export module ecs.components.basic;

export namespace ecs {
  namespace component {
    struct CTransform2D {
      Vector2 position;
      Vector2 scale;
      f32 rotation;
    };

    struct CVelocity {
      Vector2 velocity;
    };

    struct CCollider {
      std::function<void(Entity self, Entity collided)> callback;
    };

    struct CHealth {
      i32 health;
    };

    struct CParent {
      Entity parent;
    };

    struct CChildren {
      std::vector<Entity> children;
    };

    struct CSprite {
      std::shared_ptr<Texture2D> sprite;
    };

    struct CInput {};

    struct CAttraction {
      Entity attractor;
      f32 gravity;
      Vector2 cache;
    };
  }
}

