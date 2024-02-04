module;

#include <bitset>
#include <vector>
#include <raylib.h>

import types;

export module bullets;


export enum BulletPFlag {
  BPF_NORM,
  BPF_NORM_AIM,
  BPF_CIRC_AIM,
  BPF_CIRC,
  BPF_SIZE
};

export enum BulletMode {
  BM_EN_ATTACK,
  BM_PL_ATTACK,
  BM_SIZE
};

export struct BulletMgr {

  void set_count(i32 l, i32 c) {
    count.x = l;
    count.y = c;
    positions.resize(l * c);

    i32 x = 0;

    for(auto &b: positions) {
      b.x = x;
      b.y = 0;
      x += 100;
    }
  }

  void set_origin(i32 x, i32 y) {
    for(auto& p: positions) {
      p.x = x;
      p.y = y;
    }
  }

  void set_angle(i32 a1) {
    angle = a1;
  }

  void set_velocity(float sp1, float sp2) {
    velocity.x = sp1;
    velocity.y = sp2;
  }

  void set_mode(BulletMode bm) {
    modes.set(bm);
  }

  void shoot() {
  }

  Vector2 get_bullet(i32 l, i32 c) {
    return positions[l * c + positions.size() - l];
  }

  std::vector<Vector2> positions;
  Vector2 velocity;
  float angle;
  Vector2 count;
  std::bitset<BM_SIZE> modes;
  std::bitset<BPF_SIZE> pflags;
};
