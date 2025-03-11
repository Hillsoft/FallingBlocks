#pragma once

#include <memory>
#include <type_traits>

#include "render/vulkan/UniqueHandleDeleters.hpp"

namespace blocks::render::vulkan {

template <typename T>
class UniqueHandle {
 public:
  using Deleter = detail::HandleDeleter<T>;

  explicit UniqueHandle(T handle)
    requires(std::is_default_constructible_v<Deleter>)
      : handle_(handle), deleter_() {}
  explicit UniqueHandle(T handle, Deleter deleter)
      : handle_(handle), deleter_(std::move(deleter)) {}

  ~UniqueHandle() {
    if (handle_ != nullptr) {
      deleter_.destroy(handle_);
    }
  }

  UniqueHandle(const UniqueHandle& other) = delete;
  UniqueHandle& operator=(const UniqueHandle& other) = delete;

  UniqueHandle(UniqueHandle&& other) noexcept
      : handle_(other.handle_), deleter_(std::move(other.deleter_)) {
    other.handle_ = nullptr;
  }

  UniqueHandle& operator=(UniqueHandle&& other) noexcept {
    std::swap(handle_, other.handle_);
    std::swap(deleter_, other.deleter_);

    return *this;
  }

  const T& get() const { return handle_; }

 private:
  T handle_;
  [[no_unique_address]] Deleter deleter_;
};

} // namespace blocks::render::vulkan
