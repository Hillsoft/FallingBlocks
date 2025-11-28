#pragma once

#include <type_traits>
#include "util/raii_helpers.hpp"

namespace util {

template <typename T>
class StorageFor : private util::no_copy_move {
 private:
  struct DummyType {};

 public:
  explicit constexpr StorageFor() : holder_(DummyType{}) {}

  template <typename... Args>
  explicit constexpr StorageFor(Args&&... args) : holder_(DummyType{}) {
    emplace(std::forward<Args>(args)...);
  }

  template <typename... Args>
  void emplace(Args&&... args) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access,bugprone-multi-level-implicit-pointer-conversion)
    new (&holder_.value_) T(std::forward<Args>(args)...);
  }

  void destroy()
    requires(!std::is_trivially_destructible_v<T>)
  {
    get()->~T();
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
  [[nodiscard]] T* get() { return std::launder(&holder_.value_); }
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
  [[nodiscard]] const T* get() const { return std::launder(&holder_.value_); }
  [[nodiscard]] T& operator*() { return *get(); }
  [[nodiscard]] const T& operator*() const { return *get(); }
  T* operator->() { return get(); }
  const T* operator->() const { return get(); }

 private:
  // NOLINTNEXTLINE(*-special-member-functions)
  union Holder {
    ~Holder()
      requires(std::is_trivially_destructible_v<T>)
    = default;
    ~Holder()
      requires(!std::is_trivially_destructible_v<T>)
    {}

    DummyType dummy_;
    T value_;
  };

  Holder holder_;
};

} // namespace util
