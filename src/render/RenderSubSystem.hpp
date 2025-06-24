#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/RenderableQuad.hpp"
#include "render/VulkanCommandBuffer.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanInstance.hpp"
#include "render/Window.hpp"
#include "render/vulkan/UniqueHandle.hpp"
#include "util/portability.hpp"
#include "util/raii_helpers.hpp"

#ifndef NDEBUG
#include "render/VulkanDebugMessenger.hpp"
#endif

namespace blocks::render {

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

class UniqueWindowHandle : private util::no_copy {
 public:
  explicit UniqueWindowHandle(WindowRef ref) : ref_(ref) {}
  ~UniqueWindowHandle();

  UniqueWindowHandle(UniqueWindowHandle&& other) noexcept : ref_(other.ref_) {
    other.ref_.renderSystem = nullptr;
  }
  UniqueWindowHandle& operator=(UniqueWindowHandle&& other) noexcept {
    std::swap(ref_, other.ref_);
    return *this;
  }

  WindowRef get() { return ref_; }
  WindowRef operator*() { return ref_; }
  WindowRef operator->() { return ref_; }

 private:
  WindowRef ref_;
};

struct RenderableRef {
 private:
  explicit RenderableRef(size_t id, RenderSubSystem& renderSystem)
      : id(id), renderSystem(&renderSystem) {}

  size_t id;
  RenderSubSystem* renderSystem;

 public:
  friend class RenderSubSystem;
  friend class UniqueRenderableHandle;

  RenderableQuad* get();
  const RenderableQuad* get() const;
  RenderableQuad& operator*() { return *get(); }
  const RenderableQuad& operator*() const { return *get(); }
  RenderableQuad* operator->() { return get(); }
  const RenderableQuad* operator->() const { return get(); }
};

class UniqueRenderableHandle : private util::no_copy {
 public:
  explicit UniqueRenderableHandle(RenderableRef ref) : ref_(ref) {}
  ~UniqueRenderableHandle();

  UniqueRenderableHandle(UniqueRenderableHandle&& other) noexcept
      : ref_(other.ref_) {
    other.ref_.renderSystem = nullptr;
  }
  UniqueRenderableHandle& operator=(UniqueRenderableHandle&& other) noexcept {
    std::swap(ref_, other.ref_);
    return *this;
  }

  RenderableRef get() { return ref_; }
  RenderableRef operator*() { return ref_; }
  RenderableRef operator->() { return ref_; }

 private:
  RenderableRef ref_;
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

  UniqueRenderableHandle createRenderable(
      const std::filesystem::path& texturePath);
  void destroyRenderable(RenderableRef ref);

  RenderableQuad* getRenderable(RenderableRef ref);

  void drawObject(WindowRef target, RenderableRef ref);

  void commitFrame();

  void waitIdle();

 private:
  void drawWindow(size_t windowId);

  struct GLFWLifetimeScope {
    GLFWLifetimeScope();
    ~GLFWLifetimeScope();
  };

  NO_UNIQUE_ADDRESS GLFWLifetimeScope lifetimeScope_;
  VulkanInstance instance_;
#ifndef NDEBUG
  VulkanDebugMessenger debugMessenger_;
#endif
  VulkanGraphicsDevice graphics_;
  VulkanCommandPool commandPool_;
  std::vector<VulkanCommandBuffer> commandBuffers_;
  vulkan::UniqueHandle<VkRenderPass> mainRenderPass_;
  std::vector<PipelineSynchronisationSet> synchronisationSets_;
  std::vector<std::unique_ptr<Window>> windows_;
  std::vector<std::optional<RenderableQuad>> renderables_;

  struct DrawCommand {
    WindowRef target_;
    RenderableRef obj_;
  };
  std::vector<DrawCommand> commands_;

  uint32_t currentFrame_ = 0;
};

} // namespace blocks::render
