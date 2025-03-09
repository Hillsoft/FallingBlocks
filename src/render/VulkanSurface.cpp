#include "render/VulkanSurface.hpp"

#include <stdexcept>
#include <utility>

namespace blocks::render {

VulkanSurface::VulkanSurface(VulkanInstance& instance, glfw::Window window)
    : instance_(&instance), window_(std::move(window)), surface_(nullptr) {
  VkResult result = glfwCreateWindowSurface(
      instance.getRawInstance(), window_.getRawWindow(), nullptr, &surface_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create window surface"};
  }
}

VulkanSurface::~VulkanSurface() {
  if (surface_ != nullptr) {
    vkDestroySurfaceKHR(instance_->getRawInstance(), surface_, nullptr);
  }
}

VulkanSurface::VulkanSurface(VulkanSurface&& other) noexcept
    : instance_(other.instance_),
      window_(std::move(other.window_)),
      surface_(other.surface_) {
  other.surface_ = nullptr;
}

VulkanSurface& VulkanSurface::operator=(VulkanSurface&& other) noexcept {
  std::swap(instance_, other.instance_);
  std::swap(window_, other.window_);
  std::swap(surface_, other.surface_);

  return *this;
}

} // namespace blocks::render
