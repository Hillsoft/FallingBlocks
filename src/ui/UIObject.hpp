#pragma once

#include <cstdint>
#include <limits>
#include <vector>
#include "math/vec.hpp"
#include "render/Simple2DCamera.hpp"
#include "util/NotNull.hpp"

namespace blocks::ui {

enum class LayoutDirection { HORIZONTAL, VERTICAL };
enum class Align { LEFT_TOP, MIDDLE, RIGHT_BOTTOM };

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
  uint16_t outerPadding_ = 0;
  uint16_t innerPadding_ = 0;

  LayoutDirection childLayoutDirection_ = LayoutDirection::HORIZONTAL;
  Align crossLayoutPosition_ = Align::LEFT_TOP;

  std::vector<util::NotNullUniquePtr<UIObject>> children_;
};

} // namespace blocks::ui
