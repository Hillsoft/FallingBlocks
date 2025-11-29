#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

vulkan::UniqueHandle<VkSurfaceKHR> createSurfaceForWindow(
    VkInstance instance, GLFWwindow* window);

}
