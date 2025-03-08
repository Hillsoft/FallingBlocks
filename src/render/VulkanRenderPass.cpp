#include "render/VulkanRenderPass.hpp"

#include <stdexcept>

namespace tetris::render {

VulkanRenderPass::VulkanRenderPass(
    VulkanGraphicsDevice& device, VkFormat imageFormat)
    : device_(&device), renderPass_(nullptr) {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = imageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  VkResult result = vkCreateRenderPass(
      device.getRawDevice(), &renderPassInfo, nullptr, &renderPass_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create render pass"};
  }
}

VulkanRenderPass::~VulkanRenderPass() {
  if (renderPass_ != nullptr) {
    vkDestroyRenderPass(device_->getRawDevice(), renderPass_, nullptr);
  }
}

VulkanRenderPass::VulkanRenderPass(VulkanRenderPass&& other) noexcept
    : device_(other.device_), renderPass_(other.renderPass_) {
  other.device_ = nullptr;
  other.renderPass_ = nullptr;
}

VulkanRenderPass& VulkanRenderPass::operator=(
    VulkanRenderPass&& other) noexcept {
  std::swap(device_, other.device_);
  std::swap(renderPass_, other.renderPass_);

  return *this;
}

} // namespace tetris::render
