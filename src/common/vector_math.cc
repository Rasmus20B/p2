module;

#include <raylib.h>

#include <cmath>

export module vector_math;

export Vector2 rotate(const Vector2 v, const float angle) {
  return { v.x * (float)cos(angle) - (v.y * (float)sin(angle)),
           v.x * (float)sin(angle) + (v.y * (float)cos(angle))};
}

export float dot(const Vector2 v1, const Vector2 v2) {
  return (v1.x * v2.x) + (v1.y * v2.y);
}

export Vector2 minus(const Vector2 v1, const Vector2 v2) {
  return { v1.x - v2.x, v1.y - v2.y };
}
