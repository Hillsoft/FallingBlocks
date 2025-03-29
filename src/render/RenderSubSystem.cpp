#include "render/RenderSubSystem.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanCommandBuffer.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/FenceBuilder.hpp"
#include "render/vulkan/SemaphoreBuilder.hpp"

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

} // namespace

RenderSubSystem::RenderSubSystem()
    : instance_(),
#ifndef NDEBUG
      debugMessenger_(instance_),
#endif
      graphics_(VulkanGraphicsDevice::make(instance_)),
      commandPool_(graphics_),
      commandBuffers_(makeCommandBuffers(graphics_, commandPool_)),
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

} // namespace blocks::render
