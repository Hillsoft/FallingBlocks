#pragma once

#include <typeindex>
#include <vector>

namespace blocks {

template <typename TSentinel>
concept ResourceSentinel = requires() {
  TSentinel::load();
  TSentinel::unload();
};

class ResourceSentinelManager {
 private:
  struct SentinelPointer {
   public:
    void (*load)();
    void (*unload)();
    std::type_index (*typeHash)();
  };

  template <ResourceSentinel TSentinel>
  constexpr static SentinelPointer sentinelPointerInstance{
      .load = []() { TSentinel::load(); },
      .unload = []() { TSentinel::unload(); },
      .typeHash = []() { return std::type_index{typeid(TSentinel)}; }};

  void transitionToSentinelSet(std::vector<SentinelPointer> newSentinels);

 public:
  ~ResourceSentinelManager();

  template <ResourceSentinel... Sentinels>
  void transitionToSentinelSet() {
    transitionToSentinelSet({sentinelPointerInstance<Sentinels>...});
  }

 private:
  std::vector<SentinelPointer> activeSentinels_;
};

} // namespace blocks
