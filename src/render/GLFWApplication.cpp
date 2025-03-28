#include "render/GLFWApplication.hpp"

#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/Quad.hpp"
#include "render/VulkanCommandBuffer.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanPresentStack.hpp"
#include "render/glfw_wrapper/Window.hpp"
#include "render/vulkan/FenceBuilder.hpp"
#include "render/vulkan/SemaphoreBuilder.hpp"

namespace blocks::render {

namespace {

constexpr int kMaxFramesInFlight = 2;

glfw::Window makeWindow(std::function<void(int, int)> resizeHandler) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  glfw::Window window{800, 600, "Vulkan"};
  window.setResizeHandler(std::move(resizeHandler));

  return window;
}

std::vector<VulkanCommandBuffer> makeCommandBuffers(
    VulkanGraphicsDevice& device, VulkanCommandPool& commandPool) {
  std::vector<VulkanCommandBuffer> commandBuffers;
  commandBuffers.reserve(kMaxFramesInFlight);

  for (int i = 0; i < kMaxFramesInFlight; i++) {
    commandBuffers.emplace_back(device, commandPool);
  }

  return commandBuffers;
}

std::vector<GLFWApplication::PipelineSynchronisationSet>
makeSynchronisationSets(VulkanGraphicsDevice& device) {
  std::vector<GLFWApplication::PipelineSynchronisationSet> result;
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

} // namespace

GLFWApplication::GLFWApplication()
    : vulkan_(),
#ifndef NDEBUG
      debugMessenger_(vulkan_),
#endif
      surface_(vulkan_, makeWindow([&](int /* width */, int /* height */) {
                 onWindowResize();
               })),
      graphics_(VulkanGraphicsDevice::make(vulkan_)),
      vertexShader_(getQuadVertexShader(graphics_)),
      fragmentShader_(graphics_, "shaders/fragment.spv"),
      descriptorSetLayout_(graphics_),
      descriptorPool_(graphics_, descriptorSetLayout_, kMaxFramesInFlight),
      pipeline_(
          graphics_,
          VK_FORMAT_B8G8R8A8_SRGB,
          vertexShader_,
          fragmentShader_,
          descriptorSetLayout_),
      presentStack_(graphics_, surface_, pipeline_.getRenderPass()),
      commandPool_(graphics_),
      commandBuffers_(makeCommandBuffers(graphics_, commandPool_)),
      synchronisationSets_(makeSynchronisationSets(graphics_)),
      vertexAttributes_(getQuadVertexAttributesBuffer(graphics_)),
      texture_(graphics_, commandPool_, RESOURCE_DIR "/mandelbrot set.png") {
  const auto& descriptorSets = descriptorPool_.getDescriptorSets();
  std::vector<VkWriteDescriptorSet> descriptorWrites;
  descriptorWrites.reserve(descriptorSets.size());

  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageInfo.imageView = texture_.getImageView().getRawImageView();
  imageInfo.sampler = texture_.getSampler();

  for (const auto& set : descriptorSets) {
    auto& curWrite = descriptorWrites.emplace_back();
    curWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    curWrite.dstSet = set;
    curWrite.dstBinding = 0;
    curWrite.dstArrayElement = 0;
    curWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    curWrite.descriptorCount = 1;
    curWrite.pImageInfo = &imageInfo;
  }

  vkUpdateDescriptorSets(
      graphics_.getRawDevice(),
      static_cast<uint32_t>(descriptorWrites.size()),
      descriptorWrites.data(),
      0,
      nullptr);
}

GLFWApplication::~GLFWApplication() {
  glfwTerminate();
}

void GLFWApplication::run() {
  while (!surface_.window().shouldClose()) {
    std::chrono::microseconds maxFrameTime{0};
    std::chrono::microseconds totalFrameTime{0};

    for (int i = 0; i < 1000 && !surface_.window().shouldClose(); i++) {
      auto start = std::chrono::high_resolution_clock::now();
      glfwPollEvents();
      drawFrame();
      auto end = std::chrono::high_resolution_clock::now();

      std::chrono::microseconds curFrameTime =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      totalFrameTime += curFrameTime;
      maxFrameTime = std::max(maxFrameTime, curFrameTime);
    }

    std::cout << "FPS: " << 1000000.0f / (totalFrameTime / 1000).count()
              << "\nAverage frame time: " << totalFrameTime / 1000
              << "\nMax frame time: " << maxFrameTime << "\n";
  }

  vkDeviceWaitIdle(graphics_.getRawDevice());
}

void GLFWApplication::drawFrame() {
  vkWaitForFences(
      graphics_.getRawDevice(),
      1,
      &synchronisationSets_[currentFrame_].inFlightFence.get(),
      true,
      UINT64_MAX);

  if (shouldResetSwapChain_) {
    resetSwapChain();
    shouldResetSwapChain_ = false;
    return;
  }

  VulkanPresentStack::FrameData presentFrame = presentStack_.getNextImageIndex(
      synchronisationSets_[currentFrame_].imageAvailableSemaphore.get(),
      nullptr);

  if (presentFrame.refreshRequired()) {
    resetSwapChain();
    return;
  }

  vkResetFences(
      graphics_.getRawDevice(),
      1,
      &synchronisationSets_[currentFrame_].inFlightFence.get());

  commandBuffers_[currentFrame_].runRenderPass(
      pipeline_,
      presentFrame.getFrameBuffer(),
      presentStack_.extent(),
      [&](VkCommandBuffer buffer) {
        vkCmdBindDescriptorSets(
            buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_.getPipelineLayout().getRawLayout(),
            0,
            1,
            &descriptorPool_.getDescriptorSets()[currentFrame_],
            0,
            nullptr);

        VkBuffer vertexBuffer = vertexAttributes_.getRawBuffer();
        VkDeviceSize offsets = 0;
        vkCmdBindVertexBuffers(buffer, 0, 1, &vertexBuffer, &offsets);
        vkCmdDraw(buffer, 6, 1, 0, 0);
      });

  commandBuffers_[currentFrame_].submit(
      {synchronisationSets_[currentFrame_].imageAvailableSemaphore.get()},
      {synchronisationSets_[currentFrame_].renderFinishedSemaphore.get()},
      synchronisationSets_[currentFrame_].inFlightFence.get());

  presentFrame.present(
      synchronisationSets_[currentFrame_].renderFinishedSemaphore.get());

  currentFrame_ = (currentFrame_ + 1) % kMaxFramesInFlight;
}

void GLFWApplication::onWindowResize() {
  shouldResetSwapChain_ = true;
}

void GLFWApplication::resetSwapChain() {
  std::pair<int, int> windowSize = surface_.window().getCurrentWindowSize();
  while ((windowSize.first == 0 || windowSize.second == 0) &&
         !surface_.window().shouldClose()) {
    glfwWaitEvents();
    windowSize = surface_.window().getCurrentWindowSize();
  }
  if (!(windowSize.first == 0 || windowSize.second == 0)) {
    presentStack_.reset();
  }
}

GLFWApplication::GLFWLifetimeScope::GLFWLifetimeScope() {
  int result = glfwInit();
  if (result == GLFW_FALSE) {
    throw std::runtime_error{"Failed to initialise GLFW"};
  }

  std::cout << "GLFW initialised\n";
}

GLFWApplication::GLFWLifetimeScope::~GLFWLifetimeScope() {
  glfwTerminate();
}

} // namespace blocks::render
