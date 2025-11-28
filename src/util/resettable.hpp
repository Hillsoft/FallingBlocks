#pragma once

#include <type_traits>
#include <utility>

#include "util/storage.hpp"

namespace util {

template <typename T>
class Resettable {
 public:
  template <typename... TArgs>
  explicit Resettable(TArgs&&... args)
      : storage_(std::forward<TArgs>(args)...) {}

  ~Resettable()
    requires(!std::is_trivially_destructible_v<T>)
  {
    storage_.destroy();
  }

  ~Resettable()
    requires(std::is_trivially_destructible_v<T>)
  = default;

  Resettable(const Resettable& other) : storage_(*other) {}
  // NOLINTNEXTLINE(cert-oop54-cpp)
  Resettable& operator=(const Resettable& other) { *storage_ = *other; }

  Resettable(Resettable&& other) noexcept : storage_(std::move(*other)) {}
  Resettable& operator=(Resettable&& other) noexcept {
    *storage_ = std::move(*other);
  }

  template <typename... TArgs>
  void reset(TArgs&&... args) {
    if constexpr (!std::is_trivially_destructible_v<T>) {
      storage_.destroy();
    }
    storage_.emplace(std::forward<TArgs>(args)...);
  }

  [[nodiscard]] T* get() { return storage_.get(); }
  [[nodiscard]] const T* get() const { return storage_.get(); }
  [[nodiscard]] T& operator*() { return *get(); }
  [[nodiscard]] const T& operator*() const { return *get(); }
  T* operator->() { return get(); }
  const T* operator->() const { return get(); }

 private:
  StorageFor<T> storage_;
};

} // namespace util
