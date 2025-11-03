#include "engine/ResourceManager.hpp"

#include "GlobalSubSystemStack.hpp"

namespace blocks::engine {

ResourceManager& ResourceManager::get() {
  return GlobalSubSystemStack::get().resourceManager();
}

} // namespace blocks::engine
