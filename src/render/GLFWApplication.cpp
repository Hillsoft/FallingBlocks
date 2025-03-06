#include "render/GLFWApplication.h"

#include <iostream>
#include <stdexcept>
#include <GLFW/glfw3.h>

#include "render/glfw_wrapper/Window.h"

namespace tetris::render {

namespace {

glfw::Window makeWindow() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  return glfw::Window{800, 600, "Vulkan"};
}

} // namespace

GLFWApplication::GLFWApplication()
    : vulkan_(),
      graphics_(VulkanGraphicsDevice::make(vulkan_)),
      surface_(vulkan_, makeWindow()) {}

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