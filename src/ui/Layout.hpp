#pragma once

#include <cstdint>
#include "math/vec.hpp"
#include "render/Simple2DCamera.hpp"
#include "ui/UIObject.hpp"

namespace blocks::ui {

void layoutAndDraw(
    UIObject& object,
    render::Simple2DCamera& camera,
    math::Vec<uint16_t, 2> windowSize,
    int baseZ);

}
