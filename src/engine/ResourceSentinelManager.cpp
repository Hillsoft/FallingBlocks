#include "engine/ResourceSentinelManager.hpp"

#include <algorithm>
#include <utility>
#include <vector>

namespace blocks {

ResourceSentinelManager::~ResourceSentinelManager() {
  for (const auto& sentinel : activeSentinels_) {
    sentinel.unload();
  }
}

void ResourceSentinelManager::transitionToSentinelSet(
    std::vector<SentinelPointer> newSentinels) {
  for (const auto& sentinel : newSentinels) {
    auto active = std::find_if(
        activeSentinels_.begin(),
        activeSentinels_.end(),
        [&](const auto& activeSentinel) {
          return activeSentinel.typeHash() == sentinel.typeHash();
        });
    if (active != activeSentinels_.end()) {
      activeSentinels_.erase(active, active + 1);
    } else {
      sentinel.load();
    }
  }

  for (const auto& sentinel : activeSentinels_) {
    sentinel.unload();
  }

  activeSentinels_ = std::move(newSentinels);
}

} // namespace blocks
