module;

#include <bitset>
#include <vector>
#include <raylib.h>
#include <array>

import types;

export module ecs.components.bullets;

export namespace ecs {
  namespace component {
    enum BulletMode {
      BM_NORM,
      BM_NORM_AIM,
      BM_CIRC_AIM,
      BM_CIRC,
      BM_SIZE
    };

    enum BulletPFlag {
      BPF_EN_ATTACK,
      BPF_PL_ATTACK,
      BPF_SIZE
    };

    struct CBulletPatterns {
      i32 rows = 0, columns = 0;
      f32 speed1{}, speed2{};
      f32 angle1{}, angle2{};
      std::bitset<BM_SIZE> modes;
      std::bitset<BPF_SIZE> pflags;
    };

    struct CBulletManager {
      std::array<CBulletPatterns, 16> patterns;
    };
  }
}
