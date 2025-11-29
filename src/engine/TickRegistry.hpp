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
  explicit TickHandler(TickRegistry& registry) : RegistryItem(registry) {}
  virtual ~TickHandler() = default;

  TickHandler(const TickHandler& other) = delete;
  TickHandler& operator=(const TickHandler& other) = delete;

  TickHandler(TickHandler&& other) = delete;
  TickHandler& operator=(TickHandler&& other) = delete;

  virtual void update(float deltaTimeSeconds) {}
};

} // namespace blocks
