module;

import ecs.component;
import ecs.entity;

import ecs.component;
import types;
import config;
import asset_manager;

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
      if (CheckCollisionCircles(component_manager.transforms[i].position, component_manager.transforms[i].scale.x * 0.5f,
            component_manager.transforms[e].position, component_manager.transforms[e].scale.x * 0.5f)) {
        component_manager.colliders[i].callback(i, e);
      }
    }
  }

  void handle_inputs(const std::span<Entity> es, EntityManager& em) {

    for(auto e: es) {
      if (IsKeyDown(KEY_UP)) {
        component_manager.velocities[e].velocity.y = -0.5f;
      } else if (IsKeyDown(KEY_DOWN)) {
        component_manager.velocities[e].velocity.y = 0.5f;
      } else {
        component_manager.velocities[e].velocity.y = 0;
      }
      if (IsKeyDown(KEY_LEFT)) {
        component_manager.velocities[e].velocity.x = -0.5f;
      } else if (IsKeyDown(KEY_RIGHT)) {
        component_manager.velocities[e].velocity.x = 0.5f;
      } else {
        component_manager.velocities[e].velocity.x = 0;
      }

      // Player Shooting
      if (IsKeyDown(KEY_Z)) {
        auto bl = em.create_entity();
        em.add_component<CTransform2D>(bl, {
            { 
            component_manager.transforms[e].position.x,
            component_manager.transforms[e].position.y 
            },
            {
            20, 40
            },
            90});
        em.add_component<CVelocity>(bl, {
            { 
            0, -0.5
            }
          });
      }
    }
  }

  void remove_deads(EntityManager& em) {
    while(!deads.empty()) {
      auto e = deads.front();
      deads.pop();
      em.delete_entity(e);
      reuse.push(e);
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
        auto player = component_manager.transforms[1].position;
        auto enm = component_manager.transforms[e].position;
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

            auto& bp = component_manager.bullets[e].patterns[slot];

            auto sprite = assets.get_sprite(SpriteRef::ORB1);
            auto dvec = Vector2Subtract(player, enm);
            auto aim_angle = std::atan2(dvec.y, dvec.x);
            if(bp.modes.test(BM_NORM_AIM)) {
              auto even_diff = bp.columns & 1 ? 0 : 0.5;
              for(auto i = 0; i < bp.rows; ++i) {
                float lspeed = (bp.speed1 + bp.speed2) + ((bp.speed1 - bp.speed2) / (float(bp.rows) / (i)));
                for(auto j = 0; j < bp.columns; ++j) {
                    auto diff = (j + 1) - round(0.5 * bp.columns);
                    auto new_angle = aim_angle + (diff * bp.angle2) - (even_diff * bp.angle2);
                    vel.x = std::cos(new_angle) * lspeed * 0.005;
                    vel.y = std::sin(new_angle) * lspeed * 0.005;
                    auto bullet = em.create_entity();
                    em.add_components<CTransform2D, CVelocity, CSprite>(bullet,
                      {
                        enm,
                        {
                          static_cast<float>(sprite->width),
                          static_cast<float>(sprite->height)
                        },
                        component_manager.bullets[e].patterns[slot].angle1,
                      }, {
                        vel
                      }, {
                        sprite
                      }
                    );
                  }
                }
              } else if(bp.modes.test(BM_CIRC_AIM)) {
                auto ang_step =  360.f / bp.columns;
                for(auto i = 0; i < bp.rows; ++i) {
                  float lspeed = (bp.speed1 + bp.speed2) + ((bp.speed1 - bp.speed2) / (float(bp.rows) / (i)));
                  for(auto j = 0; j < bp.columns; ++j) {
                    auto angle_diff = ang_step * j;
                    auto new_angle = aim_angle + (angle_diff * (PI / 180));
                    vel.x = std::cos(new_angle) * lspeed * 0.005;
                    vel.y = std::sin(new_angle) * lspeed * 0.005;
                    auto bullet = em.create_entity();
                    em.add_components<CTransform2D, CVelocity, CSprite>(bullet,
                      {
                        enm,
                        {
                          static_cast<float>(sprite->width),
                          static_cast<float>(sprite->height)
                        },
                        component_manager.bullets[e].patterns[slot].angle1,
                      }, {
                        vel
                      }, {
                        sprite
                      }
                    );
                  }
                }
              }
            break;
          }
          case OpCode::ETSPRITE:
            break;

          case OpCode::ETOFFSET:
            break;

          case OpCode::ETANGLE: {
            auto slot = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto angle1 = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto angle2 = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 5;

            auto& bp = component_manager.bullets[e].patterns[slot];
            bp.angle1 = angle1 * (PI / 180);
            bp.angle2 = angle2 * (PI / 180);
            break;
          }
          case OpCode::ETSPEED: {
            auto slot = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto speed1 = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto speed2 = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 5;
            auto& bp = component_manager.bullets[e].patterns[slot];
            bp.speed1 = speed1;
            bp.speed2 = speed2;
            break;
          }
          case OpCode::ETCOUNT: {
            auto slot = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto rows = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto columns = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 5;

            auto& bp = component_manager.bullets[e].patterns[slot];
            bp.rows = rows;
            bp.columns = columns;
            break;
          }
          case OpCode::ETAIM: {
            auto slot = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 4;
            auto mode = component_manager.scripts[e].get_int_operand();
            component_manager.scripts[e].pc += 5;

            auto& bp = component_manager.bullets[e].patterns[slot];
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

            auto delta = Vector2Subtract({static_cast<f32>(x), static_cast<f32>(y)}, component_manager.transforms[e].position);
            component_manager.velocities[e].velocity = {
              .x = (delta.x / time ),
              .y = (delta.y / time )
            };
            component_manager.scripts[e].waitctr = time;
            component_manager.scripts[e].state.set(std::to_underlying(CScript::VMState::MOVING));
            break;
          }
          default:
            std::println("Unimplmented Opcode. {:x}", std::to_underlying(op));
        }
      }
    }
  }
}
