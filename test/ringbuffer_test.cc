import ring_buffer;
#include <gtest/gtest.h>

#include <expected>

TEST(ringbuf, norm) {

  RingBuffer<int, 8> rb;

  for(int i = 0; i < 8; ++i) {
    rb.push(i);
  }

  for(int i = 0; i < 7; ++i) {
    ASSERT_EQ(i, *rb.pop());
  }
}
