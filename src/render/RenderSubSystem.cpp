#include "render/RenderSubSystem.hpp"

#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanCommandBuffer.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/Window.hpp"
#include "render/vulkan/FenceBuilder.hpp"
#include "render/vulkan/RenderPassBuilder.hpp"
#include "render/vulkan/SemaphoreBuilder.hpp"
#include "render/vulkan/UniqueHandle.hpp"
#include "util/debug.hpp"

namespace blocks::render {

namespace {

constexpr int kMaxFramesInFlight = 2;

std::vector<VulkanCommandBuffer> makeCommandBuffers(
    VulkanGraphicsDevice& device, VulkanCommandPool& commandPool) {
  std::vector<VulkanCommandBuffer> commandBuffers;
  commandBuffers.reserve(kMaxFramesInFlight);

  for (int i = 0; i < kMaxFramesInFlight; i++) {
    commandBuffers.emplace_back(device, commandPool);
  }

  return commandBuffers;
}

std::vector<RenderSubSystem::PipelineSynchronisationSet>
makeSynchronisationSets(VulkanGraphicsDevice& device) {
  std::vector<RenderSubSystem::PipelineSynchronisationSet> result;
  result.reserve(kMaxFramesInFlight);

  for (int i = 0; i < kMaxFramesInFlight; i++) {
    result.emplace_back(
        vulkan::SemaphoreBuilder{}.build(device.getRawDevice()),
        vulkan::SemaphoreBuilder{}.build(device.getRawDevice()),
        vulkan::FenceBuilder{}.setInitiallySignalled(true).build(
            device.getRawDevice()));
  }

  return result;
}

vulkan::UniqueHandle<VkRenderPass> makeMainRenderPass(VkDevice device) {
  vulkan::RenderPassBuilder builder{};

  VkAttachmentReference colorAttachmentRef =
      builder.addAttachmentGetDescription(
          {.format = VK_FORMAT_B8G8R8A8_SRGB,
           .samples = VK_SAMPLE_COUNT_1_BIT,
           .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
           .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
           .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
           .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
           .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
           .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR},
          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  uint32_t mainSubpassIndex = builder.addSubpassGetIndex(
      VK_PIPELINE_BIND_POINT_GRAPHICS, {}, {&colorAttachmentRef, 1});

  builder.addSubpassDependency(
      {.srcSubpass = VK_SUBPASS_EXTERNAL,
       .dstSubpass = mainSubpassIndex,
       .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
       .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
       .srcAccessMask = 0,
       .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
       .dependencyFlags = 0});

  return builder.build(device);
}

} // namespace

Window* WindowRef::get() {
  return renderSystem->getWindow(*this);
}

const Window* WindowRef::get() const {
  return renderSystem->getWindow(*this);
}

UniqueWindowHandle::~UniqueWindowHandle() {
  if (ref_.renderSystem != nullptr) {
    ref_.renderSystem->destroyWindow(ref_);
  }
}

RenderSubSystem::RenderSubSystem()
    : instance_(),
#ifndef NDEBUG
      debugMessenger_(instance_),
#endif
      graphics_(VulkanGraphicsDevice::make(instance_)),
      commandPool_(graphics_),
      commandBuffers_(makeCommandBuffers(graphics_, commandPool_)),
      mainRenderPass_(makeMainRenderPass(graphics_.getRawDevice())),
      synchronisationSets_(makeSynchronisationSets(graphics_)) {
}

RenderSubSystem::GLFWLifetimeScope::GLFWLifetimeScope() {
  int result = glfwInit();
  if (result == GLFW_FALSE) {
    throw std::runtime_error{"Failed to initialise GLFW"};
  }

  std::cout << "GLFW initialised\n";
}

RenderSubSystem::GLFWLifetimeScope::~GLFWLifetimeScope() {
  glfwTerminate();
}

UniqueWindowHandle RenderSubSystem::createWindow() {
  size_t id = windows_.size();
  windows_.emplace_back(std::make_unique<Window>(
      instance_,
      graphics_,
      VkRenderPass{mainRenderPass_.get()},
      800,
      600,
      "Vulkan"));
  return UniqueWindowHandle{WindowRef{id, *this}};
}

void RenderSubSystem::destroyWindow(WindowRef ref) {
  DEBUG_ASSERT(ref.id < windows_.size() && windows_[ref.id] != nullptr);
  windows_[ref.id].reset();
}

Window* RenderSubSystem::getWindow(WindowRef ref) {
  DEBUG_ASSERT(ref.id < windows_.size() && windows_[ref.id] != nullptr);
  return &*windows_[ref.id];
}

} // namespace blocks::render
