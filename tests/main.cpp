#include <iostream>
#include "nano/geometry.h"
#include "nano/test.h"

namespace {

TEST_CASE("nano.geometry", Expect, "Expects") {
  nano::rect<float> a = {1, 2, 3, 4};
  EXPECT_EQ(a.x, 1.0f);
  
  nano::point<int> pt = {1, 2};
  
  nano::range<float> r = {0.02f, 1.0f};
  std::cout << r << std::endl;
}
} // namespace.

NANO_TEST_MAIN()

//int main() {
//  nano::range<float> r = {0.02f, 1.0f};
//  std::cout << r << std::endl;
//  return 0;
//}
