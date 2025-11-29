#include "ui/Layout.hpp"

#include <algorithm>
#include <cstddef>
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
  uint16_t resolvedMaxWidth = 0;
  uint16_t resolvedMaxHeight = 0;
  uint16_t resolvedWidth = 0;
  uint16_t resolvedHeight = 0;
  size_t childStartIndex = 0;

  [[nodiscard]] uint16_t& resolvedMinLayoutAxis(
      LayoutDirection layoutDirection) {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? resolvedMinWidth
        : resolvedMinHeight;
  }
  [[nodiscard]] const uint16_t& resolvedMinLayoutAxis(
      LayoutDirection layoutDirection) const {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? resolvedMinWidth
        : resolvedMinHeight;
  }
  [[nodiscard]] uint16_t& resolvedMinCrossLayoutAxis(
      LayoutDirection layoutDirection) {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? resolvedMinHeight
        : resolvedMinWidth;
  }
  [[nodiscard]] const uint16_t& resolvedMinCrossLayoutAxis(
      LayoutDirection layoutDirection) const {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? resolvedMinHeight
        : resolvedMinWidth;
  }
  [[nodiscard]] uint16_t& resolvedMaxLayoutAxis(
      LayoutDirection layoutDirection) {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? resolvedMaxWidth
        : resolvedMaxHeight;
  }
  [[nodiscard]] const uint16_t& resolvedMaxLayoutAxis(
      LayoutDirection layoutDirection) const {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? resolvedMaxWidth
        : resolvedMaxHeight;
  }
  [[nodiscard]] uint16_t& resolvedMaxCrossLayoutAxis(
      LayoutDirection layoutDirection) {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? resolvedMaxHeight
        : resolvedMaxWidth;
  }
  [[nodiscard]] const uint16_t& resolvedMaxCrossLayoutAxis(
      LayoutDirection layoutDirection) const {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? resolvedMaxHeight
        : resolvedMaxWidth;
  }
  [[nodiscard]] uint16_t& resolvedLayoutAxis(LayoutDirection layoutDirection) {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? resolvedWidth
        : resolvedHeight;
  }
  [[nodiscard]] const uint16_t& resolvedLayoutAxis(
      LayoutDirection layoutDirection) const {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? resolvedWidth
        : resolvedHeight;
  }
  [[nodiscard]] uint16_t& resolvedCrossLayoutAxis(
      LayoutDirection layoutDirection) {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? resolvedHeight
        : resolvedWidth;
  }
  [[nodiscard]] const uint16_t& resolvedCrossLayoutAxis(
      LayoutDirection layoutDirection) const {
    return layoutDirection == LayoutDirection::HORIZONTAL
        ? resolvedHeight
        : resolvedWidth;
  }
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
  const size_t curChildStartIndex = nextFree;
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
  const LayoutDirection layoutDirection = object.childLayoutDirection_;

  for (size_t i = 0; i < object.children_.size(); i++) {
    resolveMinimums(
        *object.children_[i],
        curObjectDetails.childStartIndex + i,
        resolvedDetails);
  }

  for (size_t i = 0; i < object.children_.size(); i++) {
    const auto& child = object.children_[i];
    const auto& childObjectDetails =
        resolvedDetails[curObjectDetails.childStartIndex + i];

    curObjectDetails.resolvedMinCrossLayoutAxis(layoutDirection) =
        std::max<uint16_t>(
            curObjectDetails.resolvedMinCrossLayoutAxis(layoutDirection),
            childObjectDetails.resolvedMinCrossLayoutAxis(layoutDirection) +
                (2 * child->outerPadding_));
    curObjectDetails.resolvedMinLayoutAxis(layoutDirection) +=
        childObjectDetails.resolvedMinLayoutAxis(layoutDirection) +
        2 * child->outerPadding_;
  }

  curObjectDetails.resolvedMinHeight =
      std::max<uint16_t>(
          object.minHeight_, curObjectDetails.resolvedMinHeight) +
      2 * object.innerPadding_;
  curObjectDetails.resolvedMinWidth =
      std::max<uint16_t>(object.minWidth_, curObjectDetails.resolvedMinWidth) +
      2 * object.innerPadding_;

  curObjectDetails.resolvedMaxHeight =
      std::max(object.maxHeight_, curObjectDetails.resolvedMinHeight);
  curObjectDetails.resolvedMaxWidth =
      std::max(object.maxWidth_, curObjectDetails.resolvedMinWidth);

  if (object.sizeStrategy_ == SizeStrategy::SHRINK_PARENT) {
    curObjectDetails.resolvedMaxHeight = curObjectDetails.resolvedMinHeight;
    curObjectDetails.resolvedMaxWidth = curObjectDetails.resolvedMinWidth;
  }
}

void distributeExcess(
    const UIObject& object,
    size_t objectIndex,
    std::span<ResolvedLayoutDetails> resolvedDetails) {
  if (object.children_.empty()) {
    return;
  }

  const ResolvedLayoutDetails& curObjectDetails = resolvedDetails[objectIndex];
  const LayoutDirection layoutDirection = object.childLayoutDirection_;

  for (size_t i = 0; i < object.children_.size(); i++) {
    auto& childObjectDetails =
        resolvedDetails[curObjectDetails.childStartIndex + i];
    childObjectDetails.resolvedHeight = childObjectDetails.resolvedMinHeight;
    childObjectDetails.resolvedWidth = childObjectDetails.resolvedMinWidth;
  }

  for (size_t i = 0; i < object.children_.size(); i++) {
    const auto& child = object.children_[i];
    auto& childObjectDetails =
        resolvedDetails[curObjectDetails.childStartIndex + i];

    childObjectDetails.resolvedCrossLayoutAxis(layoutDirection) =
        std::min<uint16_t>(
            curObjectDetails.resolvedCrossLayoutAxis(layoutDirection) -
                (2 * object.innerPadding_) - (2 * child->outerPadding_),
            childObjectDetails.resolvedMaxCrossLayoutAxis(layoutDirection));
  }

  if (object.sizeStrategy_ == SizeStrategy::GROW_CHILDREN) {
    const uint16_t availableSize =
        curObjectDetails.resolvedLayoutAxis(layoutDirection);
    uint16_t childSize =
        curObjectDetails.resolvedMinLayoutAxis(layoutDirection);
    size_t childrenWantingMore = object.children_.size();
    do {
      if (childrenWantingMore == 0 || availableSize <= childSize) {
        break;
      }
      const uint16_t excess = availableSize - childSize;
      const auto perChild = static_cast<uint16_t>(excess / childrenWantingMore);
      if (perChild == 0) {
        break;
      }

      // update the calculation of this for the next iteration
      childrenWantingMore = object.children_.size();

      for (size_t i = 0; i < object.children_.size(); i++) {
        auto& childObjectDetails =
            resolvedDetails[curObjectDetails.childStartIndex + i];

        const auto toGive = static_cast<uint16_t>(std::max(
            0,
            std::min<int>(
                perChild,
                static_cast<int>(
                    childObjectDetails.resolvedMaxLayoutAxis(layoutDirection)) -
                    static_cast<int>(childObjectDetails.resolvedLayoutAxis(
                        layoutDirection)))));
        if (toGive == 0) {
          childrenWantingMore--;
        } else {
          childObjectDetails.resolvedLayoutAxis(layoutDirection) += toGive;
          childSize += toGive;
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

uint16_t getChildSizeTotal(
    UIObject& object,
    size_t objectIndex,
    std::span<ResolvedLayoutDetails> resolvedDetails) {
  const ResolvedLayoutDetails& curObjectDetails = resolvedDetails[objectIndex];
  const LayoutDirection layoutDirection = object.childLayoutDirection_;

  uint16_t size = 0;
  for (size_t i = 0; i < object.children_.size(); i++) {
    const auto& child = object.children_[i];
    auto& childObjectDetails =
        resolvedDetails[curObjectDetails.childStartIndex + i];

    size += childObjectDetails.resolvedLayoutAxis(layoutDirection) +
        2 * child->outerPadding_;
  }

  return size;
}

uint16_t getAlignOffset(
    Align alignment, uint16_t objectSize, uint16_t availableSpace) {
  switch (alignment) {
    case Align::LEFT_TOP:
      return 0;
    case Align::MIDDLE:
      return (availableSpace - objectSize) / 2;
    case Align::RIGHT_BOTTOM:
      return availableSpace - objectSize;
    default:
      DEBUG_ASSERT(false);
      return 0;
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
  const LayoutDirection layoutDirection = object.childLayoutDirection_;

  baseZ += object.draw(
      offset,
      offset +
          math::Vec<uint16_t, 2>{
              curObjectDetails.resolvedWidth, curObjectDetails.resolvedHeight},
      camera,
      baseZ);

  const math::Vec<uint16_t, 2> layoutDirectionVec =
      layoutDirection == LayoutDirection::HORIZONTAL
      ? math::
            Vec<uint16_t, 2>{static_cast<uint16_t>(1), static_cast<uint16_t>(0)}
      : math::Vec<uint16_t, 2>{
            static_cast<uint16_t>(0), static_cast<uint16_t>(1)};
  const math::Vec<uint16_t, 2> crossLayoutDirectionVec =
      layoutDirection == LayoutDirection::HORIZONTAL
      ? math::
            Vec<uint16_t, 2>{static_cast<uint16_t>(0), static_cast<uint16_t>(1)}
      : math::Vec<uint16_t, 2>{
            static_cast<uint16_t>(1), static_cast<uint16_t>(0)};
  constexpr math::Vec<uint16_t, 2> bothDirections{
      static_cast<uint16_t>(1), static_cast<uint16_t>(1)};

  offset += object.innerPadding_ * bothDirections;

  offset +=
      getAlignOffset(
          object.alongLayoutChildPosition_,
          getChildSizeTotal(object, objectIndex, resolvedDetails),
          curObjectDetails.resolvedLayoutAxis(layoutDirection) -
              (2 * object.innerPadding_)) *
      layoutDirectionVec;

  for (size_t i = 0; i < object.children_.size(); i++) {
    const auto& child = object.children_[i];
    auto& childObjectDetails =
        resolvedDetails[curObjectDetails.childStartIndex + i];

    const uint16_t crossLayoutOffset = getAlignOffset(
        child->crossLayoutPosition_,
        childObjectDetails.resolvedCrossLayoutAxis(layoutDirection) -
            (2 * child->outerPadding_),
        curObjectDetails.resolvedCrossLayoutAxis(layoutDirection));

    drawObjects(
        *child,
        curObjectDetails.childStartIndex + i,
        resolvedDetails,
        camera,
        offset + child->outerPadding_ * bothDirections +
            crossLayoutOffset * crossLayoutDirectionVec,
        baseZ);
    offset +=
        static_cast<uint16_t>(
            childObjectDetails.resolvedLayoutAxis(layoutDirection) +
            (2 * child->outerPadding_)) *
        layoutDirectionVec;
  }
}

} // namespace

void layoutAndDraw(
    UIObject& object,
    render::Simple2DCamera& camera,
    math::Vec<uint16_t, 2> windowSize,
    int baseZ) {
  const size_t numObjects = objectCount(object);

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
      math::Vec<uint16_t, 2>{
          static_cast<uint16_t>(0), static_cast<uint16_t>(0)},
      baseZ);
}

} // namespace blocks::ui
