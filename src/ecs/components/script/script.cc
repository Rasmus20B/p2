module;

#include <vector>
#include <print>
#include <variant>

import types;

export module ecs.components.script;
export import ecs.components.script.opcodes;

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
