#include <iostream>
#include "nano/geometry.h"
#include "nano/test.h"

namespace {
// clang-format off
TEST_CASE("nano.geometry", RectConversion, "Rect conversion"){

  EXPECT_TRUE((std::is_constructible_v<nano::rect<float>, nano::rect<float>>));
  EXPECT_TRUE((std::is_constructible_v<nano::rect<float>, nano::rect<double>>));
  EXPECT_TRUE((std::is_constructible_v<nano::rect<float>, nano::rect<int>>));
  EXPECT_TRUE((std::is_constructible_v<nano::rect<float>, nano::rect<unsigned int>>));

  EXPECT_TRUE((std::is_constructible_v<nano::rect<int>, nano::rect<float>>));
  EXPECT_TRUE((std::is_constructible_v<nano::rect<int>, nano::rect<double>>));
  EXPECT_TRUE((std::is_constructible_v<nano::rect<int>, nano::rect<int>>));
  EXPECT_TRUE((std::is_constructible_v<nano::rect<int>, nano::rect<unsigned int>>));

  EXPECT_TRUE((std::is_constructible_v<nano::rect<unsigned int>, nano::rect<float>>));
  EXPECT_TRUE((std::is_constructible_v<nano::rect<unsigned int>, nano::rect<double>>));
  EXPECT_TRUE((std::is_constructible_v<nano::rect<unsigned int>, nano::rect<int>>));
  EXPECT_TRUE((std::is_constructible_v<nano::rect<unsigned int>, nano::rect<unsigned int>>));

  EXPECT_FALSE((std::is_constructible_v<nano::rect<float>, nano::point<float>>));
  EXPECT_FALSE((std::is_constructible_v<nano::rect<float>, nano::size<double>>));

  #define IS_CONSTRUCTIBLE(RectType, Type) (std::is_constructible_v<nano::rect<Type>, RectType>)
  #define IS_VALID_RECT(RectType) EXPECT_TRUE(IS_CONSTRUCTIBLE(RectType, float) && IS_CONSTRUCTIBLE(RectType, int))
  #define IS_INVALID_RECT(RectType) EXPECT_FALSE(IS_CONSTRUCTIBLE(RectType, float) || IS_CONSTRUCTIBLE(RectType, int))

  //
  // float
  //

  struct valid_os_f { struct Point { float x, y; } origin; struct Size { float width, height; } size; };
  IS_VALID_RECT(valid_os_f);

  struct wrong_os_f { struct Point { float y, x; } origin; struct Size { float width, height; } size; };
  IS_INVALID_RECT(wrong_os_f);

  struct valid_xywh_f { float x, y, width, height; };
  IS_VALID_RECT(valid_xywh_f);

  struct wrong_xywh_f { float y, x, width, height; };
  IS_INVALID_RECT(wrong_xywh_f);

  struct valid_XYWH_f { float X, Y, Width, Height; };
  IS_VALID_RECT(valid_XYWH_f);

  struct wrong_XYWH_f { float Y, X, Width, Height; };
  IS_INVALID_RECT(wrong_XYWH_f);

  struct valid_ltrb_f { float left, top, right, bottom; };
  IS_VALID_RECT(valid_ltrb_f);

  struct wrong_ltrb_1_f { float left, top, bottom, right; };
  IS_INVALID_RECT(wrong_ltrb_1_f);

  struct wrong_ltrb_2_f { float left, bottom, top, right; };
  IS_INVALID_RECT(wrong_ltrb_2_f);

  //
  // int
  //

  struct valid_os_i { struct Point { int x, y; } origin; struct Size { int width, height; } size; };
  IS_VALID_RECT(valid_os_i);

  struct wrong_os_i { struct Point { int y, x; } origin; struct Size { int width, height; } size; };
  IS_INVALID_RECT(wrong_os_i);

  struct valid_xywh_i { int x, y, width, height; };
  IS_VALID_RECT(valid_xywh_i);

  struct wrong_xywh_i { int y, x, width, height; };
  IS_INVALID_RECT(wrong_xywh_i);

  struct valid_XYWH_i { int X, Y, Width, Height; };
  IS_VALID_RECT(valid_XYWH_i);

  struct wrong_XYWH_i { int Y, X, Width, Height; };
  IS_INVALID_RECT(wrong_XYWH_i);

  struct valid_ltrb_i { int left, top, right, bottom; };
  IS_VALID_RECT(valid_ltrb_i);

  struct wrong_ltrb_1_i { int left, top, bottom, right; };
  IS_INVALID_RECT(wrong_ltrb_1_i);

  struct wrong_ltrb_2_i { int left, bottom, top, right; };
  IS_INVALID_RECT(wrong_ltrb_2_i);

  #undef IS_INVALID_RECT
  #undef IS_VALID_RECT
  #undef IS_CONSTRUCTIBLE
}
// clang-format on

TEST_CASE("nano.geometry", PointConversion, "Point conversion") {

  EXPECT_TRUE((std::is_constructible_v<nano::point<float>, nano::point<float>>));
  EXPECT_TRUE((std::is_constructible_v<nano::point<float>, nano::point<double>>));
  EXPECT_TRUE((std::is_constructible_v<nano::point<float>, nano::point<int>>));
  EXPECT_TRUE((std::is_constructible_v<nano::point<float>, nano::point<unsigned int>>));

  EXPECT_TRUE((std::is_constructible_v<nano::point<int>, nano::point<float>>));
  EXPECT_TRUE((std::is_constructible_v<nano::point<int>, nano::point<double>>));
  EXPECT_TRUE((std::is_constructible_v<nano::point<int>, nano::point<int>>));
  EXPECT_TRUE((std::is_constructible_v<nano::point<int>, nano::point<unsigned int>>));

  EXPECT_TRUE((std::is_constructible_v<nano::point<unsigned int>, nano::point<float>>));
  EXPECT_TRUE((std::is_constructible_v<nano::point<unsigned int>, nano::point<double>>));
  EXPECT_TRUE((std::is_constructible_v<nano::point<unsigned int>, nano::point<int>>));
  EXPECT_TRUE((std::is_constructible_v<nano::point<unsigned int>, nano::point<unsigned int>>));

  EXPECT_FALSE((std::is_constructible_v<nano::point<float>, nano::rect<float>>));
  EXPECT_FALSE((std::is_constructible_v<nano::point<float>, nano::size<float>>));

  struct valid_xy {
    float x, y;
  };
  EXPECT_TRUE((std::is_constructible_v<nano::point<float>, valid_xy>));

  struct wrong_xy {
    float y, x;
  };
  EXPECT_FALSE((std::is_constructible_v<nano::point<float>, wrong_xy>));

  struct valid_XY {
    float X, Y;
  };
  EXPECT_TRUE((std::is_constructible_v<nano::point<float>, valid_XY>));

  struct wrong_XY {
    float Y, X;
  };
  EXPECT_FALSE((std::is_constructible_v<nano::point<float>, wrong_XY>));
}

TEST_CASE("nano.geometry", SizeConversion, "Size conversion") {

  EXPECT_TRUE((std::is_constructible_v<nano::size<float>, nano::size<float>>));
  EXPECT_TRUE((std::is_constructible_v<nano::size<float>, nano::size<double>>));
  EXPECT_TRUE((std::is_constructible_v<nano::size<float>, nano::size<int>>));
  EXPECT_TRUE((std::is_constructible_v<nano::size<float>, nano::size<unsigned int>>));

  EXPECT_TRUE((std::is_constructible_v<nano::size<int>, nano::size<float>>));
  EXPECT_TRUE((std::is_constructible_v<nano::size<int>, nano::size<double>>));
  EXPECT_TRUE((std::is_constructible_v<nano::size<int>, nano::size<int>>));
  EXPECT_TRUE((std::is_constructible_v<nano::size<int>, nano::size<unsigned int>>));

  EXPECT_TRUE((std::is_constructible_v<nano::size<unsigned int>, nano::size<float>>));
  EXPECT_TRUE((std::is_constructible_v<nano::size<unsigned int>, nano::size<double>>));
  EXPECT_TRUE((std::is_constructible_v<nano::size<unsigned int>, nano::size<int>>));
  EXPECT_TRUE((std::is_constructible_v<nano::size<unsigned int>, nano::size<unsigned int>>));

  EXPECT_FALSE((std::is_constructible_v<nano::size<float>, nano::rect<float>>));
  EXPECT_FALSE((std::is_constructible_v<nano::size<float>, nano::point<float>>));

  struct valid_wh {
    float width, height;
  };
  EXPECT_TRUE((std::is_constructible_v<nano::size<float>, valid_wh>));

  struct wrong_wh {
    float height, width;
  };
  EXPECT_FALSE((std::is_constructible_v<nano::size<float>, wrong_wh>));

  struct valid_WH {
    float Width, Height;
  };
  EXPECT_TRUE((std::is_constructible_v<nano::size<float>, valid_WH>));

  struct wrong_WH {
    float Height, Width;
  };
  EXPECT_FALSE((std::is_constructible_v<nano::size<float>, wrong_WH>));
}

TEST_CASE("nano.geometry", Transform, "Transform") {
  {
    nano::rect<float> r = { 0, 0, 20, 20 };
    nano::transform<float> f = nano::transform<float>::translation({ 10, 10 });

    EXPECT_EQ(r.position * f, nano::point<float>(10, 10));

    f += { 5.0f, 6.0f };
    EXPECT_EQ(r.position * f, nano::point<float>(15, 16));
  }

  {
    nano::rect<float> r = { 5, 6, 20, 20 };
    nano::transform<float> f = nano::transform<float>::translation({ 10, 20 });
    nano::point<float> pt = r.position * f;
    EXPECT_EQ(pt, nano::point<float>(15, 26));
  }

  {
    nano::rect<float> r = { 5, 6, 10, 20 };
    nano::transform<float> f = nano::transform<float>::scale({ 2, 3 });
    EXPECT_EQ(r.position * f, nano::point<float>(10, 18));
  }

  {
    nano::rect<float> r = { 5, 6, 10, 20 };
    nano::transform<float> f
        = nano::transform<float>::translation({ 10, 20 }) * nano::transform<float>::scale({ 2, 3 });
    EXPECT_EQ(r.position * f, nano::point<float>(5 * 2 + 10, 6 * 3 + 20));
  }

  {
    nano::rect<float> r = { 5, 6, 10, 20 };
    nano::transform<float> f
        = nano::transform<float>::scale({ 2, 3 }) * nano::transform<float>::translation({ 10, 20 });
    EXPECT_EQ(r.position * f, nano::point<float>((5 + 10) * 2, (6 + 20) * 3));

    f.translated({ 5, 7 });
    EXPECT_EQ(r.position * f, nano::point<float>((5 + 10 + 5) * 2, (6 + 20 + 7) * 3));
  }

  {
    nano::rect<float> r = { 5, 6, 10, 20 };
    nano::transform<float> f
        = nano::transform<float>::scale({ 2, 3 }) * nano::transform<float>::translation({ 10, 20 });
    EXPECT_EQ(r.position * f, nano::point<float>((5 + 10) * 2, (6 + 20) * 3));

    f += { 5.0f, 7.0f };
    EXPECT_EQ(r.position * f, nano::point<float>((5 + 10 + 5) * 2, (6 + 20 + 7) * 3));
  }

  {
    nano::rect<double> r = { 0, 0, 10, 10 };
    nano::transform<double> f = nano::transform<double>::rotation(M_PI, r.bottom_right());
    nano::point<double> tl = r.position * f;
    nano::point<double> tr = r.top_right() * f;
    nano::point<double> bl = r.bottom_left() * f;
    nano::point<double> br = r.bottom_right() * f;
    EXPECT_EQ(tl, nano::point<double>(20, 20));
    EXPECT_EQ(tr, nano::point<double>(10, 20));
    EXPECT_EQ(bl, nano::point<double>(20, 10));
    EXPECT_EQ(br, r.bottom_right());
  }

  {
    nano::rect<double> r = { 0, 0, 10, 10 };
    nano::transform<double> f = nano::transform<double>::rotation(M_PI, r.bottom_right());
    nano::quad<double> q = r * f;
    EXPECT_EQ(q.top_left, nano::point<double>(20, 20));
    EXPECT_EQ(q.top_right, nano::point<double>(10, 20));
    EXPECT_EQ(q.bottom_left, nano::point<double>(20, 10));
    EXPECT_EQ(q.bottom_right, r.bottom_right());
  }

  {
    nano::rect<double> r = { 0, 0, 10, 10 };
    nano::quad<double> q = r;
    nano::transform<double> f = nano::transform<double>::rotation(M_PI, r.bottom_right());
    nano::quad<double> tq = q * f;
    EXPECT_EQ(tq.top_left, nano::point<double>(20, 20));
    EXPECT_EQ(tq.top_right, nano::point<double>(10, 20));
    EXPECT_EQ(tq.bottom_left, nano::point<double>(20, 10));
    EXPECT_EQ(tq.bottom_right, r.bottom_right());
  }
}
} // namespace.

NANO_TEST_MAIN()

// int main() {
//   nano::range<float> r = {0.02f, 1.0f};
//   std::cout << r << std::endl;
//   return 0;
// }
