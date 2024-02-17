#include <chrono>
#include <print>

import ecs;
import player;
import types;
import config;
import rand_helper;

#include "../../src/common/benchmark.h"

struct World {
  EntityManager em;
  // std::vector<BulletMgr> live_bullets{};
  Player pl;
};

int main() {
  /* INIT */
  World world;
  auto player = world.em.create_entity();
  world.em.add_components<CTransform2D, CVelocity, CHealth, CInput>(player, {
      .position = { 300, 300 },
      .scale = { 10, 10 },
      .rotation = 90
    }, {
      .velocity { 0, 0 }
    }, {
      .health = 3
    }, {
    });
  for(int i = 0; i < 5000; ++i) {
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
          .cache = { 0, 0 }
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
  /* INIT OVER */

  for(auto i = 0; i < 10000; ++i) {
    auto t_entities = world.em.get_associated_entities<CTransform2D>();
    std::vector<Entity> entities(t_entities.begin(), t_entities.end());
    auto t_attractors = world.em.get_associated_entities<CAttraction>();
    std::vector<Entity> a_entities(t_attractors.begin(), t_attractors.end());
    (orientToAttractor(a_entities));
    (moveTransformAll(entities));
    (checkCollisionsWithSingleEntity(a_entities, 1));
    (checkOutOfBounds(entities));
  }
  return 0;
}
