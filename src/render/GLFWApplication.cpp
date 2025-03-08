#include "render/GLFWApplication.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <GLFW/glfw3.h>

#include "render/glfw_wrapper/Window.hpp"

namespace tetris::render {

namespace {

constexpr int kMaxFramesInFlight = 2;

glfw::Window makeWindow() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  return glfw::Window{800, 600, "Vulkan"};
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
      surface_(vulkan_, makeWindow()),
      graphics_(VulkanGraphicsDevice::make(vulkan_, surface_)),
      vertexShader_(graphics_, std::filesystem::path{"shaders"} / "vertex.spv"),
      fragmentShader_(graphics_, "shaders/fragment.spv"),
      pipeline_(
          graphics_,
          graphics_.physicalInfo().swapChainSupport.preferredFormat.format,
          vertexShader_,
          fragmentShader_),
      presentStack_(graphics_, surface_, pipeline_.getRenderPass()),
      commandPool_(graphics_),
      commandBuffers_(makeCommandBuffers(graphics_, commandPool_)),
      synchronisationSets_(makeSynchronisationSets(graphics_)) {
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
  synchronisationSets_[currentFrame].inFlightFence.waitAndReset();

  uint32_t imageIndex = presentStack_.getNextImageIndex(
      &synchronisationSets_[currentFrame].imageAvailableSemaphore, nullptr);

  commandBuffers_[currentFrame].runRenderPass(
      pipeline_,
      presentStack_.getFrameBuffer(imageIndex),
      [](VkCommandBuffer buffer) { vkCmdDraw(buffer, 3, 1, 0, 0); });

  commandBuffers_[currentFrame].submit(
      {&synchronisationSets_[currentFrame].imageAvailableSemaphore},
      {&synchronisationSets_[currentFrame].renderFinishedSemaphore},
      &synchronisationSets_[currentFrame].inFlightFence);

  presentStack_.present(
      imageIndex, &synchronisationSets_[currentFrame].renderFinishedSemaphore);

  currentFrame = (currentFrame + 1) % kMaxFramesInFlight;
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

} // namespace tetris::render