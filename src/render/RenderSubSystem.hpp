#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "render/ForwardAllocateMappedBuffer.hpp"
#include "render/RenderableObject.hpp"
#include "render/Simple2DCamera.hpp"
#include "render/VulkanCommandBuffer.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanInstance.hpp"
#include "render/Window.hpp"
#include "render/resource/ShaderProgramManager.hpp"
#include "render/resource/TextureManager.hpp"
#include "render/vulkan/UniqueHandle.hpp"
#include "util/BlockForwardAllocatedArena.hpp"
#include "util/IndexedResourceStorage.hpp"
#include "util/debug.hpp"
#include "util/portability.hpp"

#ifndef NDEBUG
#include "render/VulkanDebugMessenger.hpp"
#endif

namespace blocks::render {

constexpr int kMaxFramesInFlight = 2;

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

  [[nodiscard]] Window* get();
  [[nodiscard]] const Window* get() const;
  Window& operator*() { return *get(); }
  const Window& operator*() const { return *get(); }
  Window* operator->() { return get(); }
  const Window* operator->() const { return get(); }
};

class UniqueWindowHandle {
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

  UniqueWindowHandle(const UniqueWindowHandle& other) = delete;
  UniqueWindowHandle& operator=(const UniqueWindowHandle& other) = delete;

  WindowRef get() { return ref_; }
  WindowRef operator*() { return ref_; }
  WindowRef operator->() { return ref_; }

 private:
  WindowRef ref_;
};

struct GenericRenderableRef {
 public:
  explicit GenericRenderableRef() : id(0), renderSystem_(nullptr) {}
  explicit GenericRenderableRef(size_t id, RenderSubSystem& renderSystem)
      : id(id), renderSystem_(&renderSystem) {}

 private:
  size_t id;
  RenderSubSystem* renderSystem_;

 public:
  friend class RenderSubSystem;

  RenderSubSystem* renderSystem() { return renderSystem_; }

  [[nodiscard]] RenderableObject* get();
  [[nodiscard]] const RenderableObject* get() const;
  RenderableObject& operator*() { return *get(); }
  const RenderableObject& operator*() const { return *get(); }
  RenderableObject* operator->() { return get(); }
  const RenderableObject* operator->() const { return get(); }
};

template <typename TInstanceData>
struct RenderableRef {
 public:
  explicit RenderableRef() = default;
  explicit RenderableRef(size_t id, RenderSubSystem& renderSystem)
      : rawRef_(id, renderSystem) {
    // Temporarily removed as it's only safe to call get() on the render thread
    // DEBUG_ASSERT(sizeof(TInstanceData) == get()->getInstanceDataSize());
  }

 private:
  GenericRenderableRef rawRef_;

 public:
  friend class RenderSubSystem;

  RenderSubSystem* renderSystem() { return rawRef_.renderSystem(); }

  [[nodiscard]] RenderableObject* get() { return rawRef_.get(); }
  [[nodiscard]] const RenderableObject* get() const { return rawRef_.get(); }
  RenderableObject& operator*() { return *get(); }
  const RenderableObject& operator*() const { return *get(); }
  RenderableObject* operator->() { return get(); }
  const RenderableObject* operator->() const { return get(); }
};

template <typename TInstanceData>
class UniqueRenderableHandle {
 public:
  explicit UniqueRenderableHandle(RenderableRef<TInstanceData> ref)
      : ref_(ref) {}
  ~UniqueRenderableHandle() {
    if (ref_.renderSystem() != nullptr) {
      ref_.renderSystem()->destroyRenderable(ref_);
    }
  }

  UniqueRenderableHandle(UniqueRenderableHandle&& other) noexcept : ref_() {
    std::swap(ref_, other.ref_);
  }
  UniqueRenderableHandle& operator=(UniqueRenderableHandle&& other) noexcept {
    std::swap(ref_, other.ref_);
    return *this;
  }

  UniqueRenderableHandle(const UniqueRenderableHandle& other) = delete;
  UniqueRenderableHandle& operator=(const UniqueRenderableHandle& other) =
      delete;

  [[nodiscard]] RenderableRef<TInstanceData> get() const { return ref_; }
  RenderableRef<TInstanceData> operator*() const { return ref_; }
  RenderableRef<TInstanceData> operator->() const { return ref_; }

 private:
  RenderableRef<TInstanceData> ref_;
};

class RenderSubSystem {
 private:
  struct DrawCommand {
    DrawCommand(
        WindowRef target,
        long z,
        GenericRenderableRef obj,
        Simple2DCamera* camera,
        void* instanceData)
        : target_(target),
          z_(z),
          obj_(obj),
          camera_(camera),
          instanceData_(instanceData) {}

    WindowRef target_;
    long z_;
    GenericRenderableRef obj_;
    Simple2DCamera* camera_;

    void* instanceData_;
  };

 public:
  RenderSubSystem();

  struct PipelineSynchronisationSet {
    vulkan::UniqueHandle<VkSemaphore> imageAvailableSemaphore;
    vulkan::UniqueHandle<VkSemaphore> renderFinishedSemaphore;
    vulkan::UniqueHandle<VkFence> inFlightFence;
  };

  VulkanGraphicsDevice& getGraphicsDevice() { return graphics_; }
  Simple2DCamera& getDefaultCamera() { return defaultCamera_; }

  UniqueWindowHandle createWindow();
  void destroyWindow(WindowRef ref);

  Window* getWindow(WindowRef ref);

  template <typename TConcreteRenderable, typename... TArgs>
  UniqueRenderableHandle<typename TConcreteRenderable::InstanceData>
  createRenderable(TArgs&&... args) {
    const size_t id = renderables_.pushBackBlocking(
        TConcreteRenderable::create(
            std::forward<TArgs>(args)...,
            graphics_,
            shaderProgramManager_,
            textureManager_,
            kMaxFramesInFlight));
    return UniqueRenderableHandle{
        RenderableRef<typename TConcreteRenderable::InstanceData>{id, *this}};
  }

  void destroyRenderable(GenericRenderableRef ref);
  template <typename TInstanceData>
  void destroyRenderable(RenderableRef<TInstanceData> ref) {
    destroyRenderable(ref.rawRef_);
  }

  RenderableObject* getRenderable(GenericRenderableRef ref);

  template <typename TInstanceData>
  void drawObject(
      WindowRef target,
      long z,
      RenderableRef<TInstanceData> ref,
      const TInstanceData& instanceData) {
    drawObjectRaw(
        target,
        nullptr,
        z,
        ref.rawRef_,
        instanceDataCPUBuffer_.allocate<TInstanceData>(instanceData));
  }
  template <typename TInstanceData>
  void drawObject(
      WindowRef target,
      Simple2DCamera* camera,
      long z,
      RenderableRef<TInstanceData> ref,
      const TInstanceData& instanceData) {
    drawObjectRaw(
        target,
        camera,
        z,
        ref.rawRef_,
        instanceDataCPUBuffer_.allocate<TInstanceData>(instanceData));
  }

  void commitFrame();

  void waitIdle();

 private:
  void drawObjectRaw(
      WindowRef target,
      Simple2DCamera* camera,
      long z,
      GenericRenderableRef ref,
      void* instanceData);

  void drawWindow(
      size_t windowId,
      std::span<DrawCommand> windowCommands,
      std::vector<std::optional<RenderableObject>>& renderablesVec);

  struct GLFWLifetimeScope {
    GLFWLifetimeScope();
    ~GLFWLifetimeScope();

    GLFWLifetimeScope(const GLFWLifetimeScope& other) = delete;
    GLFWLifetimeScope& operator=(const GLFWLifetimeScope& other) = delete;

    GLFWLifetimeScope(GLFWLifetimeScope&& other) = delete;
    GLFWLifetimeScope& operator=(GLFWLifetimeScope&& other) = delete;
  };

  NO_UNIQUE_ADDRESS GLFWLifetimeScope lifetimeScope_;
  VulkanInstance instance_;
#ifndef NDEBUG
  VulkanDebugMessenger debugMessenger_;
#endif
  VulkanGraphicsDevice graphics_;
  VulkanCommandPool commandPool_;
  VulkanCommandPool loadingCommandPool_;
  std::vector<VulkanCommandBuffer> commandBuffers_;
  vulkan::UniqueHandle<VkRenderPass> mainRenderPass_;
  ShaderProgramManager shaderProgramManager_;
  TextureManager textureManager_;
  std::vector<PipelineSynchronisationSet> synchronisationSets_;
  std::vector<std::unique_ptr<Window>> windows_;
  util::IndexedResourceStorage<RenderableObject> renderables_;
  std::vector<RenderableObject> renderablesPendingDestruction_;
  std::vector<ForwardAllocateMappedBuffer> instanceDataBuffers_;
  util::BlockForwardAllocatedArena instanceDataCPUBuffer_;
  Simple2DCamera defaultCamera_;

  std::vector<DrawCommand> commands_;

  uint32_t currentFrame_ = 0;
};

} // namespace blocks::render
