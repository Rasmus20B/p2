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
import config;
import rand_helper;

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

  for(auto i: t_entities) {
    DrawCircleV(component_manager.transforms[i].position,
      component_manager.transforms[i].scale.x, 
      RED);
  };


  DrawFPS(200, 200);
  EndDrawing();
}

void tick(World &w) {
  handle_player(w.em, w.live_bullets);
  // Bullet movement
  auto t_entities = w.em.get_associated_entities<CTransform>();
  std::vector<Entity> entities(t_entities.begin(), t_entities.end());
  auto t_attractors = w.em.get_associated_entities<CAttraction>();
  std::vector<Entity> a_entities(t_attractors.begin(), t_attractors.end());
  orientToAttractor(a_entities);
  checkCollisionsWithSingleEntity(a_entities, 1);
  moveTransformAll(entities);
  checkOutOfBounds(entities);

  removeDeads(w.em);
}

export void gameloop() {

  srand(time(0));
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  SetTargetFPS(60);

  config.windowDimensions = { 1080, 768 };

  InitWindow(config.windowDimensions.x, config.windowDimensions.y, "p2");

  World world;

  auto player = world.em.create_entity();
  world.em.add_component<CTransform>(player, {
      .position = { 300, 300 },
      .scale = { 10, 10 },
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


  for(int i = 0; i < 2000; ++i) {
    auto other = world.em.create_entity();
    world.em.add_components<CTransform, CVelocity, CHealth, CAttraction, CCollider>(other, {
        .position = { get_rand_float(config.windowDimensions.x ), get_rand_float(config.windowDimensions.y ) },
        .scale = { 3, 3 },
        .rotation = 90
        }, {
        .velocity { get_rand_float(0.04, -0.04), get_rand_float(0.04, -0.04) },
        }, {
        .health = 20,
        }, {
          .attractor = player,
          .gravity = 0.8,
        }, {
          .callback = [=](const auto a, const auto b) {
            deads.push(a);
            component_manager.transforms[b].scale.x += 0.05;
            component_manager.transforms[b].scale.y += 0.05;
          }
        }
      );
  }

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
