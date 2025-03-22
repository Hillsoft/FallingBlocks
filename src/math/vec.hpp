#pragma once

#include <array>
#include <type_traits>
#include "util/debug.hpp"

namespace math {

namespace detail {

template <typename TNum>
constexpr size_t alignmentForVec(size_t size) {
  if (size <= 1) {
    return alignof(TNum);
  } else if (size <= 2) {
    return 2 * alignof(TNum);
  } else {
    return 4 * alignof(TNum);
  }
}

} // namespace detail

template <typename TNum, size_t size>
class Vec {
 public:
  Vec(const Vec& other) = default;
  Vec(Vec&& other) = default;

  template <typename... TArgs>
  Vec(TArgs&&... args)
    requires(std::conjunction_v<std::is_convertible<TArgs, TNum>...>)
      : data_(std::forward<TArgs>(args)...) {}

  template <typename TNum2>
  Vec(const Vec<TNum2, size>& other)
    requires(std::is_convertible_v<TNum2, TNum>)
      : data_() {
    for (int i = 0; i < size; i++) {
      data_ = other.at(i);
    }
  }

  TNum& at(size_t index) {
    DEBUG_ASSERT(index < size);
    return data_[index];
  }

  const TNum& at(size_t index) const {
    DEBUG_ASSERT(index < size);
    return data_[index];
  }

  TNum& x()
    requires(size >= 1)
  {
    return at(0);
  }

  const TNum& x() const
    requires(size >= 1)
  {
    return at(0);
  }

  TNum& y()
    requires(size >= 2)
  {
    return at(1);
  }

  const TNum& y() const
    requires(size >= 2)
  {
    return at(1);
  }

  TNum& z()
    requires(size >= 3)
  {
    return at(2);
  }

  const TNum& z() const
    requires(size >= 3)
  {
    return at(2);
  }

 private:
  alignas(detail::alignmentForVec<TNum>(size)) std::array<TNum, size> data_;
};

static_assert(sizeof(float) == 4);
using Vec2 = Vec<float, 2>;
using Vec3 = Vec<float, 3>;

template <typename TNum, size_t size>
Vec<TNum, size> operator+(const Vec<TNum, size>& a, const Vec<TNum, size>& b)
  requires(std::is_arithmetic_v<TNum>)
{
  Vec<TNum, size> result;
  for (size_t i = 0; i < size; i++) {
    result.at(i) = a.at(i) + b.at(i);
  }
  return result;
}

template <typename TNum, size_t size>
Vec<TNum, size> operator-(const Vec<TNum, size>& a, const Vec<TNum, size>& b)
  requires(std::is_arithmetic_v<TNum>)
{
  Vec<TNum, size> result;
  for (size_t i = 0; i < size; i++) {
    result.at(i) = a.at(i) - b.at(i);
  }
  return result;
}

template <typename TNum, size_t size>
Vec<TNum, size> operator/(const Vec<TNum, size>& a, TNum d)
  requires(std::is_arithmetic_v<TNum>)
{
  Vec<TNum, size> result;
  for (size_t i = 0; i < size; i++) {
    result.at(i) = a.at(i) / d;
  }
  return result;
}

template <typename TNum, size_t size>
Vec<TNum, size> abs(const Vec<TNum, size>& a)
  requires(std::is_arithmetic_v<TNum>)
{
  Vec<TNum, size> result;
  for (size_t i = 0; i < size; i++) {
    result.at(i) = std::abs(a.at(i));
  }
  return result;
}

} // namespace math
