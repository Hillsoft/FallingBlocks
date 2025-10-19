#include "ui/Layout.hpp"

#include <algorithm>
#include <cstdint>
#include "math/vec.hpp"
#include "render/Simple2DCamera.hpp"
#include "ui/UIObject.hpp"
#include "util/debug.hpp"

namespace blocks::ui {

namespace {

void resolveMinimums(UIObject& object) {
  object.resolvedMinHeight_ = 0;
  object.resolvedMinWidth_ = 0;

  for (auto& child : object.children_) {
    resolveMinimums(*child);
  }

  if (object.childLayoutDirection_ == LayoutDirection::HORIZONTAL) {
    for (auto& child : object.children_) {
      object.resolvedMinHeight_ =
          std::max(object.resolvedMinHeight_, child->resolvedMinHeight_);
      object.resolvedMinWidth_ += child->resolvedMinWidth_;
    }
  } else if (object.childLayoutDirection_ == LayoutDirection::VERTICAL) {
    for (auto& child : object.children_) {
      object.resolvedMinWidth_ =
          std::max(object.resolvedMinWidth_, child->resolvedMinWidth_);
      object.resolvedMinHeight_ += child->resolvedMinHeight_;
    }
  } else {
    DEBUG_ASSERT(false);
  }
}

void distributeExcess(UIObject& object) {
  if (object.children_.empty()) {
    return;
  }

  for (auto& child : object.children_) {
    child->resolvedHeight_ = child->resolvedMinHeight_;
    child->resolvedWidth_ = child->resolvedMinWidth_;
  }

  if (object.childLayoutDirection_ == LayoutDirection::HORIZONTAL) {
    for (auto& child : object.children_) {
      child->resolvedHeight_ =
          std::min(object.resolvedHeight_, child->maxHeight_);
    }

    uint16_t childWidth = object.resolvedMinWidth_;
    size_t childrenWantingMore = object.children_.size();
    do {
      if (childrenWantingMore == 0 || object.resolvedWidth_ <= childWidth) {
        break;
      }
      uint16_t excess = object.resolvedWidth_ - childWidth;
      uint16_t perChild = static_cast<uint16_t>(excess / childrenWantingMore);
      if (perChild == 0) {
        break;
      }

      // update the calculation of this for the next iteration
      childrenWantingMore = object.children_.size();

      for (auto& child : object.children_) {
        uint16_t toGive = static_cast<uint16_t>(std::max(
            0,
            std::min<int>(
                perChild,
                static_cast<int>(child->maxWidth_) -
                    static_cast<int>(child->resolvedWidth_))));
        if (toGive == 0) {
          childrenWantingMore--;
        } else {
          child->resolvedWidth_ += toGive;
          childWidth += toGive;
        }
      }
    } while (true);
  } else if (object.childLayoutDirection_ == LayoutDirection::VERTICAL) {
    for (auto& child : object.children_) {
      child->resolvedWidth_ = std::min(object.resolvedWidth_, child->maxWidth_);
    }

    uint16_t childHeight = object.resolvedMinHeight_;
    size_t childrenWantingMore = object.children_.size();
    do {
      if (childrenWantingMore == 0 || object.resolvedHeight_ <= childHeight) {
        break;
      }
      uint16_t excess = object.resolvedHeight_ - childHeight;
      uint16_t perChild = static_cast<uint16_t>(excess / childrenWantingMore);
      if (perChild == 0) {
        break;
      }

      // update the calculation of this for the next iteration
      childrenWantingMore = object.children_.size();

      for (auto& child : object.children_) {
        uint16_t toGive = static_cast<uint16_t>(std::max(
            0,
            std::min<int>(
                perChild,
                static_cast<int>(child->maxHeight_) -
                    static_cast<int>(child->resolvedHeight_))));
        if (toGive == 0) {
          childrenWantingMore--;
        } else {
          child->resolvedHeight_ += toGive;
          childHeight += toGive;
        }
      }
    } while (true);
  }

  for (auto& child : object.children_) {
    distributeExcess(*child);
  }
}

void drawObjects(
    UIObject& object,
    render::Simple2DCamera& camera,
    math::Vec<uint16_t, 2> offset,
    int baseZ) {
  baseZ += object.draw(
      offset,
      offset +
          math::Vec<uint16_t, 2>{object.resolvedWidth_, object.resolvedHeight_},
      camera,
      baseZ);

  if (object.childLayoutDirection_ == LayoutDirection::HORIZONTAL) {
    for (auto& child : object.children_) {
      drawObjects(*child, camera, offset, baseZ);
      offset.x() += child->resolvedWidth_;
    }
  } else if (object.childLayoutDirection_ == LayoutDirection::VERTICAL) {
    for (auto& child : object.children_) {
      drawObjects(*child, camera, offset, baseZ);
      offset.y() += child->resolvedHeight_;
    }
  }
}

} // namespace

void layoutAndDraw(
    UIObject& object,
    render::Simple2DCamera& camera,
    math::Vec<uint16_t, 2> windowSize,
    int baseZ) {
  resolveMinimums(object);

  object.resolvedHeight_ = std::min(
      std::max(object.maxHeight_, object.resolvedMinHeight_), windowSize.y());
  object.resolvedWidth_ = std::min(
      std::max(object.maxWidth_, object.resolvedMinWidth_), windowSize.x());

  distributeExcess(object);

  drawObjects(
      object,
      camera,
      {static_cast<uint16_t>(0), static_cast<uint16_t>(0)},
      baseZ);
}

} // namespace blocks::ui
