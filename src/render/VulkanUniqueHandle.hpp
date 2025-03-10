#pragma once

#include <type_traits>
#include <utility>
#include <GLFW/glfw3.h>

#include "render/VulkanUniqueHandleDeleters.hpp"

namespace blocks::render {

template <typename T>
class VulkanUniqueHandle {
 public:
  using Deleter = detail::HandleDeleter<T>;

  explicit VulkanUniqueHandle(T handle)
    requires(std::is_default_constructible_v<Deleter>)
      : handle_(handle), deleter_() {}
  explicit VulkanUniqueHandle(T handle, Deleter deleter)
      : handle_(handle), deleter_(std::move(deleter)) {}

  ~VulkanUniqueHandle() {
    if (handle_ != nullptr) {
      deleter_.destroy(handle_);
    }
  }

  VulkanUniqueHandle(const VulkanUniqueHandle& other) = delete;
  VulkanUniqueHandle& operator=(const VulkanUniqueHandle& other) = delete;

  VulkanUniqueHandle(VulkanUniqueHandle&& other) noexcept
      : handle_(other.handle_), deleter_(std::move(other.deleter_)) {
    other.handle_ = nullptr;
  }

  VulkanUniqueHandle& operator=(VulkanUniqueHandle&& other) noexcept {
    std::swap(handle_, other.handle_);
    std::swap(deleter_, other.deleter_);

    return *this;
  }

  T get() const { return handle_; }

 private:
  T handle_;
  [[no_unique_address]] Deleter deleter_;
};

} // namespace blocks::render
