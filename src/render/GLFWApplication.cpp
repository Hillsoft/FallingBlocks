#include "render/GLFWApplication.hpp"

#include <iostream>
#include <stdexcept>
#include <GLFW/glfw3.h>

#include "render/glfw_wrapper/Window.hpp"

namespace tetris::render {

namespace {

glfw::Window makeWindow() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  return glfw::Window{800, 600, "Vulkan"};
}

std::vector<VulkanFrameBuffer> makeFrameBuffers(
    VulkanGraphicsDevice& device,
    VulkanRenderPass& renderPass,
    std::vector<VulkanImageView>& imageViews,
    VkExtent2D extent) {
  std::vector<VulkanFrameBuffer> result;
  result.reserve(imageViews.size());

  for (auto& v : imageViews) {
    result.emplace_back(device, renderPass, v, extent);
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
      swapChain_(surface_, graphics_),
      swapChainImageViews_(swapChain_.getImageViews()),
      vertexShader_(graphics_, std::filesystem::path{"shaders"} / "vertex.spv"),
      fragmentShader_(graphics_, "shaders/fragment.spv"),
      pipeline_(
          graphics_,
          swapChain_.getImageFormat(),
          vertexShader_,
          fragmentShader_),
      frameBuffers_(makeFrameBuffers(
          graphics_,
          pipeline_.getRenderPass(),
          swapChainImageViews_,
          swapChain_.getSwapchainExtent())),
      commandPool_(graphics_),
      commandBuffer_(graphics_, commandPool_),
      imageAvailableSemaphore_(graphics_),
      renderFinishedSemaphore_(graphics_),
      inFlightFence_(graphics_, true) {
}

GLFWApplication::~GLFWApplication() {
  glfwTerminate();
}

void GLFWApplication::run() {
  while (!surface_.window().shouldClose()) {
    glfwPollEvents();
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

} // namespace tetris::render