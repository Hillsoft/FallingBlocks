#pragma once

#include <type_traits>
#include <utility>
#include "util/raii_helpers.hpp"

namespace util {

template <typename T>
class StorageFor : private util::no_copy_move {
 public:
  explicit constexpr StorageFor() {}

  template <typename... Args>
  explicit constexpr StorageFor(Args&&... args) {
    emplace(std::forward<Args>(args)...);
  }

  template <typename... Args>
  void emplace(Args&&... args) {
    new (buffer_) T(std::forward<Args>(args)...);
  }

  void destroy()
    requires(!std::is_trivially_destructible_v<T>)
  {
    get()->~T();
  }

  T* get() { return reinterpret_cast<T*>(buffer_); }
  const T* get() const { return reinterpret_cast<const T*>(buffer_); }
  T& operator*() { return *get(); }
  const T& operator*() const { return *get(); }
  T* operator->() { return get(); }
  const T* operator->() const { return get(); }

 private:
  alignas(T) char buffer_[sizeof(T)]{0};
};

} // namespace util
