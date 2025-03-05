#include <iostream>

#include "render/GLFWApplication.h"

int main() {
  tetris::render::GLFWApplication vulkan{};

  vulkan.run();

  return 0;
}
