#include "Application.hpp"
#include "GlobalSubSystemStack.hpp"

int main() {
  blocks::GlobalSubSystemStack engineSystems{};
  blocks::Application vulkan{};

  vulkan.run();

  return 0;
}
