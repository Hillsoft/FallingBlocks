#pragma once

#include "util/Registry.hpp"

namespace blocks {

class Drawable;

class DrawableRegistry : public util::Registry<Drawable, DrawableRegistry> {
 public:
  void drawAll();
};

class Drawable : public util::RegistryItem<DrawableRegistry, Drawable> {
 public:
  Drawable(DrawableRegistry& registry) : RegistryItem(registry) {}

  virtual void draw() {}
};

} // namespace blocks
