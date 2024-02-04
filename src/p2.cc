module;

#include <thread>
#include <system_error>
#include <vector>
#include <ratio>
#include <chrono>
#include <iostream>
#include <algorithm>

#include <raylib.h>

import bullets;
import player;
import event;
import ring_buffer;
import net;
import types;
import ecs;

export module p2;

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
  auto t_entities = w.em.get_associated_entities<CTransform>();

  // std::for_each(t_entities.begin(), t_entities.end(), [](const auto i) {

  for(auto i: t_entities) {
    DrawRectangle(component_manager.transforms[i].position.x, 
      component_manager.transforms[i].position.y, 
      component_manager.transforms[i].scale.x, 
      component_manager.transforms[i].scale.y, RED);
  };
  DrawText(std::to_string(w.em.e_count).c_str(), 200, 400, 20, ORANGE);

  DrawFPS(200, 200);
  EndDrawing();
}

void tick(World &w) {
  handle_player(w.em, w.live_bullets);
  // Bullet movement
  auto t_entities = w.em.get_associated_entities<CTransform>();
  std::vector<Entity> entities(t_entities.begin(), t_entities.end());
  auto tmp = w.em.get_associated_entities<CTransform>();
  for(auto t: tmp) {
    entities.push_back(t);
  }
  // t_entities.merge(w.em.get_associated_entities<CVelocity>());
  moveTransformAll(entities);
  checkOutOfBounds(entities);

  removeDeads(w.em);
}

export void gameloop() {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  SetTargetFPS(60);
  const Vector2 screenDim = {
    .x = (float)640,
    .y = (float)480,
  };
  InitWindow(screenDim.x, screenDim.y, "p2");

  World world;

  auto player = world.em.create_entity();
  world.em.add_component<CTransform>(player, {
      .position = { 300, 300 },
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
