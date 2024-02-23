module;

import ecs.component;
import ecs.entity;

import opcodes;
import types;
import config;

#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <cassert>
#include <span>

#include <print>

#include <arm_neon.h>

#include <thread>

#include <raylib.h>
#include <raymath.h>

export module ecs.system;

export void moveTransform(CTransform2D& t, CVelocity& v) {
  t.position.x += v.velocity.x;
  t.position.y += v.velocity.y;
}

export void moveTransformPar(const std::vector<Entity>& es, int t, int nt) {
  size_t i = (es.size() / nt) * t;
  size_t end = i + (es.size() / nt);

  for(; i < end; i++) {
    component_manager.transforms[es[i]].position.x += component_manager.velocities[es[i]].velocity.x;
    component_manager.transforms[es[i]].position.y += component_manager.velocities[es[i]].velocity.y;
  }
}

export void moveTransformAll(const std::span<Entity> es) {
  for(auto i : es) {
    component_manager.transforms[i].position = Vector2Add(component_manager.transforms[i].position, component_manager.velocities[i].velocity);
  }
}

export void checkOutOfBounds(const std::span<Entity> es) {
  for(auto i: es) {
    if(component_manager.transforms[i].position.x < 0 ||
       component_manager.transforms[i].position.y < 0 ||
       component_manager.transforms[i].position.x > config.windowDimensions.x ||
       component_manager.transforms[i].position.y > config.windowDimensions.y) {
      deads.push(i);
    }
  }
}

export void orientToAttractor(const std::span<Entity> es) {
  for(auto i: es) {
    const auto att = component_manager.attractions[i].attractor;
    auto entity_pos = component_manager.transforms[i].position;
    auto attractor_pos = component_manager.transforms[att].position;

    if(attractor_pos.x == component_manager.attractions[i].cache.x &&
        attractor_pos.y == component_manager.attractions[i].cache.y) {
      continue;
    }
    component_manager.attractions[i].cache = attractor_pos;
    auto dvec = Vector2Subtract(attractor_pos, entity_pos);
    auto val = (dvec.x * dvec.x) + (dvec.y * dvec.y);
    float dist = sqrtf(val);
    float cur_power = component_manager.attractions[i].gravity / dist;
    component_manager.velocities[i].velocity.x = (dvec.x/dist) * cur_power;
    component_manager.velocities[i].velocity.y = (dvec.y/dist) * cur_power;
  }
}

export void checkCollisionsWithSingleEntity(const std::span<Entity> es, const Entity e) {
  for(auto i: es) {
    if (CheckCollisionCircles(component_manager.transforms[i].position, component_manager.transforms[i].scale.x,
          component_manager.transforms[e].position, component_manager.transforms[e].scale.x)) {
      component_manager.colliders[i].callback(i, e);
    }
  }
}

export void removeDeads(EntityManager& em) {
  while(!deads.empty()) {
    auto e = deads.front();
    deads.pop();
    reuse.push(e);
    em.delete_entity(e);
  }
}

export void scriptSystem(std::span<Entity> es) {
  for(auto e: es) {
    if(component_manager.scripts[e].waitctr > 0) {
      component_manager.scripts[e].waitctr--;
    } else {
      auto op = component_manager.scripts[e].consume_op();
      switch(op) {
        case OpCode::WAIT:
          component_manager.scripts[e].waitctr = component_manager.scripts[e].get_int_operand();
          component_manager.scripts[e].pc += 5;
          break;
        case OpCode::JMP:
          component_manager.scripts[e].pc = component_manager.scripts[e].get_int_operand();
          break;
        case OpCode::MOVEPOS: {
          auto x = component_manager.scripts[e].get_int_operand();
          component_manager.scripts[e].pc += 4;
          auto y = component_manager.scripts[e].get_int_operand();
          component_manager.scripts[e].pc += 5;
          component_manager.transforms[e].position = {
            static_cast<f32>(x),
            static_cast<f32>(y)
          };
          break;
        }
        default:
          std::println("Uninplmeneted Opcode.");
      }
    }
  }
}
