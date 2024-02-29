module;

#include <bitset>
#include <vector>
#include <raylib.h>
#include <array>

import types;

export module ecs.components.bullets;

export enum BulletMode {
  BM_NORM,
  BM_NORM_AIM,
  BM_CIRC_AIM,
  BM_CIRC,
  BM_SIZE
};

export enum BulletPFlag {
  BPF_EN_ATTACK,
  BPF_PL_ATTACK,
  BPF_SIZE
};

export struct CBulletPatterns {
  int rows = 0, columns = 0;
  float speed1{}, speed2{};
  float angle1{}, angle2{};
  std::bitset<BM_SIZE> modes;
  std::bitset<BPF_SIZE> pflags;
};

export struct CBulletManager {
  std::array<CBulletPatterns, 16> patterns;
};
