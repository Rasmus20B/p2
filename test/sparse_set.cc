#include <gtest/gtest.h>

import sparse_set;

TEST(sparse_set, emplace_new) {
  basic_sparse_set set;

  set.try_emplace(4);
  set.try_emplace(2);
  set.try_emplace(6);

  ASSERT_EQ(set.contains(4), true);
  ASSERT_EQ(set.contains(2), true);
  ASSERT_EQ(set.contains(6), true);
  ASSERT_NE(set.contains(1), true);

  set.remove(4);
  ASSERT_NE(set.contains(4), true);
  set.remove(2);
  set.remove(6);
  ASSERT_NE(set.contains(2), true);
  ASSERT_NE(set.contains(6), true);

  set.try_emplace(2);
  ASSERT_EQ(set.contains(2), true);
}
