#include "render/GLFWApplication.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <GLFW/glfw3.h>

#include "render/Quad.hpp"
#include "render/glfw_wrapper/Window.hpp"

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
    result.emplace_back(device, device, VulkanFence{device, true});
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
      graphics_(VulkanGraphicsDevice::make(vulkan_, surface_)),
      vertexShader_(getQuadVertexShader(graphics_)),
      fragmentShader_(graphics_, "shaders/fragment.spv"),
      pipeline_(
          graphics_,
          graphics_.physicalInfo().swapChainSupport.preferredFormat.format,
          vertexShader_,
          fragmentShader_),
      presentStack_(graphics_, surface_, pipeline_.getRenderPass()),
      commandPool_(graphics_),
      commandBuffers_(makeCommandBuffers(graphics_, commandPool_)),
      synchronisationSets_(makeSynchronisationSets(graphics_)),
      vertexAttributes_(getQuadVertexAttributesBuffer(graphics_)) {
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
  synchronisationSets_[currentFrame_].inFlightFence.wait();

  if (shouldResetSwapChain_) {
    resetSwapChain();
    shouldResetSwapChain_ = false;
    return;
  }

  VulkanPresentStack::FrameData presentFrame = presentStack_.getNextImageIndex(
      &synchronisationSets_[currentFrame_].imageAvailableSemaphore, nullptr);

  if (presentFrame.refreshRequired()) {
    resetSwapChain();
    return;
  }

  synchronisationSets_[currentFrame_].inFlightFence.reset();

  commandBuffers_[currentFrame_].runRenderPass(
      pipeline_, presentFrame.getFrameBuffer(), [&](VkCommandBuffer buffer) {
        VkBuffer vertexBuffer = vertexAttributes_.getRawBuffer();
        VkDeviceSize offsets = 0;
        vkCmdBindVertexBuffers(buffer, 0, 1, &vertexBuffer, &offsets);
        vkCmdDraw(buffer, 6, 1, 0, 0);
      });

  commandBuffers_[currentFrame_].submit(
      {&synchronisationSets_[currentFrame_].imageAvailableSemaphore},
      {&synchronisationSets_[currentFrame_].renderFinishedSemaphore},
      &synchronisationSets_[currentFrame_].inFlightFence);

  presentFrame.present(
      &synchronisationSets_[currentFrame_].renderFinishedSemaphore);

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
