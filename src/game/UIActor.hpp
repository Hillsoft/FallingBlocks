#pragma once

#include <memory>
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "render/Simple2DCamera.hpp"
#include "ui/UIObject.hpp"

namespace blocks::game {

class UIActor : public Actor, public Drawable {
 public:
  UIActor(Scene& scene);
  UIActor(Scene& scene, std::unique_ptr<ui::UIObject> ui);

  void setUIObject(std::unique_ptr<ui::UIObject> newUI);

  void draw() override;

 private:
  render::Simple2DCamera uiCamera_;
  std::unique_ptr<ui::UIObject> ui_;
};

} // namespace blocks::game
