module;

import types;

#include <random>

export module rand_helper;

export inline f32 get_rand_float(f32 max, f32 min = 0) {
  return static_cast<f32>(rand() / (static_cast<f32>(RAND_MAX)/max)) + min;
}
