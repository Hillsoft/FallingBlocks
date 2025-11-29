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
  explicit Drawable(DrawableRegistry& registry) : RegistryItem(registry) {}
  virtual ~Drawable() = default;

  Drawable(const Drawable& other) = delete;
  Drawable& operator=(const Drawable& other) = delete;

  Drawable(Drawable&& other) = delete;
  Drawable& operator=(Drawable&& other) = delete;

  virtual void draw() {}
};

} // namespace blocks
