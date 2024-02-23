module;

import ecs.component;
import ecs.entity;

import ecs.component;
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

export namespace systems {

  void moveTransform(CTransform2D& t, CVelocity& v) {
    t.position.x += v.velocity.x;
    t.position.y += v.velocity.y;
  }

  void moveTransformPar(const std::vector<Entity>& es, int t, int nt) {
    size_t i = (es.size() / nt) * t;
    size_t end = i + (es.size() / nt);

    for(; i < end; i++) {
      component_manager.transforms[es[i]].position.x += component_manager.velocities[es[i]].velocity.x;
      component_manager.transforms[es[i]].position.y += component_manager.velocities[es[i]].velocity.y;
    }
  }

  void move_transform(const std::span<Entity> es) {
    for(auto i : es) {
      component_manager.transforms[i].position = Vector2Add(component_manager.transforms[i].position, component_manager.velocities[i].velocity);
    }
  }

  void remove_out_of_bounds(const std::span<Entity> es) {
    for(auto i: es) {
      if(component_manager.transforms[i].position.x < 0 ||
         component_manager.transforms[i].position.y < 0 ||
         component_manager.transforms[i].position.x > config.windowDimensions.x ||
         component_manager.transforms[i].position.y > config.windowDimensions.y) {
        deads.push(i);
      }
    }
  }

  void orient_to_attractor(const std::span<Entity> es) {
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

  void check_collisions_with_single_entity(const std::span<Entity> es, const Entity e) {
    for(auto i: es) {
      if (CheckCollisionCircles(component_manager.transforms[i].position, component_manager.transforms[i].scale.x,
            component_manager.transforms[e].position, component_manager.transforms[e].scale.x)) {
        component_manager.colliders[i].callback(i, e);
      }
    }
  }

  void remove_deads(EntityManager& em) {
    while(!deads.empty()) {
      auto e = deads.front();
      deads.pop();
      reuse.push(e);
      em.delete_entity(e);
    }
  }

  void progress_script(std::span<Entity> es, EntityManager& em) {
    for(auto e: es) {
      if(component_manager.scripts[e].waitctr > 0) {
        component_manager.scripts[e].waitctr--;
      } else {

        if(component_manager.scripts[e].state.test(std::to_underlying(CScript::VMState::MOVING))) {
          component_manager.velocities[e].velocity = {0, 0};
        }

        auto op = component_manager.scripts[e].consume_op();
        switch(op) {
          case OpCode::WAIT:
            component_manager.scripts[e].waitctr = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 5;
            break;

          case OpCode::CALL:
            break;

          case OpCode::JMP:
            component_manager.scripts[e].pc = component_manager.scripts[e].get_int_operand();
            break;

          case OpCode::JMPEQ:
            break;

          case OpCode::JMPNEQ:
            break;

          case OpCode::CALLASYNC:
            break;

          case OpCode::CALLASYNCID:
            break;

          case OpCode::ENMCREATE:
            break;


          case OpCode::ETNEW: {
            auto slot = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 5;
            assert(slot < 16);
            new(&component_manager.bullets[e].patterns[slot]) CBulletManager();
            break;
          }
          case OpCode::ETON: {
            auto slot = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 5;
            Vector2 vel{};

            if(component_manager.bullets[e].patterns[slot].modes.test(BPF_NORM_AIM)) {
              auto player = component_manager.transforms[1].position;
              auto enm = component_manager.transforms[e].position;
              auto dvec = Vector2Subtract(player, enm);
              auto aim_angle = std::atan2(dvec.y, dvec.x);
              vel.x = std::cos(aim_angle) * component_manager.bullets[e].patterns[slot].speed * 0.01;
              vel.y = std::sin(aim_angle) * component_manager.bullets[e].patterns[slot].speed * 0.01;
            }

            auto bullet = em.create_entity();
            em.add_components<CTransform2D, CVelocity>(bullet,
                {
                  component_manager.transforms[e].position,
                  {
                    5,
                    5
                  },
                  component_manager.bullets[e].patterns[slot].angle,
                }, {
                  vel
                }
            );
            break;
          }
          case OpCode::ETSPRITE:
            break;

          case OpCode::ETOFFSET:
            break;

          case OpCode::ETANGLE: {
            auto slot = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto angle = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            component_manager.scripts[e].pc += 5;
            component_manager.bullets[e].patterns[slot].angle = angle;
            break;
          }
          case OpCode::ETSPEED: {
            auto slot = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto speed = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            component_manager.scripts[e].pc += 5;
            component_manager.bullets[e].patterns[slot].speed = speed;
            break;
          }
          case OpCode::ETCOUNT:
            break;

          case OpCode::ETAIM: {
            auto slot = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto mode = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 5;

            component_manager.bullets[e].patterns[slot].modes.reset();
            component_manager.bullets[e].patterns[slot].modes.set(static_cast<BulletPFlag>(mode));
            break;
          }
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

          case OpCode::MOVEPOSTIME: {
            auto time = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto mode = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto x = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto y = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 5;

            auto pos = component_manager.transforms[e].position;
            auto delta = Vector2Subtract({float(x), float(y)}, component_manager.transforms[e].position);
            component_manager.velocities[e].velocity = {
              .x = (delta.x / time ),
              .y = (delta.y / time )
            };
            component_manager.scripts[e].waitctr = time;
            component_manager.scripts[e].state.set(std::to_underlying(CScript::VMState::MOVING));
            break;
          }
          default:
            std::println("Uninplmeneted Opcode. {:x}", std::to_underlying(op));
        }
      }
    }
  }
}
