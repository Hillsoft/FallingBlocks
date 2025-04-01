#pragma once

#include <optional>
#include <utility>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanCommandBuffer.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanInstance.hpp"
#include "render/Window.hpp"
#include "render/vulkan/UniqueHandle.hpp"

#ifndef NDEBUG
#include "render/VulkanDebugMessenger.hpp"
#endif

namespace blocks::render {

class GLFWApplication;
class RenderSubSystem;

struct WindowRef {
 private:
  explicit WindowRef(size_t id, RenderSubSystem& renderSystem)
      : id(id), renderSystem(&renderSystem) {}

  size_t id;
  RenderSubSystem* renderSystem;

 public:
  friend class RenderSubSystem;
  friend class UniqueWindowHandle;

  Window* get();
  const Window* get() const;
  Window& operator*() { return *get(); }
  const Window& operator*() const { return *get(); }
  Window* operator->() { return get(); }
  const Window* operator->() const { return get(); }
};

class UniqueWindowHandle {
 public:
  explicit UniqueWindowHandle(WindowRef ref) : ref_(ref) {}
  ~UniqueWindowHandle();

  UniqueWindowHandle(const UniqueWindowHandle& other) = delete;
  UniqueWindowHandle& operator=(const UniqueWindowHandle& other) = delete;

  UniqueWindowHandle(UniqueWindowHandle&& other) noexcept : ref_(other.ref_) {
    other.ref_.renderSystem = nullptr;
  }
  UniqueWindowHandle& operator=(UniqueWindowHandle&& other) noexcept {
    std::swap(ref_, other.ref_);
  }

  WindowRef get() { return ref_; }
  WindowRef operator*() { return ref_; }
  WindowRef operator->() { return ref_; }

 private:
  WindowRef ref_;
};

class RenderSubSystem {
 public:
  RenderSubSystem();

  struct PipelineSynchronisationSet {
    vulkan::UniqueHandle<VkSemaphore> imageAvailableSemaphore;
    vulkan::UniqueHandle<VkSemaphore> renderFinishedSemaphore;
    vulkan::UniqueHandle<VkFence> inFlightFence;
  };

  UniqueWindowHandle createWindow();
  void destroyWindow(WindowRef ref);

  Window* getWindow(WindowRef ref);

 private:
  struct GLFWLifetimeScope {
    GLFWLifetimeScope();
    ~GLFWLifetimeScope();
  };

  [[no_unique_address]] GLFWLifetimeScope lifetimeScope_;
  VulkanInstance instance_;
#ifndef NDEBUG
  VulkanDebugMessenger debugMessenger_;
#endif
  VulkanGraphicsDevice graphics_;
  VulkanCommandPool commandPool_;
  std::vector<VulkanCommandBuffer> commandBuffers_;
  vulkan::UniqueHandle<VkRenderPass> mainRenderPass_;
  std::vector<PipelineSynchronisationSet> synchronisationSets_;
  std::vector<std::optional<Window>> windows_;

 public:
  friend class GLFWApplication;
};

} // namespace blocks::render
