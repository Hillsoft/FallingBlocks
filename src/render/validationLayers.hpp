#pragma once

#include <array>

namespace blocks::render {

constexpr std::array<const char*, 1> kValidationLayers{
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
constexpr bool kEnableValidationLayers = false;
#else
constexpr bool kEnableValidationLayers = true;
#endif

} // namespace blocks::render
