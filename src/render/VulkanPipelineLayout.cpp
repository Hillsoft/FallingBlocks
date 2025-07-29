#include "render/VulkanPipelineLayout.hpp"

#include <stdexcept>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

VulkanPipelineLayout::VulkanPipelineLayout(
    VulkanGraphicsDevice& device, VkDescriptorSetLayout descriptorLayout)
    : layout_(nullptr, nullptr) {
  VkDescriptorSetLayout rawDescriptorLayout = descriptorLayout;
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &rawDescriptorLayout;
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  VkPipelineLayout layout;
  VkResult result = vkCreatePipelineLayout(
      device.getRawDevice(), &pipelineLayoutInfo, nullptr, &layout);

  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create pipeline layout"};
  }

  layout_ =
      vulkan::UniqueHandle<VkPipelineLayout>{layout, device.getRawDevice()};
}

} // namespace blocks::render
