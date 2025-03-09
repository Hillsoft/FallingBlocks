#include <iostream>

#include "render/GLFWApplication.hpp"

int main() {
  blocks::render::GLFWApplication vulkan{};

  vulkan.run();

  return 0;
}
