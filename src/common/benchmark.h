#pragma once

#include <chrono>
#include <iostream>

#define BENCHMARK_START auto start = std::chrono::high_resolution_clock::now();
#define BENCHMARK_END auto end = std::chrono::high_resolution_clock::now();
#define BENCHMARK_DISPLAY std::cout << (end - start).count() << "\n";

#define BENCH(f, name) \
  bench_start = std::chrono::high_resolution_clock::now(); \
  f; \
  bench_end = std::chrono::high_resolution_clock::now(); \
  std::cout <<  name << " Took: " << std::chrono::duration_cast<std::chrono::nanoseconds>(bench_end - bench_start).count() << "\n";


static auto bench_start = std::chrono::high_resolution_clock::now();
static auto bench_end = std::chrono::high_resolution_clock::now();
