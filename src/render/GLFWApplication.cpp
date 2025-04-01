#include "render/GLFWApplication.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/Quad.hpp"
#include "render/VulkanCommandBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanPresentStack.hpp"

namespace blocks::render {

namespace {

constexpr int kMaxFramesInFlight = 2;

} // namespace

GLFWApplication::GLFWApplication()
    : render_(),
      window_(render_.createWindow()),
      renderable_(render_.createRenderable()),
      vertexShader_(getQuadVertexShader(render_.graphics_)),
      fragmentShader_(render_.graphics_, "shaders/fragment.spv"),
      descriptorSetLayout_(render_.graphics_),
      descriptorPool_(
          render_.graphics_, descriptorSetLayout_, kMaxFramesInFlight),
      pipeline_(
          render_.graphics_,
          VK_FORMAT_B8G8R8A8_SRGB,
          vertexShader_,
          fragmentShader_,
          descriptorSetLayout_),
      vertexAttributes_(getQuadVertexAttributesBuffer(render_.graphics_)),
      texture_(
          render_.graphics_,
          render_.commandPool_,
          RESOURCE_DIR "/mandelbrot set.png") {
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
      render_.graphics_.getRawDevice(),
      static_cast<uint32_t>(descriptorWrites.size()),
      descriptorWrites.data(),
      0,
      nullptr);
}

void GLFWApplication::run() {
  while (!window_->shouldClose()) {
    std::chrono::microseconds maxFrameTime{0};
    std::chrono::microseconds totalFrameTime{0};

    for (int i = 0; i < 1000 && !window_->shouldClose(); i++) {
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

  vkDeviceWaitIdle(render_.graphics_.getRawDevice());
}

void GLFWApplication::drawFrame() {
  vkWaitForFences(
      render_.graphics_.getRawDevice(),
      1,
      &render_.synchronisationSets_[currentFrame_].inFlightFence.get(),
      true,
      UINT64_MAX);

  if (window_->requiresReset()) {
    window_->resetSwapChain();
  }
  if (!window_->isDrawable()) {
    // Presume the window will only become drawable after an event
    glfwWaitEvents();
  }

  VulkanPresentStack::FrameData presentFrame =
      window_->presentStack_.getNextImageIndex(
          render_.synchronisationSets_[currentFrame_]
              .imageAvailableSemaphore.get(),
          nullptr);

  if (presentFrame.refreshRequired()) {
    window_->resetSwapChain();
    return;
  }

  vkResetFences(
      render_.graphics_.getRawDevice(),
      1,
      &render_.synchronisationSets_[currentFrame_].inFlightFence.get());

  render_.commandBuffers_[currentFrame_].runRenderPass(
      pipeline_,
      presentFrame.getFrameBuffer(),
      window_->presentStack_.extent(),
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

  render_.commandBuffers_[currentFrame_].submit(
      {render_.synchronisationSets_[currentFrame_]
           .imageAvailableSemaphore.get()},
      {render_.synchronisationSets_[currentFrame_]
           .renderFinishedSemaphore.get()},
      render_.synchronisationSets_[currentFrame_].inFlightFence.get());

  presentFrame.present(
      render_.synchronisationSets_[currentFrame_]
          .renderFinishedSemaphore.get());

  currentFrame_ = (currentFrame_ + 1) % kMaxFramesInFlight;
}

} // namespace blocks::render
