#include "Application.hpp"
#include "GlobalSubSystemStack.hpp"

int main() {
  blocks::GlobalSubSystemStack engineSystems{};
  blocks::Application vulkan{"Scene_Loading", "Scene_MainMenu"};

  vulkan.run();

  return 0;
}
