#include "render/VulkanPipelineLayout.hpp"

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

VulkanPipelineLayout::VulkanPipelineLayout(VulkanGraphicsDevice& device)
    : layout_(nullptr, nullptr) {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  VkPipelineLayout layout;
  VkResult result = vkCreatePipelineLayout(
      device.getRawDevice(), &pipelineLayoutInfo, nullptr, &layout);

  layout_ =
      vulkan::UniqueHandle<VkPipelineLayout>{layout, device.getRawDevice()};
}

} // namespace blocks::render
