#pragma once

#include <functional>
#include <string>
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "input/InputHandler.hpp"
#include "math/vec.hpp"

namespace blocks::game {

class ButtonResourceSentinel {
 public:
  static void load();
  static void unload();
};

class Button : public Actor, public Drawable, public input::InputHandler {
 public:
  Button(
      Scene& scene,
      math::Vec2 pos0,
      math::Vec2 pos1,
      std::string label,
      std::function<void()> onClick);

  void draw() final;

  void onMouseMove(math::Vec2 mousePos) final;
  void onMouseUp(math::Vec2 mousePos) final;

 private:
  math::Vec2 pos0_;
  math::Vec2 pos1_;
  std::string label_;
  bool hovered_;
  std::function<void()> onClick_;
};

} // namespace blocks::game
