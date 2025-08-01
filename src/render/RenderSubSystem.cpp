#include "render/RenderSubSystem.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/ForwardAllocateMappedBuffer.hpp"
#include "render/RenderableQuad.hpp"
#include "render/VulkanCommandBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanPresentStack.hpp"
#include "render/Window.hpp"
#include "render/vulkan/FenceBuilder.hpp"
#include "render/vulkan/RenderPassBuilder.hpp"
#include "render/vulkan/SemaphoreBuilder.hpp"
#include "render/vulkan/UniqueHandle.hpp"
#include "util/Generator.hpp"
#include "util/debug.hpp"
#include "util/vec_generators.hpp"

namespace blocks::render {

namespace {

constexpr int kMaxFramesInFlight = 2;

RenderSubSystem::PipelineSynchronisationSet makeSynchronisationSet(
    VulkanGraphicsDevice& device) {
  return {
      vulkan::SemaphoreBuilder{}.build(device.getRawDevice()),
      vulkan::SemaphoreBuilder{}.build(device.getRawDevice()),
      vulkan::FenceBuilder{}.setInitiallySignalled(true).build(
          device.getRawDevice())};
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

RenderableQuad* GenericRenderableRef::get() {
  return renderSystem_->getRenderable(*this);
}

const RenderableQuad* GenericRenderableRef::get() const {
  return renderSystem_->getRenderable(*this);
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
      commandPool_(graphics_, false),
      loadingCommandPool_(graphics_, true),
      commandBuffers_(),
      mainRenderPass_(makeMainRenderPass(graphics_.getRawDevice())),
      shaderProgramManager_(graphics_, mainRenderPass_.get()),
      textureManager_(graphics_, {graphics_, true}),
      synchronisationSets_(),
      instanceDataBuffers_([&]() {
        std::vector<ForwardAllocateMappedBuffer> result;
        result.reserve(kMaxFramesInFlight);
        for (int i = 0; i < kMaxFramesInFlight; i++) {
          result.emplace_back(graphics_, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        }
        return result;
      }()) {
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
      800,
      "Vulkan"));

  for (size_t i = 0; i < kMaxFramesInFlight; i++) {
    synchronisationSets_.emplace_back(makeSynchronisationSet(graphics_));
    commandBuffers_.emplace_back(graphics_, commandPool_);
  }

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

RenderableQuad* RenderSubSystem::getRenderable(GenericRenderableRef ref) {
  DEBUG_ASSERT(
      ref.id < renderables_.size() && renderables_[ref.id].has_value());
  return &*renderables_[ref.id];
}

UniqueRenderableHandle<UniformData> RenderSubSystem::createRenderable(
    const std::filesystem::path& texturePath) {
  size_t id = renderables_.size();
  renderables_.emplace_back(
      std::in_place,
      texturePath,
      graphics_,
      shaderProgramManager_,
      textureManager_,
      kMaxFramesInFlight);
  return UniqueRenderableHandle{RenderableRef<UniformData>{id, *this}};
}

void RenderSubSystem::destroyRenderable(GenericRenderableRef ref) {
  DEBUG_ASSERT(
      ref.id < renderables_.size() && renderables_[ref.id].has_value());
  renderablesPendingDestruction_.emplace_back(std::move(*renderables_[ref.id]));
  renderables_[ref.id].reset();
}

void RenderSubSystem::drawObjectRaw(
    WindowRef target, GenericRenderableRef ref, void* instanceData) {
  commands_.emplace_back(target, ref, instanceData);
}

void RenderSubSystem::commitFrame() {
  std::vector<VkFence> fences;
  fences.reserve(windows_.size());
  for (size_t i = 0; i < windows_.size(); i++) {
    fences.emplace_back(
        synchronisationSets_[i * kMaxFramesInFlight + currentFrame_]
            .inFlightFence.get());
  }

  vkWaitForFences(
      graphics_.getRawDevice(),
      static_cast<uint32_t>(fences.size()),
      fences.data(),
      true,
      UINT64_MAX);

  instanceDataBuffers_[currentFrame_].reset();

  // Pre-sort commands by window
  if (windows_.size() > 1) {
    std::sort(
        commands_.begin(), commands_.end(), [](const auto& a, const auto& b) {
          return a.target_.id < b.target_.id;
        });
  }

  util::Generator<std::span<DrawCommand>> windowGroups = util::vec::genGroups(
      std::span<DrawCommand>{commands_.begin(), commands_.end()},
      [](const DrawCommand& a, const DrawCommand& b) {
        return a.target_.id == b.target_.id;
      });

  for (size_t i = 0; i < windows_.size() && !windowGroups.isDone(); i++) {
    if (windows_[i] != nullptr) {
      // We may have draw commands for null windows, so need to find the right
      // windowGroup
      std::span<DrawCommand> curGroup;
      do {
        curGroup = windowGroups();
      } while (curGroup[0].target_.id != i && !windowGroups.isDone());
      // Just in case we hit the last group without finding any commands
      if (curGroup[0].target_.id != i) {
        curGroup = {};
      }
      drawWindow(i, curGroup);
    }
  }

  commands_.clear();
  instanceDataCPUBuffer_.reset();

  currentFrame_ = (currentFrame_ + 1) % kMaxFramesInFlight;
}

void RenderSubSystem::drawWindow(
    size_t windowId, std::span<DrawCommand> windowCommands) {
  DEBUG_ASSERT(windowId < windows_.size() && windows_[windowId] != nullptr);
  Window& window = *windows_[windowId];
  PipelineSynchronisationSet& synchronisationSet =
      synchronisationSets_[windowId * kMaxFramesInFlight + currentFrame_];
  ForwardAllocateMappedBuffer& instanceDataAllocator =
      instanceDataBuffers_[currentFrame_];

  if (window.requiresReset()) {
    window.resetSwapChain();
  }
  if (!window.isDrawable()) {
    // Presume the window will only become drawable after an event
    glfwWaitEvents();
  }

  VulkanPresentStack::FrameData presentFrame =
      window.getPresentStack().getNextImageIndex(
          synchronisationSet.imageAvailableSemaphore.get(), nullptr);

  if (presentFrame.refreshRequired()) {
    window.resetSwapChain();
    return;
  }

  VkExtent2D extent = window.getCurrentWindowExtent();

  vkResetFences(
      graphics_.getRawDevice(), 1, &synchronisationSet.inFlightFence.get());

  VkCommandBuffer commandBuffer =
      commandBuffers_[windowId * kMaxFramesInFlight + currentFrame_]
          .getRawBuffer();
  vkResetCommandBuffer(commandBuffer, 0);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to begin recording command buffer"};
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = mainRenderPass_.get();
  renderPassInfo.framebuffer = presentFrame.getFrameBuffer();
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = extent;

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(
      commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  for ([[maybe_unused]] const auto& command : windowCommands) {
    DEBUG_ASSERT(
        command.target_.id == windowId &&
        command.obj_.id < renderables_.size() &&
        renderables_[command.obj_.id].has_value());
  }

  std::sort(
      windowCommands.begin(),
      windowCommands.end(),
      [](const auto& a, const auto& b) { return a.obj_.id < b.obj_.id; });

  util::Generator<std::span<DrawCommand>> objGroups = util::vec::genGroups(
      windowCommands, [](const DrawCommand& a, const DrawCommand& b) {
        return a.obj_.id == b.obj_.id;
      });

  for (const auto& curGroup : objGroups) {
    // All commands in the group have the same renderable object,
    RenderableQuad& renderable = *renderables_[curGroup[0].obj_.id];

    ForwardAllocateMappedBuffer::Allocation instanceAlloc =
        instanceDataAllocator.alloc(
            renderable.instanceDataSize_ * curGroup.size());
    for (size_t i = 0; i < curGroup.size(); i++) {
      std::memcpy(
          reinterpret_cast<void*>(
              reinterpret_cast<size_t>(instanceAlloc.ptr) +
              i * renderable.instanceDataSize_),
          curGroup[i].instanceData_,
          renderable.instanceDataSize_);
    }

    vkCmdBindPipeline(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        renderable.shaderProgram_->pipeline_.getRawPipeline());

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        renderable.shaderProgram_->pipeline_.getPipelineLayout().getRawLayout(),
        0,
        1,
        &renderable.descriptorPool_.getDescriptorSets()[currentFrame_],
        0,
        nullptr);

    VkBuffer vertexBuffer = renderable.vertexAttributes_.getRawBuffer();
    std::array<VkBuffer, 2> vertexBuffers{vertexBuffer, instanceAlloc.buffer};
    std::array<VkDeviceSize, 2> offsets{0, instanceAlloc.bufferOffset};
    vkCmdBindVertexBuffers(
        commandBuffer, 0, 2, vertexBuffers.data(), offsets.data());
    vkCmdDraw(commandBuffer, 6, static_cast<uint32_t>(curGroup.size()), 0, 0);
  }

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to record command buffer"};
  }

  commandBuffers_[windowId * kMaxFramesInFlight + currentFrame_].submit(
      {synchronisationSet.imageAvailableSemaphore.get()},
      {synchronisationSet.renderFinishedSemaphore.get()},
      synchronisationSet.inFlightFence.get());

  presentFrame.present(synchronisationSet.renderFinishedSemaphore.get());
}

void RenderSubSystem::waitIdle() {
  vkDeviceWaitIdle(graphics_.getRawDevice());
}

} // namespace blocks::render
