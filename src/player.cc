module;

#include <vector>

#include <raylib.h>

import bullets;

export module player;

export struct Player {
  Vector2 pos{};
  Vector2 vel{};
};

export void handle_player(Player& p, std::vector<BulletMgr>& bm) {
  // Player Movement
  if (IsKeyDown(KEY_UP)) {
    p.vel.y = -0.5f;
  } else if (IsKeyDown(KEY_DOWN)) {
    p.vel.y = 0.5f;
  } else {
    p.vel.y = 0;
  }
  if (IsKeyDown(KEY_LEFT)) {
    p.vel.x = -0.5f;
  } else if (IsKeyDown(KEY_RIGHT)) {
    p.vel.x = 0.5f;
  } else {
    p.vel.x = 0;
  }
  p.pos.x += p.vel.x;
  p.pos.y += p.vel.y;

  // Player Shooting
  if (IsKeyDown(KEY_Z)) {
    BulletMgr l1;
    l1.set_count(1, 1);
    l1.set_mode(BM_EN_ATTACK);
    l1.set_origin(p.pos.x, p.pos.y);
    l1.set_angle(90);
    l1.set_velocity(0, -1);
    bm.push_back(l1);
  }

}
