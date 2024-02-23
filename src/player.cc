module;

#include <vector>

#include <raylib.h>

import ecs;

#include <iostream>

export module player;

export struct Player {
  Vector2 pos{};
  Vector2 vel{};
};

export void handle_player(EntityManager& em) {
  // Player Movement
  if (IsKeyDown(KEY_UP)) {
    component_manager.velocities[1].velocity.y = -0.5f;
  } else if (IsKeyDown(KEY_DOWN)) {
    component_manager.velocities[1].velocity.y = 0.5f;
  } else {
    component_manager.velocities[1].velocity.y = 0;
  }
  if (IsKeyDown(KEY_LEFT)) {
    component_manager.velocities[1].velocity.x = -0.5f;
  } else if (IsKeyDown(KEY_RIGHT)) {
    component_manager.velocities[1].velocity.x = 0.5f;
  } else {
    component_manager.velocities[1].velocity.x = 0;
  }

  // Player Shooting
  if (IsKeyDown(KEY_Z)) {
    auto bl = em.create_entity();
    em.add_component<CTransform2D>(bl, {
        { 
        component_manager.transforms[1].position.x,
        component_manager.transforms[1].position.y 
        },
        {
        20, 40
        },
        90});
    em.add_component<CVelocity>(bl, {
        { 
        0, -0.5
        }});
  }
}
