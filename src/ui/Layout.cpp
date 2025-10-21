#include "ui/Layout.hpp"

#include <algorithm>
#include <cstdint>
#include <span>
#include <vector>
#include "math/vec.hpp"
#include "render/Simple2DCamera.hpp"
#include "ui/UIObject.hpp"
#include "util/debug.hpp"

namespace blocks::ui {

namespace {

struct ResolvedLayoutDetails {
  uint16_t resolvedMinWidth = 0;
  uint16_t resolvedMinHeight = 0;
  uint16_t resolvedWidth = 0;
  uint16_t resolvedHeight = 0;
  size_t childStartIndex = 0;
};

void objectCountImpl(const UIObject& object, size_t& accum) {
  accum++;
  for (const auto& child : object.children_) {
    objectCountImpl(*child, accum);
  }
}

size_t objectCount(const UIObject& object) {
  size_t result = 0;
  objectCountImpl(object, result);
  return result;
}

void buildChildStartIndices(
    const UIObject& object,
    size_t curObjectIndex,
    size_t& nextFree,
    std::span<ResolvedLayoutDetails> resolvedDetails) {
  size_t curChildStartIndex = nextFree;
  resolvedDetails[curObjectIndex].childStartIndex = curChildStartIndex;
  nextFree += object.children_.size();
  for (size_t i = 0; i < object.children_.size(); i++) {
    buildChildStartIndices(
        *object.children_[i],
        curChildStartIndex + i,
        nextFree,
        resolvedDetails);
  }
}

void resolveMinimums(
    const UIObject& object,
    size_t objectIndex,
    std::span<ResolvedLayoutDetails> resolvedDetails) {
  ResolvedLayoutDetails& curObjectDetails = resolvedDetails[objectIndex];
  curObjectDetails.resolvedMinHeight = 0;
  curObjectDetails.resolvedMinWidth = 0;

  for (size_t i = 0; i < object.children_.size(); i++) {
    resolveMinimums(
        *object.children_[i],
        curObjectDetails.childStartIndex + i,
        resolvedDetails);
  }

  if (object.childLayoutDirection_ == LayoutDirection::HORIZONTAL) {
    for (size_t i = 0; i < object.children_.size(); i++) {
      const auto& child = object.children_[i];
      const auto& childObjectDetails =
          resolvedDetails[curObjectDetails.childStartIndex + i];

      curObjectDetails.resolvedMinHeight = std::max<uint16_t>(
          curObjectDetails.resolvedMinHeight,
          childObjectDetails.resolvedMinHeight + 2 * child->outerPadding_);
      curObjectDetails.resolvedMinWidth +=
          childObjectDetails.resolvedMinWidth + 2 * child->outerPadding_;
    }
  } else if (object.childLayoutDirection_ == LayoutDirection::VERTICAL) {
    for (size_t i = 0; i < object.children_.size(); i++) {
      const auto& child = object.children_[i];
      const auto& childObjectDetails =
          resolvedDetails[curObjectDetails.childStartIndex + i];

      curObjectDetails.resolvedMinWidth = std::max<uint16_t>(
          curObjectDetails.resolvedMinWidth,
          childObjectDetails.resolvedMinWidth + 2 * child->outerPadding_);
      curObjectDetails.resolvedMinHeight +=
          childObjectDetails.resolvedMinHeight + 2 * child->outerPadding_;
    }
  } else {
    DEBUG_ASSERT(false);
  }

  curObjectDetails.resolvedMinHeight =
      std::max<uint16_t>(
          object.minHeight_, curObjectDetails.resolvedMinHeight) +
      2 * object.innerPadding_;
  curObjectDetails.resolvedMinWidth =
      std::max<uint16_t>(object.minWidth_, curObjectDetails.resolvedMinWidth) +
      2 * object.innerPadding_;
}

void distributeExcess(
    const UIObject& object,
    size_t objectIndex,
    std::span<ResolvedLayoutDetails> resolvedDetails) {
  if (object.children_.empty()) {
    return;
  }

  const ResolvedLayoutDetails& curObjectDetails = resolvedDetails[objectIndex];

  for (size_t i = 0; i < object.children_.size(); i++) {
    auto& childObjectDetails =
        resolvedDetails[curObjectDetails.childStartIndex + i];
    childObjectDetails.resolvedHeight = childObjectDetails.resolvedMinHeight;
    childObjectDetails.resolvedWidth = childObjectDetails.resolvedMinWidth;
  }

  if (object.childLayoutDirection_ == LayoutDirection::HORIZONTAL) {
    for (size_t i = 0; i < object.children_.size(); i++) {
      const auto& child = object.children_[i];
      auto& childObjectDetails =
          resolvedDetails[curObjectDetails.childStartIndex + i];

      childObjectDetails.resolvedHeight = std::min<uint16_t>(
          curObjectDetails.resolvedHeight - 2 * object.innerPadding_ -
              2 * child->outerPadding_,
          child->maxHeight_);
    }

    uint16_t availableWidth = curObjectDetails.resolvedWidth;
    uint16_t childWidth = curObjectDetails.resolvedMinWidth;
    size_t childrenWantingMore = object.children_.size();
    do {
      if (childrenWantingMore == 0 || availableWidth <= childWidth) {
        break;
      }
      uint16_t excess = availableWidth - childWidth;
      uint16_t perChild = static_cast<uint16_t>(excess / childrenWantingMore);
      if (perChild == 0) {
        break;
      }

      // update the calculation of this for the next iteration
      childrenWantingMore = object.children_.size();

      for (size_t i = 0; i < object.children_.size(); i++) {
        const auto& child = object.children_[i];
        auto& childObjectDetails =
            resolvedDetails[curObjectDetails.childStartIndex + i];

        uint16_t toGive = static_cast<uint16_t>(std::max(
            0,
            std::min<int>(
                perChild,
                static_cast<int>(child->maxWidth_) -
                    static_cast<int>(childObjectDetails.resolvedWidth))));
        if (toGive == 0) {
          childrenWantingMore--;
        } else {
          childObjectDetails.resolvedWidth += toGive;
          childWidth += toGive;
        }
      }
    } while (true);
  } else if (object.childLayoutDirection_ == LayoutDirection::VERTICAL) {
    for (size_t i = 0; i < object.children_.size(); i++) {
      const auto& child = object.children_[i];
      auto& childObjectDetails =
          resolvedDetails[curObjectDetails.childStartIndex + i];

      childObjectDetails.resolvedWidth = std::min<uint16_t>(
          curObjectDetails.resolvedWidth - 2 * object.innerPadding_ -
              2 * child->outerPadding_,
          child->maxWidth_);
    }

    uint16_t availableHeight = curObjectDetails.resolvedHeight;
    uint16_t childHeight = curObjectDetails.resolvedMinHeight;
    size_t childrenWantingMore = object.children_.size();
    do {
      if (childrenWantingMore == 0 || availableHeight <= childHeight) {
        break;
      }
      uint16_t excess = availableHeight - childHeight;
      uint16_t perChild = static_cast<uint16_t>(excess / childrenWantingMore);
      if (perChild == 0) {
        break;
      }

      // update the calculation of this for the next iteration
      childrenWantingMore = object.children_.size();

      for (size_t i = 0; i < object.children_.size(); i++) {
        const auto& child = object.children_[i];
        auto& childObjectDetails =
            resolvedDetails[curObjectDetails.childStartIndex + i];

        uint16_t toGive = static_cast<uint16_t>(std::max(
            0,
            std::min<int>(
                perChild,
                static_cast<int>(child->maxHeight_) -
                    static_cast<int>(childObjectDetails.resolvedHeight))));
        if (toGive == 0) {
          childrenWantingMore--;
        } else {
          childObjectDetails.resolvedHeight += toGive;
          childHeight += toGive;
        }
      }
    } while (true);
  }

  for (size_t i = 0; i < object.children_.size(); i++) {
    distributeExcess(
        *object.children_[i],
        curObjectDetails.childStartIndex + i,
        resolvedDetails);
  }
}

void drawObjects(
    UIObject& object,
    size_t objectIndex,
    std::span<ResolvedLayoutDetails> resolvedDetails,
    render::Simple2DCamera& camera,
    math::Vec<uint16_t, 2> offset,
    int baseZ) {
  const ResolvedLayoutDetails& curObjectDetails = resolvedDetails[objectIndex];

  baseZ += object.draw(
      offset,
      offset +
          math::Vec<uint16_t, 2>{
              curObjectDetails.resolvedWidth, curObjectDetails.resolvedHeight},
      camera,
      baseZ);

  offset += math::Vec<uint16_t, 2>{object.innerPadding_, object.innerPadding_};

  if (object.childLayoutDirection_ == LayoutDirection::HORIZONTAL) {
    for (size_t i = 0; i < object.children_.size(); i++) {
      const auto& child = object.children_[i];
      auto& childObjectDetails =
          resolvedDetails[curObjectDetails.childStartIndex + i];

      uint16_t crossLayoutOffset = [&]() -> uint16_t {
        switch (child->crossLayoutPosition_) {
          case Align::LEFT_TOP:
            return 0;
          case Align::MIDDLE:
            return (curObjectDetails.resolvedHeight -
                    childObjectDetails.resolvedHeight -
                    2 * child->outerPadding_) /
                2;
          case Align::RIGHT_BOTTOM:
            return curObjectDetails.resolvedHeight -
                childObjectDetails.resolvedHeight - 2 * child->outerPadding_;
          default:
            DEBUG_ASSERT(false);
            return 0;
        }
      }();

      drawObjects(
          *child,
          curObjectDetails.childStartIndex + i,
          resolvedDetails,
          camera,
          math::Vec<uint16_t, 2>{
              static_cast<uint16_t>(offset.x() + child->outerPadding_),
              static_cast<uint16_t>(
                  offset.y() + child->outerPadding_ + crossLayoutOffset)},
          baseZ);
      offset.x() += childObjectDetails.resolvedWidth + 2 * child->outerPadding_;
    }
  } else if (object.childLayoutDirection_ == LayoutDirection::VERTICAL) {
    for (size_t i = 0; i < object.children_.size(); i++) {
      const auto& child = object.children_[i];
      auto& childObjectDetails =
          resolvedDetails[curObjectDetails.childStartIndex + i];

      uint16_t crossLayoutOffset = [&]() -> uint16_t {
        switch (child->crossLayoutPosition_) {
          case Align::LEFT_TOP:
            return 0;
          case Align::MIDDLE:
            return (curObjectDetails.resolvedWidth -
                    childObjectDetails.resolvedWidth -
                    2 * child->outerPadding_) /
                2;
          case Align::RIGHT_BOTTOM:
            return curObjectDetails.resolvedWidth -
                childObjectDetails.resolvedWidth - 2 * child->outerPadding_;
          default:
            DEBUG_ASSERT(false);
            return 0;
        }
      }();

      drawObjects(
          *child,
          curObjectDetails.childStartIndex + i,
          resolvedDetails,
          camera,
          math::Vec<uint16_t, 2>{
              static_cast<uint16_t>(
                  offset.x() + child->outerPadding_ + crossLayoutOffset),
              static_cast<uint16_t>(offset.y() + child->outerPadding_)},
          baseZ);
      offset.y() +=
          childObjectDetails.resolvedHeight + 2 * child->outerPadding_;
    }
  }
}

} // namespace

void layoutAndDraw(
    UIObject& object,
    render::Simple2DCamera& camera,
    math::Vec<uint16_t, 2> windowSize,
    int baseZ) {
  size_t numObjects = objectCount(object);

  std::vector<ResolvedLayoutDetails> resolvedDetails;
  resolvedDetails.resize(numObjects);

  {
    size_t nextFree = 1;
    buildChildStartIndices(object, 0, nextFree, resolvedDetails);
    DEBUG_ASSERT(nextFree == numObjects);
  }

  resolveMinimums(object, 0, resolvedDetails);

  resolvedDetails[0].resolvedHeight = std::min(
      std::max(object.maxHeight_, resolvedDetails[0].resolvedMinHeight),
      windowSize.y());
  resolvedDetails[0].resolvedWidth = std::min(
      std::max(object.maxWidth_, resolvedDetails[0].resolvedMinWidth),
      windowSize.x());

  distributeExcess(object, 0, resolvedDetails);

  drawObjects(
      object,
      0,
      resolvedDetails,
      camera,
      {static_cast<uint16_t>(0), static_cast<uint16_t>(0)},
      baseZ);
}

} // namespace blocks::ui
