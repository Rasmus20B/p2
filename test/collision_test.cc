
import collision;
#include <raylib.h>
#include <gtest/gtest.h>

#include <chrono>

TEST(collision, getCorners) {
  Rectangle r = { 200, 200, 300, 400 };

  std::array<Vector2, 4> res1 = getCorners(r, 0.f);
  // std::array<Vec2, 4> res2 = getCornersSIMD(r, 0.f);

  // for(int i = 0; i < res1.size(); ++i) {
  //   EXPECT_EQ(res1[i], res2[i]);
  // }
}

TEST(collision, checkContains) {
  auto cs1 = getCorners({
      200, 200, 300, 400
      }, 0);
  auto cs2 = getCorners({
      300, 200, 300, 400
      }, 0);

  auto res1 = checkContains(cs1, cs2);
  // auto res2 = checkContainsSIMD(cs1, cs2);

  // EXPECT_EQ(res1, res2);
}
