module;

#include <vector>

#include <raylib.h>

import bullets;
import ecs;

#include <iostream>

export module player;

export struct Player {
  Vector2 pos{};
  Vector2 vel{};
};

export void handle_player(EntityManager& em, std::vector<BulletMgr>& bm) {
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
    BulletMgr l1;
    // l1.set_count(1, 1);
    // l1.set_mode(BM_EN_ATTACK);
    // l1.set_origin(component_manager.transforms[1].position.x,
    //     component_manager.transforms[1].position.y);
    // l1.set_angle(90);
    // l1.set_velocity(0, -1);
    // bm.push_back(l1);
    auto bl = em.create_entity();
    em.add_component<CTransform>(bl, {
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
