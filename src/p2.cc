module;

#include <thread>
#include <system_error>
#include <vector>
#include <ratio>
#include <chrono>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <execution>
#include <ranges>

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"

import asset_manager;
import ecs;
import event;
import ring_buffer;
import net;
import types;
import ecs;
import config;
import rand_helper;

import main_menu;

#include "common/benchmark.h"

#include <print>

export module p2;

constinit float SLICE = 1.f;

float dt = 0.f;;

struct World {
  EntityManager em;
};

void render(World &w) {
  BeginDrawing();
  ClearBackground(BLACK);
  auto t_entities = w.em.get_associated_entities<CTransform2D>();
  auto s_entities = w.em.get_associated_entities<CSprite>();

  for(auto i: t_entities) {
    DrawCircleV(component_manager.transforms[i].position,
      component_manager.transforms[i].scale.x, 
      RED);
  };

  for(auto i: s_entities) {
    DrawTextureV(component_manager.sprites[i].sprite, 
        {
          component_manager.transforms[i].position.x - (component_manager.sprites[i].sprite.width * 0.5f), 
          component_manager.transforms[i].position.y - (component_manager.sprites[i].sprite.height * 0.5f), 
        },
        RAYWHITE);
  }
  DrawFPS(20, 20);

  DrawText(std::to_string(t_entities.size()).append(" Entities.").data(), 20, 40, 20, RAYWHITE);
  EndDrawing();
}

void tick(World &w) {

  auto p_entities = w.em.get_associated_entities<CInput>();
  systems::handle_inputs(p_entities, w.em);

  // Bullet movement
  auto t_entities = w.em.get_associated_entities<CTransform2D>();
  std::vector<Entity> entities(t_entities.begin(), t_entities.end());
  auto t_attractors = w.em.get_associated_entities<CAttraction>();
  std::vector<Entity> a_entities(t_attractors.begin(), t_attractors.end());
  systems::orient_to_attractor(a_entities);
  systems::move_transform(entities);
  systems::check_collisions_with_single_entity(a_entities, 1);
  systems::remove_out_of_bounds(entities);
  systems::remove_deads(w.em);
  auto t_scripts = w.em.get_associated_entities<CScript>();
  systems::progress_script(t_scripts, w.em);
}

export void gameloop() {

  srand(time(0));
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  SetTargetFPS(60);

  config.windowDimensions = { 1080, 768 };
  InitWindow(config.windowDimensions.x, config.windowDimensions.y, "p2");

#if 0
  auto choice = main_menu();
  if(!choice) return;
#endif

  World world;

  assets.init_asset_manager();

  auto player = world.em.create_entity();
  world.em.add_components<CTransform2D, CVelocity, CHealth, CInput>(player, {
      .position = { 300, 300 },
      .scale = { 10, 10 },
      .rotation = 90
    }, {
      .velocity { 0, 0 }
    }, {
      .health = 3
    },
    {});

  for(int i = 0; i < 100; ++i) {
    auto other = world.em.create_entity();
    world.em.add_components<CTransform2D, CVelocity, CHealth, CAttraction, CCollider>(other, {
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
          .callback { [=](const auto a, const auto b) {
            deads.push(a);
            component_manager.transforms[b].scale.x += 0.005;
            component_manager.transforms[b].scale.y += 0.005;
            }
          }
        }
      );
  }

  std::vector<u8> prog = assets.scripts[0];
  auto enemy = world.em.create_entity();
  world.em.add_components<CTransform2D, CHealth, CScript>(enemy, {
      .position = {0, 0},
      .scale = { 10, 10 },
      .rotation = 90
      }, {
        .health = 20
      }, CScript(prog)
  );

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
