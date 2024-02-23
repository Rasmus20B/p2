module;

#include <bitset>
#include <vector>
#include <raylib.h>
#include <array>

import types;

export module ecs.components.bullets;

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

export struct CBulletPatterns {
  int cols = 0, rows = 0;
  float speed{};
  float angle{};
  std::bitset<BM_SIZE> modes;
  std::bitset<BPF_SIZE> pflags;
};

export struct CBulletManager {
  std::array<CBulletPatterns, 16> patterns;
};
