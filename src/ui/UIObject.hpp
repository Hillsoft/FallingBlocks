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

  uint16_t& minSizeLayoutAxis(LayoutDirection layoutDirection) {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? minWidth_
        : minHeight_;
  }
  const uint16_t& minSizeLayoutAxis(LayoutDirection layoutDirection) const {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? minWidth_
        : minHeight_;
  }
  uint16_t& minSizeCrossLayoutAxis(LayoutDirection layoutDirection) {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? minHeight_
        : minWidth_;
  }
  const uint16_t& minSizeCrossLayoutAxis(
      LayoutDirection layoutDirection) const {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? minHeight_
        : minWidth_;
  }
  uint16_t& maxSizeLayoutAxis(LayoutDirection layoutDirection) {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? maxWidth_
        : maxHeight_;
  }
  const uint16_t& maxSizeLayoutAxis(LayoutDirection layoutDirection) const {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? maxWidth_
        : maxHeight_;
  }
  uint16_t& maxSizeCrossLayoutAxis(LayoutDirection layoutDirection) {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? maxHeight_
        : maxWidth_;
  }
  const uint16_t& maxSizeCrossLayoutAxis(
      LayoutDirection layoutDirection) const {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? maxHeight_
        : maxWidth_;
  }
};

} // namespace blocks::ui
