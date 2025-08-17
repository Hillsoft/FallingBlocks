#include "engine/DrawableRegistry.hpp"

namespace blocks {

void DrawableRegistry::drawAll() {
  for (auto& item : *getRegisteredItems()) {
    item->draw();
  }
}

} // namespace blocks
