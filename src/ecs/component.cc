module;

#include <raylib.h>

#include <vector>
#include <variant>
#include <functional>
#include <print>

import types;
import opcodes;

export module ecs.component;



export enum class ComponentID {
  Transform2D,
  Velocity,
  Collider,
  BulletInfo,
  Health,
  Parent,
  Children,
  Script,
  Input,
  Sprite,
  Attractor,
  Size,
};

export struct CTransform2D {
  Vector2 position;
  Vector2 scale;
  f32 rotation;
};

export struct CVelocity {
  Vector2 velocity;
};

export struct CCollider {
  std::function<void(Entity self, Entity collided)> callback;
};

export struct CBulletInfo {

};

export struct CHealth {
  i32 health;
};

export struct CParent {
  Entity parent;
};

export struct CChildren {
  std::vector<Entity> children;
};


export struct CSprite {

};

export struct CInput {

};

export struct CAttraction {
  Entity attractor;
  f32 gravity;
  Vector2 cache;
};

export struct CScript {

  CScript() = default;
  CScript(CScript const &other) : program(other.program) {
    init_from_script();
  };
  CScript(const std::vector<u8>& code) : program(code) {
    init_from_script();
  }

  enum DataType {
    Integer,
    Float,
  };

  struct StackSlot {
    std::variant<i32, f32> data;
    DataType type;
  };


  void init_from_script() {
    constexpr std::array magic = { 0x7f, 0x44, 0x4d, 0x4c };
    for(int i = 0; i < 4; ++i) {
      if(magic[i] != program[i]) {
        std::println("Unrecognized file format.");
        break;
      }
    }
    // Set pc to the entry point found in header file
    pc += (program[4]) & 0x000000FF;
    pc += (program[5] << 8) & 0x0000FF00;
    pc += (program[6] << 16) & 0x00FF0000;
    pc += (program[7] << 24) & 0xFF000000;
  }

  [[nodiscard]]
  float get_float_operand() noexcept {
    std::vector<uint8_t> num_str(program.begin() + pc + 1, program.begin() + pc + 5);
    float res = 0;
    memcpy(&res, num_str.data(), 4);
    return res;
  }

  [[nodiscard]]
  int get_int_operand() noexcept {
    int num = 0;
    num += program[pc+1] & 0x000000FF;
    num += (program[pc+2] << 8) & 0x0000FF00;
    num += (program[pc+3] << 16) & 0x00FF0000;
    num += (program[pc+4] << 24) & 0xFF000000;
    return num;
  }

  OpCode consume_op() {
    int op = program[pc];
    op = op << 8;
    pc++;
    op |= program[pc];
    auto opcode = static_cast<OpCode>(op);
    return opcode;
  }

  u16 pc = 0;
  u16 sp = 0;
  u16 waitctr = 0;
  std::vector<u8> program;
  std::vector<StackSlot> memory;
};

export struct ComponentManager {
  ComponentManager() {
    transforms.resize(100000);
    velocities.resize(100000);
    health.resize(100000);
    sprites.resize(100000);
    vms.resize(100000);
    inputs.resize(100000);
    attractions.resize(100000);
    colliders.resize(100000);
    scripts.resize(100000);
  }
  std::vector<CTransform2D> transforms;
  std::vector<CVelocity> velocities;
  std::vector<CHealth> health;
  std::vector<CSprite> sprites;
  std::vector<CScript> vms;
  std::vector<CInput> inputs;
  std::vector<CAttraction> attractions;
  std::vector<CCollider> colliders;
  std::vector<CScript> scripts;
};

export inline ComponentManager component_manager{};

export template<typename T>
consteval ComponentID get_component_id() {
  if constexpr(std::is_same_v<T, CTransform2D>) {
    return ComponentID::Transform2D;
  }
  else if constexpr(std::is_same_v<T, CVelocity>) {
    return ComponentID::Velocity;
  }
  else if constexpr (std::is_same_v<T, CScript>) {
    return ComponentID::Script;
  }
  else if constexpr (std::is_same_v<T, CHealth>) {
    return ComponentID::Health;
  }
  else if constexpr (std::is_same_v<T, CInput>) {
    return ComponentID::Input;
  }
  else if constexpr (std::is_same_v<T, CAttraction>) {
    return ComponentID::Attractor;
  }
};
