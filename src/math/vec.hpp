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
  constexpr Vec(const Vec& other) = default;
  constexpr Vec(Vec&& other) = default;
  constexpr Vec& operator=(const Vec& other) = default;
  constexpr Vec& operator=(Vec&& other) = default;

  template <typename... TArgs>
  constexpr Vec(TArgs&&... args)
    requires(std::conjunction_v<std::is_convertible<TArgs, TNum>...>)
      : data_{std::forward<TArgs>(args)...} {}

  template <typename TNum2>
  constexpr Vec(const Vec<TNum2, size>& other)
    requires(std::is_convertible_v<TNum2, TNum>)
      : data_() {
    for (int i = 0; i < size; i++) {
      data_ = other.at(i);
    }
  }

  constexpr TNum& at(size_t index) {
    DEBUG_ASSERT(index < size);
    return data_[index];
  }

  constexpr const TNum& at(size_t index) const {
    DEBUG_ASSERT(index < size);
    return data_[index];
  }

  constexpr TNum& x()
    requires(size >= 1)
  {
    return at(0);
  }

  constexpr const TNum& x() const
    requires(size >= 1)
  {
    return at(0);
  }

  constexpr TNum& y()
    requires(size >= 2)
  {
    return at(1);
  }

  constexpr const TNum& y() const
    requires(size >= 2)
  {
    return at(1);
  }

  constexpr TNum& z()
    requires(size >= 3)
  {
    return at(2);
  }

  constexpr const TNum& z() const
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
using Vec4 = Vec<float, 4>;

template <typename TNum, size_t size>
constexpr Vec<TNum, size> operator+(
    const Vec<TNum, size>& a, const Vec<TNum, size>& b)
  requires(std::is_arithmetic_v<TNum>)
{
  Vec<TNum, size> result;
  for (size_t i = 0; i < size; i++) {
    result.at(i) = a.at(i) + b.at(i);
  }
  return result;
}

template <typename TNum, size_t size>
constexpr Vec<TNum, size> operator+=(
    Vec<TNum, size>& a, const Vec<TNum, size>& b)
  requires(std::is_arithmetic_v<TNum>)
{
  for (size_t i = 0; i < size; i++) {
    a.at(i) += b.at(i);
  }
  return a;
}

template <typename TNum, size_t size>
constexpr Vec<TNum, size> operator-(
    const Vec<TNum, size>& a, const Vec<TNum, size>& b)
  requires(std::is_arithmetic_v<TNum>)
{
  Vec<TNum, size> result;
  for (size_t i = 0; i < size; i++) {
    result.at(i) = a.at(i) - b.at(i);
  }
  return result;
}

template <typename TNum, size_t size>
constexpr Vec<TNum, size> operator*(TNum fac, const Vec<TNum, size>& a)
  requires(std::is_arithmetic_v<TNum>)
{
  Vec<TNum, size> result;
  for (size_t i = 0; i < size; i++) {
    result.at(i) = fac * a.at(i);
  }
  return result;
}

template <typename TNum, size_t size>
constexpr Vec<TNum, size>& operator*=(Vec<TNum, size>& a, TNum fac)
  requires(std::is_arithmetic_v<TNum>)
{
  for (size_t i = 0; i < size; i++) {
    a.at(i) *= fac;
  }
  return a;
}

template <typename TNum, size_t size>
constexpr Vec<TNum, size> operator/(const Vec<TNum, size>& a, TNum d)
  requires(std::is_arithmetic_v<TNum>)
{
  Vec<TNum, size> result;
  for (size_t i = 0; i < size; i++) {
    result.at(i) = a.at(i) / d;
  }
  return result;
}

template <typename TNum, size_t size>
constexpr Vec<TNum, size> abs(const Vec<TNum, size>& a)
  requires(std::is_arithmetic_v<TNum>)
{
  Vec<TNum, size> result;
  for (size_t i = 0; i < size; i++) {
    result.at(i) = std::abs(a.at(i));
  }
  return result;
}

template <typename TNum, size_t sizeWidth, size_t sizeHeight>
class Matrix {
 public:
  constexpr Matrix(const Matrix& other) = default;
  constexpr Matrix(Matrix&& other) = default;
  constexpr Matrix& operator=(const Matrix& other) = default;
  constexpr Matrix& operator=(Matrix&& other) = default;

  template <typename... TArgs>
  constexpr Matrix(TArgs&&... args)
    requires(std::conjunction_v<std::is_convertible<TArgs, TNum>...>)
      : data_{std::forward<TArgs>(args)...} {}

  template <typename TNum2>
  constexpr Matrix(const Matrix<TNum2, sizeWidth, sizeHeight>& other)
    requires(std::is_convertible_v<TNum2, TNum>)
      : data_() {
    for (int i = 0; i < sizeWidth * sizeHeight; i++) {
      data_[i] = other.data_[i];
    }
  }

  constexpr TNum& at(size_t x, size_t y) {
    DEBUG_ASSERT(x < sizeWidth && y < sizeHeight);
    return data_[x].at(y);
  }
  constexpr const TNum& at(size_t x, size_t y) const {
    DEBUG_ASSERT(x < sizeWidth && y < sizeHeight);
    return data_[x].at(y);
  }

  constexpr static Matrix identity()
    requires(sizeWidth == sizeHeight)
  {
    Matrix result{};
    for (size_t i = 0; i < sizeWidth; i++) {
      result.at(i, i) = 1.0f;
    }
    return result;
  }

  constexpr static Matrix scale(TNum factor)
    requires(sizeWidth == sizeHeight)
  {
    Matrix result = identity();
    result *= factor;
    return result;
  }

  constexpr static Matrix scale(const Vec<TNum, sizeWidth>& v)
    requires(sizeWidth == sizeHeight)
  {
    Matrix result = identity();
    for (size_t i = 0; i < sizeWidth; i++) {
      result.at(i, i) = v.at(i);
    }
    return result;
  }

  constexpr static Matrix scale(const Vec<TNum, sizeWidth - 1>& v)
    requires(sizeWidth == sizeHeight)
  {
    Matrix result = identity();
    for (size_t i = 0; i < sizeWidth - 1; i++) {
      result.at(i, i) = v.at(i);
    }
    return result;
  }

  constexpr static Matrix translate(const Vec<TNum, sizeHeight - 1>& v) {
    Matrix result = identity();
    for (size_t i = 0; i < sizeHeight - 1; i++) {
      result.at(sizeWidth - 1, i) = v.at(i);
    }
    return result;
  }

 private:
  std::array<Vec<TNum, sizeHeight>, sizeWidth> data_;
};

using Mat2 = Matrix<float, 2, 2>;
using Mat3 = Matrix<float, 3, 3>;

template <typename TNum, size_t sizeWidth, size_t sizeHeight>
constexpr Matrix<TNum, sizeWidth, sizeHeight> operator*(
    const Matrix<TNum, sizeWidth, sizeHeight>& a, TNum fac)
  requires(std::is_arithmetic_v<TNum>)
{
  Matrix<TNum, sizeWidth, sizeHeight> result{};
  for (size_t y = 0; y < sizeHeight; y++) {
    for (size_t x = 0; x < sizeHeight; x++) {
      result.at(x, y) = a.at(x, y) * fac;
    }
  }
  return result;
}

template <typename TNum, size_t sizeA, size_t sizeB, size_t sizeC>
constexpr Matrix<TNum, sizeC, sizeB> operator*(
    const Matrix<TNum, sizeA, sizeB>& l, const Matrix<TNum, sizeC, sizeA>& r) {
  Matrix<TNum, sizeC, sizeB> result;
  for (size_t y = 0; y < sizeB; y++) {
    for (size_t x = 0; x < sizeC; x++) {
      for (size_t z = 0; z < sizeA; z++) {
        result.at(x, y) += l.at(z, y) * r.at(x, z);
      }
    }
  }
  return result;
}

template <typename TNum = float>
constexpr Matrix<TNum, 3, 3> modelMatrixFromBounds(
    math::Vec2 p0, math::Vec2 p1) {
  return Matrix<TNum, 3, 3>::translate(p0) * Matrix<TNum, 3, 3>::scale(p1 - p0);
}

} // namespace math
