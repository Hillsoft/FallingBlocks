#include "engine/TickRegistry.hpp"

namespace blocks {

void TickRegistry::update(float deltaTimeSeconds) {
  for (auto& item : *getRegisteredItems()) {
    item->update(deltaTimeSeconds);
  }
}

} // namespace blocks
