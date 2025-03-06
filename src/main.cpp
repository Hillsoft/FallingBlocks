#include <iostream>

#include "render/GLFWApplication.hpp"

int main() {
  tetris::render::GLFWApplication vulkan{};

  vulkan.run();

  return 0;
}
