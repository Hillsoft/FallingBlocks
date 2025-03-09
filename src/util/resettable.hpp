#pragma once

#include <type_traits>
#include <utility>

#include "util/storage.hpp"

namespace util {

template <typename T>
class Resettable {
 public:
  template <typename... TArgs>
  Resettable(TArgs&&... args) : storage_(std::forward<TArgs>(args)...) {}

  ~Resettable()
    requires(!std::is_trivially_destructible_v<T>)
  {
    storage_.destroy();
  }

  ~Resettable()
    requires(std::is_trivially_destructible_v<T>)
  = default;

  template <typename... TArgs>
  void reset(TArgs&&... args) {
    if constexpr (!std::is_trivially_destructible_v<T>) {
      storage_.destroy();
    }
    storage_.emplace(std::forward<TArgs>(args)...);
  }

  T* get() { return storage_.get(); }
  const T* get() const { return storage_.get(); }
  T& operator*() { return *get(); }
  const T& operator*() const { return *get(); }
  T* operator->() { return get(); }
  const T* operator->() const { return get(); }

 private:
  StorageFor<T> storage_;
};

} // namespace util
