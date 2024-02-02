module;

#include <thread>
#include <system_error>
#include <vector>
#include <ratio>
#include <chrono>

#include <raylib.h>

import bullets;
import player;
import event;
import component;
import ring_buffer;
import net;
import entity;
import system;

export module p2;

constinit float GAME_LOGIC_SPEED = 1.f;
constinit float SLICE = 1.f;
float dt = 0.f;;

struct World {
  EntityManager em;
  std::vector<BulletMgr> live_bullets{};
  Player pl;
};

void render(World &w) {
  BeginDrawing();
  ClearBackground(BLACK);

  // Draw Live Bullets
  for(auto b: w.live_bullets) {
    for(auto e : b.positions) {
      DrawRectangle(e.x, e.y, 50, 50, RED);
    }
  }

  for(auto i: w.em.get_associated_entities<CTransform>()) {
    DrawRectangle(component_manager.transforms[i].position.x, 
      component_manager.transforms[i].position.y, 
      component_manager.transforms[i].scale.x, 
      component_manager.transforms[i].scale.y, RED);
  }
  DrawFPS(200, 200);
  EndDrawing();
}

void tick(World &w) {

  handle_player(w.live_bullets);
  // Bullet movement
  for(auto &b: w.live_bullets) {
    for(auto &p : b.positions) {
      p.x += b.velocity.x ;
      p.y += b.velocity.y ;
    }
  }
  moveTransformAll();
}

export void gameloop() {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  SetTargetFPS(60);
  const Vector2 screenDim = {
    .x = (float)GetMonitorWidth(0),
    .y = (float)GetMonitorHeight(0)
  };
  InitWindow(screenDim.x, screenDim.y, "p2");

  World world;
  // BulletMgr l1;
  // l1.set_count(5, 3);
  // l1.set_mode(BM_PL_ATTACK);
  // l1.set_velocity(0, 0.5f);
  // world.live_bullets.push_back(l1);

  auto player = world.em.create_entity();
  world.em.add_component<CTransform>(player, {
      .position = { 1000, 1000 },
      .scale = { 50, 50 },
      .rotation = 90
    });
  world.em.add_component<CVelocity>(player, {
      .velocity { 0, 0 }
    });
  world.em.add_component<CHealth>(player, {
      .health = 3
    });
  world.em.add_component<CInput>(player, {
    });

  auto event_queue = std::make_shared<RingBuffer<Event, 128>>();
  NetClient nc(event_queue);

  float currentSlice = 0.f;
  int lastFrameTime = 0;
  auto lastTimePt = std::chrono::high_resolution_clock::now();
  auto deltaTime = std::chrono::high_resolution_clock::now() - lastTimePt;

  while(!WindowShouldClose()) {
    currentSlice += lastFrameTime;
    for(; currentSlice >= SLICE; currentSlice -= SLICE) {
      tick(world);
    }
    render(world);
    deltaTime = std::chrono::high_resolution_clock::now() - lastTimePt;
    lastTimePt = std::chrono::high_resolution_clock::now();

    int ft = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(deltaTime).count();
    lastFrameTime = ft;
  }

  CloseWindow();
  return;
}
