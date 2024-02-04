module;

import types;

#include <random>

export module rand_helper;

export f32 get_rand_float(f32 max) {
  return static_cast<f32>(rand() / (static_cast<f32>(RAND_MAX)/max));
}
