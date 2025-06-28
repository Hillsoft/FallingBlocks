#pragma once

#include "util/Registry.hpp"

namespace blocks {

class TickHandler;

class TickRegistry : public util::Registry<TickHandler, TickRegistry> {
 public:
  void update(float deltaTimeSeconds);
};

class TickHandler : public util::RegistryItem<TickRegistry, TickHandler> {
 public:
  TickHandler(TickRegistry& registry) : RegistryItem(registry) {}

  virtual void update(float deltaTimeSeconds) {}
};

} // namespace blocks
