module;

#include <variant>

#include <raylib.h>

export module event;

export enum EventType {
  ET_PL_MOVE,
  ET_SIZE
};

struct Movement {
  int player;
  Vector2 pos;
  Vector2 vel;
};

struct Death {
  int player;
};

struct Shoot {
  int player;
};

export struct Event {
  std::variant<Movement, Death, Shoot> e;
  EventType et;
};
