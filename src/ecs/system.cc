module;

import ecs.component;
import ecs.entity;
import types;

#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <algorithm>
#include <functional>

#include <thread>

export module ecs.system;

export void moveTransform(CTransform& t, CVelocity& v) {
  t.position.x += v.velocity.x;
  t.position.y += v.velocity.y;
}

export void moveTransformPar(std::vector<Entity>& es, int t, int nt) {
  size_t i = (es.size() / nt) * t;
  size_t end = i + (es.size() / nt);

  for(; i < end; i++) {
    component_manager.transforms[es[i]].position.x += component_manager.velocities[es[i]].velocity.x;
    component_manager.transforms[es[i]].position.y += component_manager.velocities[es[i]].velocity.y;
  }
}

export void moveTransformAll(std::vector<Entity> es) {

  for(auto i : es) {
    component_manager.transforms[i].position.x += component_manager.velocities[i].velocity.x;
    component_manager.transforms[i].position.y += component_manager.velocities[i].velocity.y;
  }
}

export void checkOutOfBounds(std::vector<Entity> es) {
  for(auto i: es) {
    if(component_manager.transforms[i].position.x < 0 ||
       component_manager.transforms[i].position.y < 0) {
      deads.push(i);
    }
  }
}

export void removeDeads(EntityManager& em) {
  while(!deads.empty()) {
    auto e = deads.front();
    deads.pop();
    em.delete_entity(e);
    reuse.push(e);
  }
}

