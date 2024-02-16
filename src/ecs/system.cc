module;

import ecs.component;
import ecs.entity;
import types;
import config;

#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <cassert>

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

export void moveTransformAll(const std::vector<Entity>& es) {
  for(auto i : es) {
    component_manager.transforms[i].position.x += component_manager.velocities[i].velocity.x;
    component_manager.transforms[i].position.y += component_manager.velocities[i].velocity.y;
  }
}

export void checkOutOfBounds(const std::vector<Entity>& es) {
  for(auto i: es) {
    if(component_manager.transforms[i].position.x < 0 ||
       component_manager.transforms[i].position.y < 0 ||
       component_manager.transforms[i].position.x > config.windowDimensions.x ||
       component_manager.transforms[i].position.y > config.windowDimensions.y) {
      deads.push(i);
    }
  }
}

export [[gnu::always_inline]] void orientToAttractorSIMD(const std::vector<Entity>& es) {
    for(int i = 0; i + 1 < es.size(); i = (i + 2)) {
      Entity att1 = component_manager.attractions[es[i]].attractor;
      Entity att2 = component_manager.attractions[es[i+1]].attractor;

      float32x2_t dx = { component_manager.transforms[es[i]].position.x,
                         component_manager.transforms[es[i+1]].position.x,
                        };
      float32x2_t attx = { component_manager.transforms[att1].position.x,
                           component_manager.transforms[att2].position.x,
                        };

      float32x2_t dy = { component_manager.transforms[es[i]].position.y,
                         component_manager.transforms[es[i+1]].position.y,
                        };

      float32x2_t atty = { component_manager.transforms[att1].position.y,
                           component_manager.transforms[att2].position.y,
                        };
      dx = vsub_f32(attx, dx);
      dy = vsub_f32(atty, dy);
      float32x2_t vx = vmul_f32(dx, dx);
      float32x2_t vy = vmul_f32(dy, dy);
      float32x2_t val = vadd_f32(vx, vy);
      float32x2_t dist = vsqrt_f32(val);
      float32x2_t gravities = { component_manager.attractions[es[i]].gravity,
                                component_manager.attractions[es[i+1]].gravity,
                          };
      float32x2_t powers = vdiv_f32(gravities, dist);
      float32x2_t lhs = vdiv_f32(dx, dist);
      float32x2_t vels = vmul_f32(lhs, powers);
      component_manager.velocities[es[i]].velocity.x = vels[0];
      component_manager.velocities[es[i+1]].velocity.x = vels[1];
      lhs = vdiv_f32(dy, dist);
      vels = vmul_f32(lhs, powers);
      component_manager.velocities[es[i]].velocity.y = vels[0];
      component_manager.velocities[es[i+1]].velocity.y = vels[1];
    }
}
export [[gnu::always_inline]] void orientToAttractorSIMDQ(const std::vector<Entity>& es) {
    for(int i = 0; i + 1 < es.size(); i += 4) {
      Entity att1 = component_manager.attractions[es[i]].attractor;
      Entity att2 = component_manager.attractions[es[i+1]].attractor;
      Entity att3 = component_manager.attractions[es[i+2]].attractor;
      Entity att4 = component_manager.attractions[es[i+3]].attractor;

      float32x4_t dx = { component_manager.transforms[es[i]].position.x,
                         component_manager.transforms[es[i+1]].position.x,
                         component_manager.transforms[es[i+2]].position.x,
                         component_manager.transforms[es[i+3]].position.x,
                        };
      float32x4_t attx = { component_manager.transforms[att1].position.x,
                           component_manager.transforms[att2].position.x,
                           component_manager.transforms[att3].position.x,
                           component_manager.transforms[att4].position.x
                        };

      float32x4_t dy = { component_manager.transforms[es[i]].position.y,
                         component_manager.transforms[es[i+1]].position.y,
                         component_manager.transforms[es[i+2]].position.y,
                         component_manager.transforms[es[i+3]].position.y,
                        };

      float32x4_t atty = { component_manager.transforms[att1].position.y,
                           component_manager.transforms[att2].position.y,
                           component_manager.transforms[att3].position.y,
                           component_manager.transforms[att4].position.y
                        };
      dx = vsubq_f32(attx, dx);
      dy = vsubq_f32(atty, dy);
      float32x4_t vx = vmulq_f32(dx, dx);
      float32x4_t vy = vmulq_f32(dy, dy);
      float32x4_t val = vaddq_f32(vx, vy);
      float32x4_t dist = vsqrtq_f32(val);
      float32x4_t gravities = { component_manager.attractions[es[i]].gravity,
                                component_manager.attractions[es[i+1]].gravity,
                                component_manager.attractions[es[i+2]].gravity,
                                component_manager.attractions[es[i+3]].gravity,
                          };
      float32x4_t powers = vdivq_f32(gravities, dist);
      float32x4_t lhs = vdivq_f32(dx, dist);
      float32x4_t vels = vmulq_f32(lhs, powers);
      component_manager.velocities[es[i]].velocity.x = vels[0];
      component_manager.velocities[es[i+1]].velocity.x = vels[1];
      component_manager.velocities[es[i+2]].velocity.x = vels[2];
      component_manager.velocities[es[i+3]].velocity.x = vels[3];
      lhs = vdivq_f32(dy, dist);
      vels = vmulq_f32(lhs, powers);
      component_manager.velocities[es[i]].velocity.y = vels[0];
      component_manager.velocities[es[i+1]].velocity.y = vels[1];
      component_manager.velocities[es[i+2]].velocity.y = vels[2];
      component_manager.velocities[es[i+3]].velocity.y = vels[3];
    }
  }
export void orientToAttractor(const std::vector<Entity>& es) {
#pragma clang loop vectorize(enable)
  for(auto i: es) {
    __builtin_prefetch(&component_manager.attractions[(2 * i + 1) % component_manager.attractions.size()]);
    const auto att = component_manager.attractions[i].attractor;
    auto dvec = Vector2Subtract(component_manager.transforms[att].position, component_manager.transforms[i].position);
    auto val = (dvec.x * dvec.x) + (dvec.y * dvec.y);
    float dist = sqrtf(val);
    float cur_power = component_manager.attractions[i].gravity / dist;
    component_manager.velocities[i].velocity.x = (dvec.x/dist) * cur_power;
    component_manager.velocities[i].velocity.y = (dvec.y/dist) * cur_power;
  }
}

export void checkCollisionsWithSingleEntity(const std::vector<Entity>& es, const Entity e) {
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

