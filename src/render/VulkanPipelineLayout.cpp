#include "render/VulkanPipelineLayout.hpp"

namespace tetris::render {

VulkanPipelineLayout::VulkanPipelineLayout(VulkanGraphicsDevice& device)
    : device_(&device), layout_(nullptr) {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  VkResult result = vkCreatePipelineLayout(
      device.getRawDevice(), &pipelineLayoutInfo, nullptr, &layout_);
}

VulkanPipelineLayout::~VulkanPipelineLayout() {
  if (layout_ != nullptr) {
    vkDestroyPipelineLayout(device_->getRawDevice(), layout_, nullptr);
  }
}

VulkanPipelineLayout::VulkanPipelineLayout(
    VulkanPipelineLayout&& other) noexcept
    : device_(other.device_), layout_(other.layout_) {
  other.device_ = nullptr;
  other.layout_ = nullptr;
}

VulkanPipelineLayout& VulkanPipelineLayout::operator=(
    VulkanPipelineLayout&& other) noexcept {
  std::swap(device_, other.device_);
  std::swap(layout_, other.layout_);

  return *this;
}

} // namespace tetris::render
