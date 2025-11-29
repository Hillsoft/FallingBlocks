#pragma once

#include <array>
#include <type_traits>
#include "util/debug.hpp"
#include "util/meta_utils.hpp"

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

template <typename TNum, size_t size>
struct VecFieldNamesImpl {
  using Fields = void;
};

template <typename TNum>
struct VecFieldNamesImpl<TNum, 2> {
  using Fields = util::TArray<
      util::TPair<util::TString<"x">, TNum>,
      util::TPair<util::TString<"y">, TNum>>;
};

template <typename TNum, size_t size>
using VecFieldNames = VecFieldNamesImpl<TNum, size>::Fields;

} // namespace detail

template <typename TNum, size_t size>
class Vec {
 public:
  Vec() : data_{} {}
  constexpr Vec(const Vec& other) = default;
  constexpr Vec(Vec&& other) = default;
  constexpr Vec& operator=(const Vec& other) = default;
  constexpr Vec& operator=(Vec&& other) = default;
  constexpr ~Vec() = default;

  template <typename... TArgs>
  explicit constexpr Vec(TArgs&&... args)
    requires(std::conjunction_v<std::is_convertible<TArgs, TNum>...>)
      : data_{std::forward<TArgs>(args)...} {}

  template <typename TNum2>
  explicit constexpr Vec(const Vec<TNum2, size>& other)
    requires(std::is_convertible_v<TNum2, TNum>)
      : data_() {
    for (int i = 0; i < size; i++) {
      data_.at(i) = static_cast<TNum>(other.at(i));
    }
  }

  [[nodiscard]] constexpr TNum& at(size_t index) {
    DEBUG_ASSERT(index < size);
    return data_[index];
  }

  [[nodiscard]] constexpr const TNum& at(size_t index) const {
    DEBUG_ASSERT(index < size);
    return data_[index];
  }

  [[nodiscard]] constexpr TNum& x()
    requires(size >= 1)
  {
    return at(0);
  }

  [[nodiscard]] constexpr const TNum& x() const
    requires(size >= 1)
  {
    return at(0);
  }

  [[nodiscard]] constexpr TNum& y()
    requires(size >= 2)
  {
    return at(1);
  }

  [[nodiscard]] constexpr const TNum& y() const
    requires(size >= 2)
  {
    return at(1);
  }

  [[nodiscard]] constexpr TNum& z()
    requires(size >= 3)
  {
    return at(2);
  }

  [[nodiscard]] constexpr const TNum& z() const
    requires(size >= 3)
  {
    return at(2);
  }

  [[nodiscard]] constexpr TNum mag2() const {
    TNum mag2 = 0.0f;
    for (int i = 0; i < size; i++) {
      mag2 += data_[i] * data_[i];
    }
    return mag2;
  }

  [[nodiscard]] constexpr TNum mag() const { return sqrt(mag2()); }

  [[nodiscard]] constexpr TNum dot(const Vec& other) {
    TNum dotVal = 0.0f;
    for (int i = 0; i < size; i++) {
      dotVal += data_[i] * other.data_[i];
    }
    return dotVal;
  }

  using Fields = detail::VecFieldNames<TNum, size>;

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
  Matrix() : data_{} {}
  constexpr Matrix(const Matrix& other) = default;
  constexpr Matrix(Matrix&& other) = default;
  constexpr Matrix& operator=(const Matrix& other) = default;
  constexpr Matrix& operator=(Matrix&& other) = default;
  constexpr ~Matrix() = default;

  template <typename... TArgs>
  explicit constexpr Matrix(TArgs&&... args)
    requires(std::conjunction_v<std::is_convertible<TArgs, TNum>...>)
      : data_{std::forward<TArgs>(args)...} {}

  template <typename TNum2>
  explicit constexpr Matrix(const Matrix<TNum2, sizeWidth, sizeHeight>& other)
    requires(std::is_convertible_v<TNum2, TNum>)
      : data_() {
    for (int i = 0; i < sizeWidth * sizeHeight; i++) {
      data_[i] = static_cast<TNum>(other.data_[i]);
    }
  }

  [[nodiscard]] constexpr TNum& at(size_t x, size_t y) {
    DEBUG_ASSERT(x < sizeWidth && y < sizeHeight);
    return data_[x].at(y);
  }
  [[nodiscard]] constexpr const TNum& at(size_t x, size_t y) const {
    DEBUG_ASSERT(x < sizeWidth && y < sizeHeight);
    return data_[x].at(y);
  }

  [[nodiscard]] constexpr TNum determinant() const
    requires(sizeWidth == sizeHeight)
  {
    if constexpr (sizeWidth == 1) {
      return at(0, 0);
    } else {
      TNum accum = 0;
      for (size_t i = 0; i < sizeWidth; i++) {
        Matrix<TNum, sizeWidth - 1, sizeHeight - 1> minor;
        size_t minI = 0;
        for (size_t i2 = 0; i2 < sizeWidth; i2++) {
          if (i2 != i) {
            for (size_t j = 0; j < sizeHeight - 1; j++) {
              minor.at(minI, j) = at(i2, j + 1);
            }
            minI++;
          }
        }

        TNum sign = i % 2 == 0 ? static_cast<TNum>(1) : static_cast<TNum>(-1);
        accum += sign * at(i, 0) * minor.determinant();
      }

      return accum;
    }
  }

  [[nodiscard]] constexpr Matrix invert() const
    requires(sizeWidth == sizeHeight)
  {
    Matrix result{};

    if constexpr (sizeWidth == 1) {
      result.at(0, 0) = static_cast<TNum>(1) / at(0, 0);
      return result;
    } else {
      // matrix of minors
      for (size_t i = 0; i < sizeWidth; i++) {
        for (size_t j = 0; j < sizeHeight; j++) {
          Matrix<TNum, sizeWidth - 1, sizeHeight - 1> minor;
          size_t minI = 0;
          for (size_t i2 = 0; i2 < sizeWidth; i2++) {
            if (i2 == i) {
              continue;
            }
            size_t minJ = 0;
            for (size_t j2 = 0; j2 < sizeHeight; j2++) {
              if (j2 == j) {
                continue;
              }
              minor.at(minI, minJ) = at(i2, j2);
              minJ++;
            }
            minI++;
          }

          result.at(i, j) = minor.determinant();
        }
      }

      // cofactors
      for (size_t i = 0; i < sizeWidth; i++) {
        for (size_t j = 0; j < sizeHeight; j++) {
          if ((i + j) % 2 != 0) {
            result.at(i, j) *= -1;
          }
        }
      }

      // adjugate
      for (size_t i = 0; i < sizeWidth; i++) {
        for (size_t j = 0; j < i; j++) {
          std::swap(result.at(i, j), result.at(j, i));
        }
      }

      result *= 1 / determinant();

      return result;
    }
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

template <typename TNum, size_t sizeWidth, size_t sizeHeight>
constexpr Matrix<TNum, sizeWidth, sizeHeight>& operator*=(
    Matrix<TNum, sizeWidth, sizeHeight>& mat, TNum fac)
  requires(std::is_arithmetic_v<TNum>)
{
  for (size_t y = 0; y < sizeHeight; y++) {
    for (size_t x = 0; x < sizeHeight; x++) {
      mat.at(x, y) *= fac;
    }
  }
  return mat;
}

template <typename TNum, size_t sizeA, size_t sizeB, size_t sizeC>
constexpr Matrix<TNum, sizeC, sizeB> operator*(
    const Matrix<TNum, sizeA, sizeB>& l, const Matrix<TNum, sizeC, sizeA>& r)
  requires(std::is_arithmetic_v<TNum>)
{
  Matrix<TNum, sizeC, sizeB> result{};
  for (size_t y = 0; y < sizeB; y++) {
    for (size_t x = 0; x < sizeC; x++) {
      for (size_t z = 0; z < sizeA; z++) {
        result.at(x, y) += l.at(z, y) * r.at(x, z);
      }
    }
  }
  return result;
}

template <typename TNum, size_t sizeA, size_t sizeB>
constexpr Vec<TNum, sizeB> operator*(
    const Matrix<TNum, sizeA, sizeB>& l, const Vec<TNum, sizeA>& r)
  requires(std::is_arithmetic_v<TNum>)
{
  Vec<TNum, sizeB> result;
  for (size_t y = 0; y < sizeB; y++) {
    for (size_t z = 0; z < sizeA; z++) {
      result.at(y) += l.at(z, y) * r.at(z);
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
