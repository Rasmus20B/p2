module;

import component;

export module system;

export void moveTransform(CTransform& t, CVelocity& v) {
  t.position.x += v.velocity.x;
  t.position.y += v.velocity.y;
}
export void moveTransformAll() {
  for(int i = 0; i < component_manager.transforms.size(); ++i) {
    component_manager.transforms[i].position.x += component_manager.velocities[i].velocity.y;
    component_manager.transforms[i].position.y += component_manager.velocities[i].velocity.y;
  }
}

