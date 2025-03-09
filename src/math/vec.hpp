#pragma once

#include <array>

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
  template <typename... TArgs>
  Vec(TArgs&&... args) : data_(std::forward<TArgs>(args)...) {}

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

} // namespace math
