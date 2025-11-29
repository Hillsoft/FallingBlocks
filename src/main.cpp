#include "Application.hpp"
#include "GlobalSubSystemStack.hpp"

int main() {
  // NOLINTNEXTLINE(misc-const-correctness)
  blocks::GlobalSubSystemStack engineSystems{};
  blocks::Application vulkan{"Scene_Loading", "Scene_MainMenu"};

  vulkan.run();

  return 0;
}
