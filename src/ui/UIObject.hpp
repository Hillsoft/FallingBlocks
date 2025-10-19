#pragma once

#include <cstdint>
#include <limits>
#include <memory>
#include <vector>
#include "math/vec.hpp"
#include "render/Simple2DCamera.hpp"

namespace blocks::ui {

enum class LayoutDirection { HORIZONTAL, VERTICAL };

class UIObject {
 public:
  virtual ~UIObject() = default;

  // Returns z-step
  virtual int draw(
      math::Vec<uint16_t, 2> minPos,
      math::Vec<uint16_t, 2> maxPos,
      render::Simple2DCamera& camera,
      int baseZ) {
    return 0;
  }

  uint16_t minWidth_ = 0;
  uint16_t maxWidth_ = std::numeric_limits<uint16_t>::max();
  uint16_t minHeight_ = 0;
  uint16_t maxHeight_ = std::numeric_limits<uint16_t>::max();

  LayoutDirection childLayoutDirection_ = LayoutDirection::HORIZONTAL;

  std::vector<std::unique_ptr<UIObject>> children_;

  uint16_t resolvedMinWidth_ = 0;
  uint16_t resolvedMinHeight_ = 0;
  uint16_t resolvedWidth_ = 0;
  uint16_t resolvedHeight_ = 0;
};

} // namespace blocks::ui
