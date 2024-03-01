module;

#include <vector>
#include <iostream>
#include <expected>
#include <thread>
#include <mutex>

export module ring_buffer;

enum class RB_exceptions {
  RB_Empty,
  RB_Size
};

export template<typename T, size_t S>
class RingBufferMP {
  public:
    [[nodiscard]] bool empty() const {
      return (tail == head);
    }

    [[nodiscard]] bool full() const {
      return (head + 1) % S == tail;
    }

    T front() {
      return arr[tail];
    }

    bool push(T val) {
      std::scoped_lock l(m);
      if(full()) {
        return false;
      }
      arr[head] = val;
      head = (head + 1) % S;
      return true;
    }

    std::expected<T, RB_exceptions> pop() {
      std::scoped_lock l(m);
      if(empty()) return std::unexpected(RB_exceptions::RB_Empty);
      auto ret = arr[tail];
      tail = (tail + 1) % S;
      return ret;
    }

  private:
    std::array<T, S> arr;
    std::mutex m;
    size_t tail = 0;
    size_t head = 0;
};


export template<typename T, size_t S>
class RingBuffer {
  public:
    [[nodiscard]] bool empty() const {
      return (tail == head);
    }

    [[nodiscard]] bool full() const {
      return (head + 1) % S == tail;
    }

    T front() {
      return arr[tail];
    }

    bool push(T val) {
      if(full()) {
        return false;
      }
      arr[head] = val;
      head = (head + 1) % S;
      return true;
    }

    std::expected<T, RB_exceptions> pop() {
      if(empty()) return std::unexpected(RB_exceptions::RB_Empty);
      auto ret = arr[tail];
      tail = (tail + 1) % S;
      return ret;
    }

  private:
    std::array<T, S> arr;
    size_t tail = 0;
    size_t head = 0;
};


