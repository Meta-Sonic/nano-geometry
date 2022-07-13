/*
 * Nano Library
 *
 * Copyright (C) 2022, Meta-Sonic
 * All rights reserved.
 *
 * Proprietary and confidential.
 * Any unauthorized copying, alteration, distribution, transmission, performance,
 * display or other use of this material is strictly prohibited.
 *
 * Written by Alexandre Arsenault <alx.arsenault@gmail.com>
 */

#pragma once

/*!
 * @file      nano.h
 * @brief     nano core
 * @copyright Copyright (C) 2022, Meta-Sonic
 * @author    Alexandre Arsenault alx.arsenault@gmail.com
 * @date      Created 16/06/2022
 */

#include <algorithm>
#include <cmath>
#include <limits>
#include <ostream>
#include <type_traits>
#include <utility>

//
// MARK: - Macros -
//

#define NANO_INLINE inline

#define NANO_CXPR constexpr

#define NANO_NOEXCEPT noexcept

#define NANO_NODISCARD [[nodiscard]]

/// @def NANO_INLINE_CXPR
#define NANO_INLINE_CXPR NANO_INLINE NANO_CXPR

/// @def NANO_NODC_INLINE
#define NANO_NODC_INLINE NANO_NODISCARD inline

/// @def NANO_NODC_INLINE_CXPR
#define NANO_NODC_INLINE_CXPR NANO_NODISCARD NANO_INLINE_CXPR

#define NANO_STRINGIFY(X) NANO_STR(X)
#define NANO_STR(X) #X

#ifdef _MSC_VER
  #define NANO_MSVC_PRAGMA(X) __pragma(X)
#else
  #define NANO_MSVC_PRAGMA(X)
#endif

#ifdef __clang__
  #define NANO_CLANG_PRAGMA(X) _Pragma(X)
#else
  #define NANO_CLANG_PRAGMA(X)
#endif

#define NANO_MSVC_DIAGNOSTIC_PUSH() NANO_MSVC_PRAGMA(warning(push))
#define NANO_MSVC_DIAGNOSTIC_POP() NANO_MSVC_PRAGMA(warning(pop))
#define NANO_MSVC_PUSH_WARNING(X) NANO_MSVC_DIAGNOSTIC_PUSH() NANO_MSVC_PRAGMA(warning(disable : X))
#define NANO_MSVC_POP_WARNING() NANO_MSVC_DIAGNOSTIC_POP()

#define NANO_CLANG_DIAGNOSTIC_PUSH() NANO_CLANG_PRAGMA("clang diagnostic push")
#define NANO_CLANG_DIAGNOSTIC_POP() NANO_CLANG_PRAGMA("clang diagnostic pop")
#define NANO_CLANG_DIAGNOSTIC(TYPE, X) NANO_CLANG_PRAGMA(NANO_STRINGIFY(clang diagnostic TYPE X))

#define NANO_CLANG_PUSH_WARNING(X)                                                                                     \
  NANO_CLANG_DIAGNOSTIC_PUSH() NANO_CLANG_PRAGMA(NANO_STRINGIFY(clang diagnostic ignored X))

#define NANO_CLANG_POP_WARNING() NANO_CLANG_DIAGNOSTIC_POP()

#define NANO_USING_TYPE(x)                                                                                             \
  template <class T>                                                                                                   \
  using x = decltype(T::x)

NANO_CLANG_DIAGNOSTIC_PUSH()
NANO_CLANG_DIAGNOSTIC(warning, "-Weverything")
NANO_CLANG_DIAGNOSTIC(ignored, "-Wc++98-compat")

namespace nano {

///
template <typename T>
struct range;

///
template <typename T>
struct rect;

//
// MARK: - Traits -
//

///
struct nonesuch {
  nonesuch(const nonesuch&) = delete;
  ~nonesuch() = delete;
  void operator=(const nonesuch&) = delete;
};

template <class Default, class AlwaysVoid, template <class...> class Op, class... Args>
struct detector_value : std::false_type {};

///
template <class Default, template <class...> class Op, class... Args>
struct detector_value<Default, std::void_t<Op<Args...>>, Op, Args...> : std::true_type {};

template <class T, template <class...> class... Ops>
using has_members = std::conjunction<detector_value<nonesuch, void, Ops, T>...>;

template <class T, template <class...> class... Ops>
using enable_if_has_members_t = std::enable_if_t<has_members<T, Ops...>::value, std::nullptr_t>;

template <class T, template <class...> class... Ops>
using if_members = enable_if_has_members_t<T, Ops...>;

namespace meta {
  NANO_USING_TYPE(x);
  NANO_USING_TYPE(y);
  NANO_USING_TYPE(width);
  NANO_USING_TYPE(height);
  NANO_USING_TYPE(X);
  NANO_USING_TYPE(Y);
  NANO_USING_TYPE(Width);
  NANO_USING_TYPE(Height);
  NANO_USING_TYPE(origin);
  NANO_USING_TYPE(size);
  NANO_USING_TYPE(left);
  NANO_USING_TYPE(right);
  NANO_USING_TYPE(top);
  NANO_USING_TYPE(bottom);
  NANO_USING_TYPE(start);
  NANO_USING_TYPE(end);
} // namespace meta.

template <typename T1, typename T2, std::enable_if_t<std::is_floating_point_v<std::common_type_t<T1, T2>>, int> = 0>
NANO_NODC_INLINE_CXPR bool fcompare(T1 a, T2 b) noexcept {
  using T = std::common_type_t<T1, T2>;
  return std::abs(static_cast<T>(a) - static_cast<T>(b)) < std::numeric_limits<T>::epsilon();
}

//
// MARK: - Geometry -
//

/// A general-purpose range object, that simply represents any linear range with
/// a start and end value.
template <typename T>
struct range {
  using value_type = T;
  value_type start, end;

  /// No custom default/copy/move constructor, copy/move assignment and destructor
  /// to remain a trivial type.
  range() NANO_NOEXCEPT = default;
  range(range&&) NANO_NOEXCEPT = default;
  range(const range&) NANO_NOEXCEPT = default;
  ~range() NANO_NOEXCEPT = default;
  range& operator=(range&&) NANO_NOEXCEPT = default;
  range& operator=(const range&) NANO_NOEXCEPT = default;

  /// Constructor.
  NANO_INLINE_CXPR range(value_type _start, value_type _end) NANO_NOEXCEPT;

  /// Creates a range with and start and length.
  NANO_NODC_INLINE_CXPR static range with_length(value_type start, value_type len) NANO_NOEXCEPT;

  /// Create a new range with given start position.
  ///
  /// @see set_start
  NANO_NODC_INLINE_CXPR range with_start(value_type s) const NANO_NOEXCEPT;

  /// Create a new range with given end position.
  ///
  /// @see set_end
  NANO_NODC_INLINE_CXPR range with_end(value_type e) const NANO_NOEXCEPT;

  /// Create a new range with shifted start position by delta.
  ///
  /// @see shift_start
  NANO_NODC_INLINE_CXPR range with_shifted_start(value_type delta) const NANO_NOEXCEPT;

  /// Create a new range with shifted end position by delta.
  ///
  /// @see shift_end
  NANO_NODC_INLINE_CXPR range with_shifted_end(value_type delta) const NANO_NOEXCEPT;

  /// Create a new range with given length.
  ///
  /// @see set_length
  NANO_NODC_INLINE_CXPR range with_length(value_type len) const NANO_NOEXCEPT;

  /// Create a new range with shifted start position by delta but keeping length.
  ///
  /// @see shift
  NANO_NODC_INLINE_CXPR range with_shift(value_type delta) const NANO_NOEXCEPT;

  /// Create a new range with new start position but keeping length.
  ///
  /// @see move_to
  NANO_NODC_INLINE_CXPR range with_move(value_type s) const NANO_NOEXCEPT;

  /// Moves to new start position.
  NANO_INLINE_CXPR range& set_start(value_type s) NANO_NOEXCEPT;

  /// Moves to new end position .
  NANO_INLINE_CXPR range& set_end(value_type e) NANO_NOEXCEPT;

  /// Moves to new start position and keep current length.
  NANO_INLINE_CXPR range& move_to(value_type s) NANO_NOEXCEPT;

  /// Moves start position by delta and keep current length.
  NANO_INLINE_CXPR range& shift(value_type delta) NANO_NOEXCEPT;

  /// Moves start position by delta.
  NANO_INLINE_CXPR range& shift_start(value_type delta) NANO_NOEXCEPT;

  /// Moves start position by delta.
  NANO_INLINE_CXPR range& shift_end(value_type delta) NANO_NOEXCEPT;

  /// Changes the length of the range.
  NANO_INLINE_CXPR range& set_length(value_type len) NANO_NOEXCEPT;

  /// Returns the length of the range.
  NANO_NODC_INLINE_CXPR value_type length() const NANO_NOEXCEPT;

  /// Returns the middle of the range.
  NANO_NODC_INLINE_CXPR value_type middle() const NANO_NOEXCEPT;

  /// Returns true if the start side of range is smaller than the end side.
  NANO_NODC_INLINE_CXPR bool is_sorted() const NANO_NOEXCEPT;

  /// Returns true if the range is symmetric (i.e. start == -end).
  NANO_NODC_INLINE_CXPR bool is_symmetric() const NANO_NOEXCEPT;

  /// Returns true if the given position lies inside this range.
  NANO_NODC_INLINE_CXPR bool contains(value_type x) const NANO_NOEXCEPT;

  /// Same as contains(x).
  NANO_NODC_INLINE_CXPR bool contains_closed(value_type x) const NANO_NOEXCEPT;

  /// Returns true if the given position lies inside ]start, end[
  NANO_NODC_INLINE_CXPR bool contains_opened(value_type x) const NANO_NOEXCEPT;

  /// Returns true if the given position lies inside ]start, end]
  NANO_NODC_INLINE_CXPR bool contains_left_opened(value_type x) const NANO_NOEXCEPT;

  /// Returns true if the given position lies inside [start, end[
  NANO_NODC_INLINE_CXPR bool contains_right_opened(value_type x) const NANO_NOEXCEPT;

  /// Returns true if the given range lies entirely inside this range.
  NANO_NODC_INLINE_CXPR bool contains(const range& r) const NANO_NOEXCEPT;

  /// Returns the nearest value to the one supplied, which lies within the range.
  NANO_NODC_INLINE_CXPR value_type clipped_value(value_type x) const NANO_NOEXCEPT;

  /// Swaps the value of start and end if the range is not sorted.
  ///
  /// @see is_sorted.
  NANO_INLINE_CXPR void sort() NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool operator==(const range& r) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool operator!=(const range& r) const NANO_NOEXCEPT;
};

static_assert(std::is_trivial<range<int>>::value, "nano::range must remain a trivial type");
static_assert(std::is_trivial<range<float>>::value, "nano::range must remain a trivial type");

///
template <typename T>
struct padding {
  using value_type = T;
  value_type top;
  value_type left;
  value_type bottom;
  value_type right;

  padding() NANO_NOEXCEPT = default;
  padding(const padding&) NANO_NOEXCEPT = default;
  padding(padding&&) NANO_NOEXCEPT = default;

  ~padding() NANO_NOEXCEPT = default;

  padding& operator=(const padding&) NANO_NOEXCEPT = default;
  padding& operator=(padding&&) NANO_NOEXCEPT = default;

  NANO_INLINE_CXPR padding(value_type t, value_type l, value_type b, value_type r) NANO_NOEXCEPT;

  NANO_INLINE_CXPR padding(value_type p) NANO_NOEXCEPT;

  NANO_NODC_INLINE nano::rect<value_type> inside_rect(const nano::rect<value_type>& rect) const NANO_NOEXCEPT;

  NANO_NODC_INLINE nano::rect<value_type> outside_rect(const nano::rect<value_type>& rect) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool empty() const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool operator==(const padding& p) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool operator!=(const padding& p) const NANO_NOEXCEPT;
};

///
template <typename T>
struct point {
  static_assert(std::is_arithmetic<T>::value, "point value_type must be arithmetic");

  using value_type = T;
  value_type x, y;

  point() NANO_NOEXCEPT = default;
  point(const point&) NANO_NOEXCEPT = default;
  point(point&&) NANO_NOEXCEPT = default;

  NANO_INLINE_CXPR point(value_type X, value_type Y) NANO_NOEXCEPT : x(X), y(Y) {}

  /// Construct a Point from PointType with member x and y.
  template <typename PointType, if_members<PointType, meta::x, meta::y> = nullptr>
  NANO_INLINE_CXPR point(const PointType& point) NANO_NOEXCEPT;

  /// Construct a Point from PointType with member X and Y.
  template <typename PointType, if_members<PointType, meta::X, meta::Y> = nullptr>
  NANO_INLINE_CXPR point(const PointType& point) NANO_NOEXCEPT;

  ~point() NANO_NOEXCEPT = default;

  point& operator=(const point&) NANO_NOEXCEPT = default;
  point& operator=(point&&) NANO_NOEXCEPT = default;

  NANO_INLINE_CXPR point& set_x(value_type _x) NANO_NOEXCEPT;
  NANO_INLINE_CXPR point& set_y(value_type _y) NANO_NOEXCEPT;

  NANO_INLINE_CXPR point& add_x(value_type dx) NANO_NOEXCEPT;
  NANO_INLINE_CXPR point& add_y(value_type dy) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR point with_x(value_type _x) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point with_y(value_type _y) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR point with_add_x(value_type dx) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point with_add_y(value_type dy) const NANO_NOEXCEPT;

  NANO_INLINE_CXPR point& operator+=(value_type v) NANO_NOEXCEPT;
  NANO_INLINE_CXPR point& operator-=(value_type v) NANO_NOEXCEPT;
  NANO_INLINE_CXPR point& operator*=(value_type v) NANO_NOEXCEPT;
  NANO_INLINE_CXPR point& operator/=(value_type v) NANO_NOEXCEPT;

  NANO_INLINE_CXPR point& operator+=(const point& pt) NANO_NOEXCEPT;
  NANO_INLINE_CXPR point& operator-=(const point& pt) NANO_NOEXCEPT;
  NANO_INLINE_CXPR point& operator*=(const point& pt) NANO_NOEXCEPT;
  NANO_INLINE_CXPR point& operator/=(const point& pt) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR point operator+(value_type v) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point operator-(value_type v) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point operator*(value_type v) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point operator/(value_type v) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR point operator+(const point& pt) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point operator-(const point& pt) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point operator*(const point& pt) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point operator/(const point& pt) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool operator==(const point& pt) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR bool operator!=(const point& pt) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool operator<(const point& pt) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR bool operator<=(const point& pt) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR bool operator>(const point& pt) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR bool operator>=(const point& pt) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR point operator-() const NANO_NOEXCEPT;

  /// Conversion operator to PointType with member x and y.
  template <typename PointType, if_members<PointType, meta::x, meta::y> = nullptr>
  NANO_NODC_INLINE PointType convert() const;

  /// Conversion operator to PointType with member X and Y.
  template <typename PointType, if_members<PointType, meta::X, meta::Y> = nullptr>
  NANO_NODC_INLINE PointType convert() const;

  /// Conversion operator to PointType with member x and y.
  template <typename PointType, if_members<PointType, meta::x, meta::y> = nullptr>
  NANO_NODC_INLINE explicit operator PointType() const;

  /// Conversion operator to PointType with member X and Y.
  template <typename PointType, if_members<PointType, meta::X, meta::Y> = nullptr>
  NANO_NODC_INLINE explicit operator PointType() const;

  template <typename U>
  NANO_INLINE friend std::ostream& operator<<(std::ostream& s, const nano::point<U>& point);
};

static_assert(std::is_trivial<point<int>>::value, "nano::point must remain a trivial type");
static_assert(std::is_trivial<point<float>>::value, "nano::point must remain a trivial type");

///
template <typename T>
struct size {
  static_assert(std::is_arithmetic<T>::value, "size value_type must be arithmetic");

  using value_type = T;
  value_type width, height;

  size() NANO_NOEXCEPT = default;
  size(const size&) NANO_NOEXCEPT = default;
  size(size&&) NANO_NOEXCEPT = default;

  NANO_INLINE_CXPR size(value_type W, value_type H) NANO_NOEXCEPT;

  ///
  template <typename SizeType, if_members<SizeType, meta::width, meta::height> = nullptr>
  NANO_INLINE_CXPR size(const SizeType& s) NANO_NOEXCEPT;

  ///
  template <typename SizeType, if_members<SizeType, meta::Width, meta::Height> = nullptr>
  NANO_INLINE_CXPR size(const SizeType& s) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR static size zero();
  NANO_NODC_INLINE_CXPR static size full_scale();

  ~size() NANO_NOEXCEPT = default;

  size& operator=(const size&) NANO_NOEXCEPT = default;
  size& operator=(size&&) NANO_NOEXCEPT = default;

  NANO_INLINE_CXPR size& set_width(value_type w) NANO_NOEXCEPT;
  NANO_INLINE_CXPR size& set_height(value_type h) NANO_NOEXCEPT;
  NANO_INLINE_CXPR size& add_width(value_type dw) NANO_NOEXCEPT;
  NANO_INLINE_CXPR size& add_height(value_type dh) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR size with_width(value_type w) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR size with_height(value_type h) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR size with_add_width(value_type dw) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR size with_add_height(value_type dh) const NANO_NOEXCEPT;

  NANO_INLINE_CXPR size& operator+=(value_type v) NANO_NOEXCEPT;
  NANO_INLINE_CXPR size& operator-=(value_type v) NANO_NOEXCEPT;
  NANO_INLINE_CXPR size& operator*=(value_type v) NANO_NOEXCEPT;
  NANO_INLINE_CXPR size& operator/=(value_type v) NANO_NOEXCEPT;

  NANO_INLINE_CXPR size& operator+=(const size& s) NANO_NOEXCEPT;
  NANO_INLINE_CXPR size& operator-=(const size& s) NANO_NOEXCEPT;
  NANO_INLINE_CXPR size& operator*=(const size& s) NANO_NOEXCEPT;
  NANO_INLINE_CXPR size& operator/=(const size& s) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR size operator+(value_type v) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR size operator-(value_type v) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR size operator*(value_type v) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR size operator/(value_type v) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR size operator+(const size& s) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR size operator-(const size& s) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR size operator*(const size& s) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR size operator/(const size& s) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool operator==(const size& s) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR bool operator!=(const size& s) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool operator<(const size& s) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR bool operator<=(const size& s) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR bool operator>(const size& s) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR bool operator>=(const size& s) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR size operator-() const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool empty() const NANO_NOEXCEPT;

  template <typename SizeType, if_members<SizeType, meta::width, meta::height> = nullptr>
  NANO_NODC_INLINE SizeType convert() const;

  template <typename SizeType, if_members<SizeType, meta::Width, meta::Height> = nullptr>
  NANO_NODC_INLINE SizeType convert() const;

  template <typename SizeType, if_members<SizeType, meta::width, meta::height> = nullptr>
  NANO_NODC_INLINE explicit operator SizeType() const;

  template <typename SizeType, if_members<SizeType, meta::Width, meta::Height> = nullptr>
  NANO_NODC_INLINE explicit operator SizeType() const;

  template <typename U>
  NANO_INLINE friend std::ostream& operator<<(std::ostream& s, const nano::size<U>& size);
};

static_assert(std::is_trivial<size<int>>::value, "nano::size must remain a trivial type");
static_assert(std::is_trivial<size<float>>::value, "nano::size must remain a trivial type");

namespace detail {
  template <class RectType>
  using enable_if_rect_xywh = std::enable_if_t<has_members<RectType, meta::x, meta::y, meta::width, meta::height>::value
          && !has_members<RectType, meta::origin, meta::size>::value,
      std::nullptr_t>;
} // namespace detail.

///
template <typename T>
struct rect {
  static_assert(std::is_arithmetic<T>::value, "rect value_type must be arithmetic");

  using value_type = T;
  using point_type = nano::point<value_type>;
  using size_type = nano::size<value_type>;

  NANO_CLANG_DIAGNOSTIC_PUSH()
  NANO_CLANG_DIAGNOSTIC(ignored, "-Wnested-anon-types")
  NANO_CLANG_DIAGNOSTIC(ignored, "-Wgnu-anonymous-struct")

  union {
    point_type origin;
    point_type position;
    struct {
      value_type x, y;
    };
  };

  union {
    size_type size;
    struct {
      value_type width, height;
    };
  };

  NANO_CLANG_DIAGNOSTIC_POP()

  rect() NANO_NOEXCEPT = default;
  rect(const rect&) NANO_NOEXCEPT = default;
  rect(rect&&) NANO_NOEXCEPT = default;

  NANO_INLINE_CXPR rect(const point_type& p, const size_type& s) NANO_NOEXCEPT : origin(p), size(s) {}

  NANO_INLINE_CXPR rect(value_type x, value_type y, value_type w, value_type h) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect(value_type x, value_type y, const size_type& s) NANO_NOEXCEPT;

  template <typename RectType, if_members<RectType, meta::origin, meta::size> = nullptr>
  NANO_INLINE_CXPR rect(const RectType& rect) NANO_NOEXCEPT;

  template <typename RectType, if_members<RectType, meta::X, meta::Y, meta::Width, meta::Height> = nullptr>
  NANO_INLINE_CXPR rect(const RectType& rect) NANO_NOEXCEPT;

  template <typename RectType, if_members<RectType, meta::left, meta::top, meta::right, meta::bottom> = nullptr>
  NANO_INLINE_CXPR rect(const RectType& rect) NANO_NOEXCEPT;

  template <typename RectType, detail::enable_if_rect_xywh<RectType> = nullptr>
  NANO_INLINE_CXPR rect(const RectType& rect) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR static rect create_from_point(
      const point_type& topLeft, const point_type& bottomRight) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR static rect create_from_bottom_left(
      value_type x, value_type y, value_type w, value_type h) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR static rect create_from_bottom_left(const point_type& p, const size_type& s) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR static rect create_from_bottom_right(
      value_type x, value_type y, value_type w, value_type h) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR static rect create_from_bottom_right(const point_type& p, const size_type& s) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR static rect create_from_top_left(
      value_type x, value_type y, value_type w, value_type h) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR static rect create_from_top_left(const point_type& p, const size_type& s) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR static rect create_from_top_right(
      value_type x, value_type y, value_type w, value_type h) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR static rect create_from_top_right(const point_type& p, const size_type& s) NANO_NOEXCEPT;

  ~rect() NANO_NOEXCEPT = default;

  rect& operator=(const rect&) NANO_NOEXCEPT = default;
  rect& operator=(rect&&) NANO_NOEXCEPT = default;

  NANO_INLINE_CXPR rect& set_x(value_type _x) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& set_y(value_type _y) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& set_width(value_type w) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& set_height(value_type h) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& set_position(const point_type& point) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& set_size(const size_type& s) NANO_NOEXCEPT;

  NANO_INLINE_CXPR rect& add_x(value_type _x) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& add_y(value_type _y) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& add_width(value_type w) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& add_height(value_type h) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& add_point(const point_type& point) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& add_size(const size_type& s) NANO_NOEXCEPT;

  NANO_INLINE_CXPR rect& mul_x(value_type _x) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& mul_y(value_type _y) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& mul_width(value_type w) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& mul_height(value_type h) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR rect with_x(value_type _x) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_y(value_type _y) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_width(value_type w) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_height(value_type h) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_position(const point_type& point) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_size(const size_type& s) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR rect with_top_left(const point_type& point) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_top_right(const point_type& point) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_bottom_left(const point_type& point) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_bottom_right(const point_type& point) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_middle(const point_type& point) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_middle_left(const point_type& point) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_middle_right(const point_type& point) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_middle_top(const point_type& point) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect with_middle_bottom(const point_type& point) const NANO_NOEXCEPT;

  NANO_INLINE_CXPR rect& operator+=(const point_type& pos) NANO_NOEXCEPT;
  NANO_INLINE_CXPR rect& operator-=(const point_type& pos) NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR rect operator+(const point_type& pos) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect operator-(const point_type& pos) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR value_type left() const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR value_type right() const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR value_type top() const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR value_type bottom() const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR point_type top_left() const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type top_right() const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type top_right(value_type dx, value_type dy) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type bottom_left() const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type bottom_right() const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type middle() const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type middle_left() const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type middle_right() const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type middle_top() const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type middle_bottom() const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type next_left(value_type delta) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type next_left(const point_type& dt) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type next_right(value_type delta) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type next_right(const point_type& dt) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type next_down(value_type delta) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type next_down(const point_type& dt) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type next_up(value_type delta) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR point_type next_up(const point_type& dt) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool operator==(const rect& r) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR bool operator!=(const rect& r) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool contains(const point_type& pos) const NANO_NOEXCEPT;

  NANO_INLINE_CXPR rect<T>& reduce(const point_type& pt) NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect reduced(const point_type& pt) const NANO_NOEXCEPT;

  NANO_INLINE_CXPR rect<T>& expand(const point_type& pt) NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect expanded(const point_type& pt) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR bool intersects(const rect& r) const NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR bool intersects(const point_type& p) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR value_type area() const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR rect get_union(const rect& rhs) const NANO_NOEXCEPT;

  NANO_INLINE_CXPR rect<T>& merge(const rect& rhs) NANO_NOEXCEPT;
  NANO_NODC_INLINE_CXPR rect merged(const rect& rhs) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR rect intersection(const rect& rhs) const NANO_NOEXCEPT;

  NANO_NODC_INLINE_CXPR rect get_fitted_rect(const rect& r) const NANO_NOEXCEPT;

  NANO_INLINE void swap(rect& w) NANO_NOEXCEPT;

  template <typename U>
  NANO_INLINE void swap(rect<U>& w) NANO_NOEXCEPT;

  template <typename RectType, if_members<RectType, meta::origin, meta::size> = nullptr>
  NANO_NODC_INLINE RectType convert() const;

  template <typename RectType, if_members<RectType, meta::X, meta::Y, meta::Width, meta::Height> = nullptr>
  NANO_NODC_INLINE RectType convert() const;

  template <typename RectType, if_members<RectType, meta::left, meta::top, meta::right, meta::bottom> = nullptr>
  NANO_NODC_INLINE RectType convert() const;

  template <typename RectType, detail::enable_if_rect_xywh<RectType> = nullptr>
  NANO_NODC_INLINE RectType convert() const;

  template <typename RectType, if_members<RectType, meta::origin, meta::size> = nullptr>
  NANO_NODC_INLINE explicit operator RectType() const;

  template <typename RectType, if_members<RectType, meta::X, meta::Y, meta::Width, meta::Height> = nullptr>
  NANO_NODC_INLINE explicit operator RectType() const;

  template <typename RectType, if_members<RectType, meta::left, meta::top, meta::right, meta::bottom> = nullptr>
  NANO_NODC_INLINE explicit operator RectType() const;

  template <typename RectType, detail::enable_if_rect_xywh<RectType> = nullptr>
  NANO_NODC_INLINE explicit operator RectType() const;

  template <typename U>
  NANO_INLINE friend std::ostream& operator<<(std::ostream& s, const nano::rect<U>& rect);
};

static_assert(std::is_trivial<rect<int>>::value, "nano::rect must remain a trivial type");
static_assert(std::is_trivial<rect<float>>::value, "nano::rect must remain a trivial type");

template <typename T>
rect(T, T, T, T) -> rect<T>;

template <typename T1, typename T2, typename T3, typename T4>
rect(T1, T2, T3, T4) -> rect<std::common_type_t<T1, T2, T3, T4>>;

template <typename RectType, if_members<RectType, meta::origin, meta::size> = nullptr>
rect(const RectType&) -> rect<decltype(RectType{}.origin.x)>;

template <typename RectType, if_members<RectType, meta::X, meta::Y, meta::Width, meta::Height> = nullptr>
rect(const RectType&) -> rect<decltype(RectType{}.X)>;

template <typename RectType, if_members<RectType, meta::left, meta::top, meta::right, meta::bottom> = nullptr>
rect(const RectType&) -> rect<decltype(RectType{}.left)>;

template <typename RectType, detail::enable_if_rect_xywh<RectType> = nullptr>
rect(const RectType&) -> rect<decltype(RectType{}.x)>;

} // namespace nano.

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//
//
//
//
// MARK: - IMPLEMENTATION -
//
//
//
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

// MARK: - Macros -

#ifndef __has_feature
  #define __has_feature(x) 0
#endif

#ifndef __has_builtin
  #define __has_builtin(x) 0
#endif

namespace nano {

//
// MARK: - point -
//

// template <typename T>
// NANO_INLINE_CXPR point<T>::point(value_type X, value_type Y) NANO_NOEXCEPT : x(X), y(Y) {}

/// Construct a Point from PointType with member x and y.
template <typename T>
template <typename PointType, if_members<PointType, meta::x, meta::y>>
NANO_INLINE_CXPR point<T>::point(const PointType& point) NANO_NOEXCEPT : x(static_cast<value_type>(point.x)),
                                                                         y(static_cast<value_type>(point.y)) {}

/// Construct a Point from PointType with member X and Y.
template <typename T>
template <typename PointType, if_members<PointType, meta::X, meta::Y>>
NANO_INLINE_CXPR point<T>::point(const PointType& point) NANO_NOEXCEPT : x(static_cast<value_type>(point.X)),
                                                                         y(static_cast<value_type>(point.Y)) {}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::operator+(value_type v) const NANO_NOEXCEPT {
  return point(x + v, y + v);
}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::operator-(value_type v) const NANO_NOEXCEPT {
  return point(x - v, y - v);
}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::operator*(value_type v) const NANO_NOEXCEPT {
  return point(x * v, y * v);
}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::operator/(value_type v) const NANO_NOEXCEPT {
  return point(x / v, y / v);
}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::operator+(const point& pt) const NANO_NOEXCEPT {
  return point(x + pt.x, y + pt.y);
}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::operator-(const point& pt) const NANO_NOEXCEPT {
  return point(x - pt.x, y - pt.y);
}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::operator*(const point& pt) const NANO_NOEXCEPT {
  return point(x * pt.x, y * pt.y);
}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::operator/(const point& pt) const NANO_NOEXCEPT {
  return point(x / pt.x, y / pt.y);
}

template <typename T>
NANO_INLINE_CXPR point<T>& point<T>::operator+=(value_type v) NANO_NOEXCEPT {
  return *this = (*this + v);
}

template <typename T>
NANO_INLINE_CXPR point<T>& point<T>::operator-=(value_type v) NANO_NOEXCEPT {
  return *this = (*this - v);
}

template <typename T>
NANO_INLINE_CXPR point<T>& point<T>::operator*=(value_type v) NANO_NOEXCEPT {
  return *this = (*this * v);
}

template <typename T>
NANO_INLINE_CXPR point<T>& point<T>::operator/=(value_type v) NANO_NOEXCEPT {
  return *this = (*this / v);
}

template <typename T>
NANO_INLINE_CXPR point<T>& point<T>::operator+=(const point& pt) NANO_NOEXCEPT {
  return *this = (*this + pt);
}

template <typename T>
NANO_INLINE_CXPR point<T>& point<T>::operator-=(const point& pt) NANO_NOEXCEPT {
  return *this = (*this - pt);
}

template <typename T>
NANO_INLINE_CXPR point<T>& point<T>::operator*=(const point& pt) NANO_NOEXCEPT {
  return *this = (*this * pt);
}

template <typename T>
NANO_INLINE_CXPR point<T>& point<T>::operator/=(const point& pt) NANO_NOEXCEPT {
  return *this = (*this / pt);
}

template <typename T>
NANO_INLINE_CXPR bool point<T>::operator==(const point& pt) const NANO_NOEXCEPT {
  if constexpr (std::is_floating_point_v<T>) {
    return nano::fcompare(x, pt.x) && nano::fcompare(y, pt.y);
  }
  else {
    return x == pt.x && y == pt.y;
  }
}

template <typename T>
NANO_INLINE_CXPR bool point<T>::operator!=(const point& pt) const NANO_NOEXCEPT {
  return !operator==(pt);
}

template <typename T>
NANO_INLINE_CXPR bool point<T>::operator<(const point& pt) const NANO_NOEXCEPT {
  return (x < pt.x && y < pt.y);
}
template <typename T>
NANO_INLINE_CXPR bool point<T>::operator<=(const point& pt) const NANO_NOEXCEPT {
  return (x <= pt.x && y <= pt.y);
}
template <typename T>
NANO_INLINE_CXPR bool point<T>::operator>(const point& pt) const NANO_NOEXCEPT {
  return (x > pt.x && y > pt.y);
}
template <typename T>
NANO_INLINE_CXPR bool point<T>::operator>=(const point& pt) const NANO_NOEXCEPT {
  return (x >= pt.x && y >= pt.y);
}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::operator-() const NANO_NOEXCEPT {
  return { -x, -y };
}

template <typename T>
NANO_INLINE_CXPR point<T>& point<T>::set_x(value_type _x) NANO_NOEXCEPT {
  x = _x;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR point<T>& point<T>::set_y(value_type _y) NANO_NOEXCEPT {
  y = _y;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR point<T>& point<T>::add_x(value_type dx) NANO_NOEXCEPT {
  return set_x(x + dx);
}

template <typename T>
NANO_INLINE_CXPR point<T>& point<T>::add_y(value_type dy) NANO_NOEXCEPT {
  return set_y(y + dy);
}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::with_x(value_type _x) const NANO_NOEXCEPT {
  return point(_x, y);
}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::with_y(value_type _y) const NANO_NOEXCEPT {
  return point(x, _y);
}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::with_add_x(value_type dx) const NANO_NOEXCEPT {
  return point(x + dx, y);
}

template <typename T>
NANO_INLINE_CXPR point<T> point<T>::with_add_y(value_type dy) const NANO_NOEXCEPT {
  return point(x, y + dy);
}

template <typename T>
template <typename PointType, if_members<PointType, meta::x, meta::y>>
NANO_INLINE point<T>::operator PointType() const {
  using Type = decltype(PointType{}.x);
  return PointType{ static_cast<Type>(x), static_cast<Type>(y) };
}

template <typename T>
template <typename PointType, if_members<PointType, meta::X, meta::Y>>
NANO_INLINE point<T>::operator PointType() const {
  using Type = decltype(PointType{}.X);
  return PointType{ static_cast<Type>(x), static_cast<Type>(y) };
}

template <typename T>
template <typename PointType, if_members<PointType, meta::x, meta::y>>
NANO_INLINE PointType point<T>::convert() const {
  using Type = decltype(PointType{}.x);
  return PointType{ static_cast<Type>(x), static_cast<Type>(y) };
}

template <typename T>
template <typename PointType, if_members<PointType, meta::X, meta::Y>>
NANO_INLINE PointType point<T>::convert() const {
  using Type = decltype(PointType{}.X);
  return PointType{ static_cast<Type>(x), static_cast<Type>(y) };
}

template <typename T>
NANO_INLINE std::ostream& operator<<(std::ostream& s, const nano::point<T>& point) {
  return s << '{' << point.x << ',' << point.y << '}';
}

//
// MARK: - size -
//

template <typename T>
NANO_INLINE_CXPR size<T>::size(value_type W, value_type H) NANO_NOEXCEPT : width(W), height(H) {}

template <typename T>
template <typename SizeType, if_members<SizeType, meta::width, meta::height>>
NANO_INLINE_CXPR size<T>::size(const SizeType& s) NANO_NOEXCEPT : width(static_cast<value_type>(s.width)),
                                                                  height(static_cast<value_type>(s.height)) {}

template <typename T>
template <typename SizeType, if_members<SizeType, meta::Width, meta::Height>>
NANO_INLINE_CXPR size<T>::size(const SizeType& s) NANO_NOEXCEPT : width(static_cast<value_type>(s.Width)),
                                                                  height(static_cast<value_type>(s.Height)) {}

template <typename T>
NANO_INLINE_CXPR size<T> size<T>::full_scale() {
  return { std::numeric_limits<value_type>::max(), std::numeric_limits<value_type>::max() };
}

template <typename T>
NANO_INLINE_CXPR size<T> size<T>::zero() {
  return { 0, 0 };
}

template <typename T>
NANO_INLINE_CXPR size<T> size<T>::operator+(value_type v) const NANO_NOEXCEPT {
  return { width + v, height + v };
}
template <typename T>
NANO_INLINE_CXPR size<T> size<T>::operator-(value_type v) const NANO_NOEXCEPT {
  return { width - v, height - v };
}
template <typename T>
NANO_INLINE_CXPR size<T> size<T>::operator*(value_type v) const NANO_NOEXCEPT {
  return { width * v, height * v };
}
template <typename T>
NANO_INLINE_CXPR size<T> size<T>::operator/(value_type v) const NANO_NOEXCEPT {
  return { width / v, height / v };
}
template <typename T>
NANO_INLINE_CXPR size<T> size<T>::operator+(const size& s) const NANO_NOEXCEPT {
  return { width + s.width, height + s.height };
}
template <typename T>
NANO_INLINE_CXPR size<T> size<T>::operator-(const size& s) const NANO_NOEXCEPT {
  return { width - s.width, height - s.height };
}
template <typename T>
NANO_INLINE_CXPR size<T> size<T>::operator*(const size& s) const NANO_NOEXCEPT {
  return { width * s.width, height * s.height };
}
template <typename T>
NANO_INLINE_CXPR size<T> size<T>::operator/(const size& s) const NANO_NOEXCEPT {
  return { width / s.width, height / s.height };
}
template <typename T>
NANO_INLINE_CXPR size<T>& size<T>::operator+=(value_type v) NANO_NOEXCEPT {
  return *this = (*this + v);
}
template <typename T>
NANO_INLINE_CXPR size<T>& size<T>::operator-=(value_type v) NANO_NOEXCEPT {
  return *this = (*this - v);
}
template <typename T>
NANO_INLINE_CXPR size<T>& size<T>::operator*=(value_type v) NANO_NOEXCEPT {
  return *this = (*this * v);
}
template <typename T>
NANO_INLINE_CXPR size<T>& size<T>::operator/=(value_type v) NANO_NOEXCEPT {
  return *this = (*this / v);
}
template <typename T>
NANO_INLINE_CXPR size<T>& size<T>::operator+=(const size& s) NANO_NOEXCEPT {
  return *this = (*this + s);
}
template <typename T>
NANO_INLINE_CXPR size<T>& size<T>::operator-=(const size& s) NANO_NOEXCEPT {
  return *this = (*this - s);
}
template <typename T>
NANO_INLINE_CXPR size<T>& size<T>::operator*=(const size& s) NANO_NOEXCEPT {
  return *this = (*this * s);
}
template <typename T>
NANO_INLINE_CXPR size<T>& size<T>::operator/=(const size& s) NANO_NOEXCEPT {
  return *this = (*this / s);
}

template <typename T>
NANO_INLINE_CXPR bool size<T>::operator==(const size& s) const NANO_NOEXCEPT {
  if constexpr (std::is_floating_point_v<T>) {
    return nano::fcompare(width, s.width) && nano::fcompare(height, s.height);
  }
  else {
    return (width == s.width && height == s.height);
  }
}

template <typename T>
NANO_INLINE_CXPR bool size<T>::operator!=(const size& s) const NANO_NOEXCEPT {
  return !operator==(s);
}

template <typename T>
NANO_INLINE_CXPR bool size<T>::operator<(const size& s) const NANO_NOEXCEPT {
  return (width < s.width && height < s.height);
}
template <typename T>
NANO_INLINE_CXPR bool size<T>::operator<=(const size& s) const NANO_NOEXCEPT {
  return (width <= s.width && height <= s.height);
}
template <typename T>
NANO_INLINE_CXPR bool size<T>::operator>(const size& s) const NANO_NOEXCEPT {
  return (width > s.width && height > s.height);
}
template <typename T>
NANO_INLINE_CXPR bool size<T>::operator>=(const size& s) const NANO_NOEXCEPT {
  return (width >= s.width && height >= s.height);
}

//
template <typename T>
NANO_INLINE_CXPR size<T> size<T>::operator-() const NANO_NOEXCEPT {
  return { -width, -height };
}

template <typename T>
NANO_INLINE_CXPR size<T>& size<T>::set_width(value_type w) NANO_NOEXCEPT {
  width = w;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR size<T>& size<T>::set_height(value_type h) NANO_NOEXCEPT {
  height = h;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR size<T>& size<T>::add_width(value_type dw) NANO_NOEXCEPT {
  return set_width(width + dw);
}

template <typename T>
NANO_INLINE_CXPR size<T>& size<T>::add_height(value_type dh) NANO_NOEXCEPT {
  return set_height(height + dh);
}

template <typename T>
NANO_INLINE_CXPR size<T> size<T>::with_width(value_type w) const NANO_NOEXCEPT {
  return size(w, height);
}

template <typename T>
NANO_INLINE_CXPR size<T> size<T>::with_height(value_type h) const NANO_NOEXCEPT {
  return size(width, h);
}

template <typename T>
NANO_INLINE_CXPR size<T> size<T>::with_add_width(value_type dw) const NANO_NOEXCEPT {
  return size(width + dw, height);
}

template <typename T>
NANO_INLINE_CXPR size<T> size<T>::with_add_height(value_type dh) const NANO_NOEXCEPT {
  return size(width, height + dh);
}

template <typename T>
NANO_NODC_INLINE_CXPR bool size<T>::empty() const NANO_NOEXCEPT {
  return width == 0 && height == 0;
}

template <typename T>
template <typename SizeType, if_members<SizeType, meta::width, meta::height>>
NANO_INLINE size<T>::operator SizeType() const {
  using Type = decltype(SizeType{}.width);
  return SizeType{ static_cast<Type>(width), static_cast<Type>(height) };
}

template <typename T>
template <typename SizeType, if_members<SizeType, meta::Width, meta::Height>>
NANO_INLINE size<T>::operator SizeType() const {
  using Type = decltype(SizeType{}.Width);
  return SizeType{ static_cast<Type>(width), static_cast<Type>(height) };
}

template <typename T>
template <typename SizeType, if_members<SizeType, meta::width, meta::height>>
NANO_INLINE SizeType size<T>::convert() const {
  using Type = decltype(SizeType{}.width);
  return SizeType{ static_cast<Type>(width), static_cast<Type>(height) };
}

template <typename T>
template <typename SizeType, if_members<SizeType, meta::Width, meta::Height>>
NANO_INLINE SizeType size<T>::convert() const {
  using Type = decltype(SizeType{}.Width);
  return SizeType{ static_cast<Type>(width), static_cast<Type>(height) };
}

template <typename T>
NANO_INLINE std::ostream& operator<<(std::ostream& s, const nano::size<T>& size) {
  return s << '{' << size.width << ',' << size.height << '}';
}

//
// MARK: - rect -
//

template <typename T>
NANO_INLINE_CXPR rect<T>::rect(value_type x, value_type y, value_type w, value_type h) NANO_NOEXCEPT : origin{ x, y },
                                                                                                       size{ w, h } {}

template <typename T>
NANO_INLINE_CXPR rect<T>::rect(value_type x, value_type y, const size_type& s) NANO_NOEXCEPT : origin{ x, y },
                                                                                               size(s) {}

template <typename T>
template <typename RectType, if_members<RectType, meta::origin, meta::size>>
NANO_INLINE_CXPR rect<T>::rect(const RectType& rect) NANO_NOEXCEPT
    : origin{ static_cast<value_type>(rect.origin.x), static_cast<value_type>(rect.origin.y) },
      size{ static_cast<value_type>(rect.size.width), static_cast<value_type>(rect.size.height) } {}

template <typename T>
template <typename RectType, if_members<RectType, meta::X, meta::Y, meta::Width, meta::Height>>
NANO_INLINE_CXPR rect<T>::rect(const RectType& rect) NANO_NOEXCEPT
    : origin{ static_cast<value_type>(rect.X), static_cast<value_type>(rect.Y) },
      size{ static_cast<value_type>(rect.Width), static_cast<value_type>(rect.Height) } {}

template <typename T>
template <typename RectType, if_members<RectType, meta::left, meta::top, meta::right, meta::bottom>>
NANO_INLINE_CXPR rect<T>::rect(const RectType& rect) NANO_NOEXCEPT
    : origin{ static_cast<value_type>(rect.left), static_cast<value_type>(rect.top) },
      size{ static_cast<value_type>(rect.right - rect.left), static_cast<value_type>(rect.bottom - rect.top) } {}

template <typename T>
template <typename RectType, detail::enable_if_rect_xywh<RectType>>
NANO_INLINE_CXPR rect<T>::rect(const RectType& rect) NANO_NOEXCEPT
    : origin{ static_cast<value_type>(rect.x), static_cast<value_type>(rect.y) },
      size{ static_cast<value_type>(rect.width), static_cast<value_type>(rect.height) } {}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::create_from_point(
    const point_type& topLeft, const point_type& bottomRight) NANO_NOEXCEPT {
  return rect(topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::create_from_bottom_left(
    value_type x, value_type y, value_type w, value_type h) NANO_NOEXCEPT {
  return rect(x, y - h, w, h);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::create_from_bottom_left(const point_type& p, const size_type& s) NANO_NOEXCEPT {
  return rect(p.x, p.y - s.height, s.width, s.height);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::create_from_bottom_right(
    value_type x, value_type y, value_type w, value_type h) NANO_NOEXCEPT {
  return rect(x - w, y - h, w, h);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::create_from_bottom_right(const point_type& p, const size_type& s) NANO_NOEXCEPT {
  return rect(p.x - s.width, p.y - s.height, s.width, s.height);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::create_from_top_left(
    value_type x, value_type y, value_type w, value_type h) NANO_NOEXCEPT {
  return rect(x, y, w, h);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::create_from_top_left(const point_type& p, const size_type& s) NANO_NOEXCEPT {
  return rect(p.x, p.y, s.width, s.height);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::create_from_top_right(
    value_type x, value_type y, value_type w, value_type h) NANO_NOEXCEPT {
  return rect(x - w, y, w, h);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::create_from_top_right(const point_type& p, const size_type& s) NANO_NOEXCEPT {
  return rect(p.x - s.width, p.y, s.width, s.height);
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::set_x(value_type _x) NANO_NOEXCEPT {
  x = _x;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::set_y(value_type _y) NANO_NOEXCEPT {
  y = _y;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::set_width(value_type w) NANO_NOEXCEPT {
  width = w;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::set_height(value_type h) NANO_NOEXCEPT {
  height = h;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::set_position(const point_type& point) NANO_NOEXCEPT {
  origin = point;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::set_size(const size_type& s) NANO_NOEXCEPT {
  size = s;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_top_left(const point_type& point) const NANO_NOEXCEPT {
  return { point, size };
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_top_right(const point_type& p) const NANO_NOEXCEPT {
  return { p - point_type{ width, static_cast<value_type>(0) }, size };
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_bottom_left(const point_type& p) const NANO_NOEXCEPT {
  return { p - point_type{ static_cast<value_type>(0), height }, size };
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_bottom_right(const point_type& p) const NANO_NOEXCEPT {
  return { p - point_type{ width, height }, size };
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_middle(const point_type& point) const NANO_NOEXCEPT {
  return { static_cast<value_type>(point.x - width * 0.5), static_cast<value_type>(point.y - height * 0.5), width,
    height };
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_middle_left(const point_type& point) const NANO_NOEXCEPT {
  return { point.x, point.y - height * 0.5, width, height };
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_middle_right(const point_type& point) const NANO_NOEXCEPT {
  return { point.x - width, static_cast<value_type>(point.y - height * 0.5), width, height };
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_middle_top(const point_type& point) const NANO_NOEXCEPT {
  return { static_cast<value_type>(point.x - width * 0.5), point.y, width, height };
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_middle_bottom(const point_type& point) const NANO_NOEXCEPT {
  return { static_cast<value_type>(point.x - width * 0.5), point.y - height, width, height };
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::add_x(value_type _x) NANO_NOEXCEPT {
  x += _x;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::add_y(value_type _y) NANO_NOEXCEPT {
  y += _y;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::add_width(value_type w) NANO_NOEXCEPT {
  width += w;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::add_height(value_type h) NANO_NOEXCEPT {
  height += h;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::add_point(const point_type& point) NANO_NOEXCEPT {
  x += point.x;
  y += point.y;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::add_size(const size_type& s) NANO_NOEXCEPT {
  width += s.width;
  height += s.height;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::mul_x(value_type _x) NANO_NOEXCEPT {
  x *= _x;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::mul_y(value_type _y) NANO_NOEXCEPT {
  y *= _y;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::mul_width(value_type w) NANO_NOEXCEPT {
  width *= w;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::mul_height(value_type h) NANO_NOEXCEPT {
  height *= h;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_x(value_type _x) const NANO_NOEXCEPT {
  return rect(point_type{ _x, y }, size);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_y(value_type _y) const NANO_NOEXCEPT {
  return rect(point_type{ x, _y }, size);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_width(value_type w) const NANO_NOEXCEPT {
  return rect(origin, size_type{ w, height });
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_height(value_type h) const NANO_NOEXCEPT {
  return rect(origin, size_type{ width, h });
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_position(const point_type& point) const NANO_NOEXCEPT {
  return rect(point, size);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::with_size(const size_type& s) const NANO_NOEXCEPT {
  return rect(origin, s);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::operator+(const point_type& pos) const NANO_NOEXCEPT {
  return rect(x + pos.x, y + pos.y, width, height);
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::operator-(const point_type& pos) const NANO_NOEXCEPT {
  return rect(x - pos.x, y - pos.y, width, height);
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::operator+=(const point_type& pos) NANO_NOEXCEPT {
  x += pos.x;
  y += pos.y;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::operator-=(const point_type& pos) NANO_NOEXCEPT {
  x -= pos.x;
  y -= pos.y;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::value_type rect<T>::left() const NANO_NOEXCEPT {
  return x;
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::value_type rect<T>::right() const NANO_NOEXCEPT {
  return x + width;
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::value_type rect<T>::top() const NANO_NOEXCEPT {
  return y;
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::value_type rect<T>::bottom() const NANO_NOEXCEPT {
  return y + height;
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::top_left() const NANO_NOEXCEPT {
  return origin;
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::top_right() const NANO_NOEXCEPT {
  return { x + width, y };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::top_right(value_type dx, value_type dy) const NANO_NOEXCEPT {
  return point_type{ x + width + dx, y + dy };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::bottom_left() const NANO_NOEXCEPT {
  return point_type{ x, y + height };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::bottom_right() const NANO_NOEXCEPT {
  return point_type{ x + width, y + height };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::middle() const NANO_NOEXCEPT {

  return point_type{ static_cast<value_type>(x + width * 0.5), static_cast<value_type>(y + height * 0.5) };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::middle_left() const NANO_NOEXCEPT {
  return point_type{ x, static_cast<value_type>(y + height * 0.5) };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::middle_right() const NANO_NOEXCEPT {
  return point_type{ x + width, static_cast<value_type>(y + height * 0.5) };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::middle_top() const NANO_NOEXCEPT {
  return point_type{ static_cast<value_type>(x * 0.5), y };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::middle_bottom() const NANO_NOEXCEPT {
  return point_type{ static_cast<value_type>(x * 0.5), y + height };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::next_left(value_type delta) const NANO_NOEXCEPT {
  return point_type{ x - delta, y };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::next_left(const point_type& dt) const NANO_NOEXCEPT {
  return point_type{ x - dt.x, y + dt.y };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::next_right(value_type delta) const NANO_NOEXCEPT {
  return point_type{ x + width + delta, y };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::next_right(const point_type& dt) const NANO_NOEXCEPT {
  return point_type{ x + width + dt.x, y + dt.y };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::next_down(value_type delta) const NANO_NOEXCEPT {
  return point_type{ x, y + height + delta };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::next_down(const point_type& dt) const NANO_NOEXCEPT {
  return point_type{ x + dt.x, y + height + dt.y };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::next_up(value_type delta) const NANO_NOEXCEPT {
  return point_type{ x, y - delta };
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::point_type rect<T>::next_up(const point_type& dt) const NANO_NOEXCEPT {
  return point_type{ x + dt.x, y - dt.y };
}

template <typename T>
NANO_INLINE_CXPR bool rect<T>::operator==(const rect& r) const NANO_NOEXCEPT {
  if constexpr (std::is_floating_point_v<T>) {
    return nano::fcompare(x, r.x) && nano::fcompare(y, r.y) && nano::fcompare(width, r.width)
        && nano::fcompare(height, r.height);
  }
  else {
    return x == r.x && y == r.y && width == r.width && height == r.height;
  }

  //  return x == r.x && y == r.y && width == r.width && height == r.height;
}

template <typename T>
NANO_INLINE_CXPR bool rect<T>::operator!=(const rect& r) const NANO_NOEXCEPT {
  return !operator==(r);
}

template <typename T>
NANO_INLINE_CXPR bool rect<T>::contains(const point_type& pos) const NANO_NOEXCEPT {
  return pos.x >= x && pos.x <= x + width && pos.y >= y && pos.y <= y + height;
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::reduce(const point_type& pt) NANO_NOEXCEPT {
  x += pt.x;
  y += pt.y;
  width -= static_cast<value_type>(2) * pt.x;
  height -= static_cast<value_type>(2) * pt.y;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR rect<T> rect<T>::reduced(const point_type& pt) const NANO_NOEXCEPT {
  return rect(
      x + pt.x, y + pt.y, width - static_cast<value_type>(2 * pt.x), height - static_cast<value_type>(2 * pt.y));
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::expand(const point_type& pt) NANO_NOEXCEPT {
  x -= pt.x;
  y -= pt.y;
  width += static_cast<value_type>(2) * pt.x;
  height += static_cast<value_type>(2) * pt.y;
  return *this;
}

template <typename T>
NANO_NODC_INLINE_CXPR rect<T> rect<T>::expanded(const point_type& pt) const NANO_NOEXCEPT {
  return rect(
      x - pt.x, y - pt.y, width + static_cast<value_type>(2 * pt.x), height + static_cast<value_type>(2 * pt.y));
}

template <typename T>
NANO_INLINE_CXPR bool rect<T>::intersects(const rect& r) const NANO_NOEXCEPT {
  return ((std::min(right(), r.right()) - std::max(x, r.x)) > 0)
      && ((std::min(bottom(), r.bottom()) - std::max(y, r.y)) > 0);
}

template <typename T>
NANO_INLINE_CXPR bool rect<T>::intersects(const point_type& p) const NANO_NOEXCEPT {
  return ((std::min(right(), p.x + std::numeric_limits<value_type>::epsilon()) - std::max(x, p.x)) >= 0)
      && ((std::min(bottom(), p.y + std::numeric_limits<value_type>::epsilon()) - std::max(y, p.y)) >= 0);
}

template <typename T>
NANO_INLINE_CXPR typename rect<T>::value_type rect<T>::area() const NANO_NOEXCEPT {
  return size.width * size.height;
}

template <typename T>
NANO_NODC_INLINE_CXPR rect<T> rect<T>::get_union(const rect& rhs) const NANO_NOEXCEPT {
  value_type nx = std::min(x, rhs.x);
  value_type ny = std::min(y, rhs.y);
  return { nx, ny, std::max(right(), rhs.right()) - nx, std::max(bottom(), rhs.bottom()) - ny };
}

template <typename T>
NANO_INLINE_CXPR rect<T>& rect<T>::merge(const rect& rhs) NANO_NOEXCEPT {
  value_type nx = std::min(x, rhs.x);
  value_type ny = std::min(y, rhs.y);
  *this = { nx, ny, std::max(right(), rhs.right()) - nx, std::max(bottom(), rhs.bottom()) - ny };
  return *this;
}

template <typename T>
NANO_NODC_INLINE_CXPR rect<T> rect<T>::merged(const rect& rhs) const NANO_NOEXCEPT {
  value_type nx = std::min(x, rhs.x);
  value_type ny = std::min(y, rhs.y);
  return { nx, ny, std::max(right(), rhs.right()) - nx, std::max(bottom(), rhs.bottom()) - ny };
}

template <typename T>
NANO_NODC_INLINE_CXPR rect<T> rect<T>::intersection(const rect& rhs) const NANO_NOEXCEPT {
  value_type nx = std::max(x, rhs.x);
  value_type nw = std::min(right(), rhs.right()) - nx;

  if (nw < 0) {
    return { 0, 0, 0, 0 };
  }

  value_type ny = std::max(y, rhs.y);
  value_type nh = std::min(bottom(), rhs.bottom()) - ny;

  if (nh < 0) {
    return { 0, 0, 0, 0 };
  }

  return { nx, ny, nw, nh };
}

template <typename T>
NANO_NODC_INLINE_CXPR rect<T> rect<T>::get_fitted_rect(const rect& r) const NANO_NOEXCEPT {
  if (width < height) {
    double hRatio = r.height / static_cast<double>(r.width);
    return r.with_size({ width, static_cast<value_type>(hRatio * width) });
  }
  else {
    double wRatio = r.width / static_cast<double>(r.height);
    return r.with_size({ static_cast<value_type>(wRatio * height), height });
  }
}

template <typename T>
NANO_INLINE void rect<T>::swap(rect<T>& r) NANO_NOEXCEPT {
  rect tmp = r;
  r = *this;
  *this = tmp;
}

template <typename T>
template <typename U>
NANO_INLINE void rect<T>::swap(rect<U>& r) NANO_NOEXCEPT {
  rect tmp = r;
  r = *this;
  *this = tmp;
}

template <typename T>
template <typename RectType, if_members<RectType, meta::origin, meta::size>>
NANO_INLINE rect<T>::operator RectType() const {
  using Type = decltype(RectType{}.origin.x);
  return RectType{ { static_cast<Type>(x), static_cast<Type>(y) },
    { static_cast<Type>(width), static_cast<Type>(height) } };
}

template <typename T>
template <typename RectType, if_members<RectType, meta::X, meta::Y, meta::Width, meta::Height>>
NANO_INLINE rect<T>::operator RectType() const {
  using Type = decltype(RectType{}.X);
  return RectType{ static_cast<Type>(x), static_cast<Type>(y), static_cast<Type>(width), static_cast<Type>(height) };
}

template <typename T>
template <typename RectType, if_members<RectType, meta::left, meta::top, meta::right, meta::bottom>>
NANO_INLINE rect<T>::operator RectType() const {
  using Type = decltype(RectType{}.left);
  return RectType{ static_cast<Type>(x), static_cast<Type>(y), static_cast<Type>(x + width),
    static_cast<Type>(y + height) };
}

template <typename T>
template <typename RectType, detail::enable_if_rect_xywh<RectType>>
NANO_INLINE rect<T>::operator RectType() const {
  using Type = decltype(RectType{}.x);
  return RectType{ static_cast<Type>(x), static_cast<Type>(y), static_cast<Type>(width), static_cast<Type>(height) };
}

template <typename T>
template <typename RectType, if_members<RectType, meta::origin, meta::size>>
NANO_INLINE RectType rect<T>::convert() const {
  using Type = decltype(RectType{}.origin.x);
  return RectType{ { static_cast<Type>(x), static_cast<Type>(y) },
    { static_cast<Type>(width), static_cast<Type>(height) } };
}

template <typename T>
template <typename RectType, if_members<RectType, meta::X, meta::Y, meta::Width, meta::Height>>
NANO_INLINE RectType rect<T>::convert() const {
  using Type = decltype(RectType{}.X);
  return RectType{ static_cast<Type>(x), static_cast<Type>(y), static_cast<Type>(width), static_cast<Type>(height) };
}

template <typename T>
template <typename RectType, if_members<RectType, meta::left, meta::top, meta::right, meta::bottom>>
NANO_INLINE RectType rect<T>::convert() const {
  using Type = decltype(RectType{}.left);
  return RectType{ static_cast<Type>(x), static_cast<Type>(y), static_cast<Type>(x + width),
    static_cast<Type>(y + height) };
}

template <typename T>
template <typename RectType, detail::enable_if_rect_xywh<RectType>>
NANO_INLINE RectType rect<T>::convert() const {
  using Type = decltype(RectType{}.x);
  return RectType{ static_cast<Type>(x), static_cast<Type>(y), static_cast<Type>(width), static_cast<Type>(height) };
}

template <typename T>
NANO_INLINE std::ostream& operator<<(std::ostream& s, const nano::rect<T>& rect) {
  return s << '{' << rect.x << ',' << rect.y << ',' << rect.width << ',' << rect.height << '}';
}

//
// MARK: - range -
//

template <typename T>
NANO_CXPR range<T> range<T>::with_length(value_type start, value_type len) NANO_NOEXCEPT {
  return { start, start + len };
}

template <typename T>
NANO_CXPR range<T>::range(value_type _start, value_type _end) NANO_NOEXCEPT : start(_start), end(_end) {}

template <typename T>
NANO_CXPR range<T> range<T>::with_start(value_type s) const NANO_NOEXCEPT {
  return { s, end };
}

template <typename T>
NANO_CXPR range<T> range<T>::with_end(value_type e) const NANO_NOEXCEPT {
  return { start, e };
}

template <typename T>
NANO_CXPR range<T> range<T>::with_shifted_start(value_type delta) const NANO_NOEXCEPT {
  return { start + delta, end };
}

template <typename T>
NANO_CXPR range<T> range<T>::with_shifted_end(value_type delta) const NANO_NOEXCEPT {
  return { start, end + delta };
}

template <typename T>
NANO_CXPR range<T> range<T>::with_length(value_type len) const NANO_NOEXCEPT {
  return { start, start + len };
}

template <typename T>
NANO_CXPR range<T> range<T>::with_shift(value_type delta) const NANO_NOEXCEPT {
  return { start + delta, end + delta };
}

template <typename T>
NANO_CXPR range<T> range<T>::with_move(value_type s) const NANO_NOEXCEPT {
  return { s, s + length() };
}

template <typename T>
NANO_INLINE_CXPR range<T>& range<T>::set_start(value_type s) NANO_NOEXCEPT {
  start = s;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR range<T>& range<T>::set_end(value_type e) NANO_NOEXCEPT {
  end = e;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR range<T>& range<T>::move_to(value_type s) NANO_NOEXCEPT {
  value_type len = length();
  start = s;
  end = s + len;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR range<T>& range<T>::shift(value_type delta) NANO_NOEXCEPT {
  start += delta;
  end += delta;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR range<T>& range<T>::shift_start(value_type delta) NANO_NOEXCEPT {
  start += delta;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR range<T>& range<T>::shift_end(value_type delta) NANO_NOEXCEPT {
  end += delta;
  return *this;
}

template <typename T>
NANO_INLINE_CXPR range<T>& range<T>::set_length(value_type len) NANO_NOEXCEPT {
  end = start + len;
  return *this;
}

template <typename T>
NANO_CXPR typename range<T>::value_type range<T>::length() const NANO_NOEXCEPT {
  return end - start;
}

template <typename T>
NANO_CXPR typename range<T>::value_type range<T>::middle() const NANO_NOEXCEPT {
  return static_cast<T>(start + (end - start) * 0.5);
}

template <typename T>
NANO_CXPR bool range<T>::is_sorted() const NANO_NOEXCEPT {
  return start <= end;
}

template <typename T>
NANO_CXPR bool range<T>::is_symmetric() const NANO_NOEXCEPT {
  if constexpr (std::is_floating_point_v<T>) {
    return nano::fcompare(start, -end);
  }
  else {
    return start == -end;
  }
}

template <typename T>
NANO_CXPR bool range<T>::contains(value_type x) const NANO_NOEXCEPT {
  return x >= start && x <= end;
}

template <typename T>
NANO_CXPR bool range<T>::contains_closed(value_type x) const NANO_NOEXCEPT {
  return x >= start && x <= end;
}

template <typename T>
NANO_CXPR bool range<T>::contains_opened(value_type x) const NANO_NOEXCEPT {
  return x > start && x < end;
}

template <typename T>
NANO_CXPR bool range<T>::contains_left_opened(value_type x) const NANO_NOEXCEPT {
  return x > start && x <= end;
}

template <typename T>
NANO_CXPR bool range<T>::contains_right_opened(value_type x) const NANO_NOEXCEPT {
  return x >= start && x < end;
}

template <typename T>
NANO_CXPR bool range<T>::contains(const range& r) const NANO_NOEXCEPT {
  return contains(r.start) && contains(r.end);
}

template <typename T>
NANO_NODC_INLINE_CXPR typename range<T>::value_type range<T>::clipped_value(value_type x) const NANO_NOEXCEPT {
  const value_type t = x < start ? start : x;
  return t > end ? end : t;
}

template <typename T>
NANO_CXPR void range<T>::sort() NANO_NOEXCEPT {
  if (!is_sorted()) {
    std::swap(start, end);
  }
}

template <typename T>
NANO_CXPR bool range<T>::operator==(const range<T>& r) const NANO_NOEXCEPT {
  if constexpr (std::is_floating_point_v<T>) {
    return nano::fcompare(start, r.start) && nano::fcompare(end, r.end);
  }
  else {
    return start == r.start && end == r.end;
  }
}

template <typename T>
NANO_CXPR bool range<T>::operator!=(const range<T>& r) const NANO_NOEXCEPT {
  return !operator==(r);
}

template <class T>
NANO_INLINE std::ostream& operator<<(std::ostream& s, const range<T>& r) {
  return s << '{' << r.start << ',' << r.end << '}';
}

//
// MARK: - padding -
//

template <class T>
NANO_CXPR padding<T>::padding(value_type t, value_type l, value_type b, value_type r) NANO_NOEXCEPT : top(t),
                                                                                                      left(l),
                                                                                                      bottom(b),
                                                                                                      right(r) {}

template <class T>
NANO_CXPR padding<T>::padding(value_type p) NANO_NOEXCEPT : top(p), left(p), bottom(p), right(p) {}

template <class T>
nano::rect<T> padding<T>::inside_rect(const nano::rect<T>& rect) const NANO_NOEXCEPT {
  return nano::rect<T>(
      rect.origin.x + left, rect.origin.y + top, rect.size.width - (left + right), rect.size.height - (top + bottom));
}

template <class T>
nano::rect<T> padding<T>::outside_rect(const nano::rect<T>& rect) const NANO_NOEXCEPT {
  return nano::rect<T>(
      rect.origin.x - left, rect.origin.y - top, rect.size.width + left + right, rect.size.height + top + bottom);
}

template <class T>
NANO_CXPR bool padding<T>::empty() const NANO_NOEXCEPT {
  return top == 0 && left == 0 && bottom == 0 && right == 0;
}

template <class T>
NANO_CXPR bool padding<T>::operator==(const padding& p) const NANO_NOEXCEPT {
  return top == p.top && left == p.left && bottom == p.bottom && right == p.right;
}

template <class T>
NANO_CXPR bool padding<T>::operator!=(const padding& p) const NANO_NOEXCEPT {
  return !operator==(p);
}

template <class T>
std::ostream& operator<<(std::ostream& s, const padding<T>& p) {
  return s << '{' << p.top << ',' << p.left << ',' << p.bottom << ',' << p.right << '}';
}
} // namespace nano.

NANO_CLANG_DIAGNOSTIC_POP()
