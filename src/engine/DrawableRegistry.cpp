#include "engine/DrawableRegistry.hpp"

namespace blocks {

void DrawableRegistry::drawAll() {
  for (auto& item : *getRegisteredItemsWrite()) {
    item->draw();
  }
}

} // namespace blocks
