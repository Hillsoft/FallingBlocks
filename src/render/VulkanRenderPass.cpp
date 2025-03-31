#include "render/VulkanRenderPass.hpp"

#include <cstdint>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/RenderPassBuilder.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

namespace {

vulkan::UniqueHandle<VkRenderPass> makeRenderPass(
    VkDevice device, VkFormat imageFormat) {
  vulkan::RenderPassBuilder builder{};

  VkAttachmentReference colorAttachmentRef =
      builder.addAttachmentGetDescription(
          {.format = imageFormat,
           .samples = VK_SAMPLE_COUNT_1_BIT,
           .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
           .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
           .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
           .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
           .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
           .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR},
          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  uint32_t mainSubpassIndex = builder.addSubpassGetIndex(
      VK_PIPELINE_BIND_POINT_GRAPHICS, {}, {&colorAttachmentRef, 1});

  builder.addSubpassDependency(
      {.srcSubpass = VK_SUBPASS_EXTERNAL,
       .dstSubpass = mainSubpassIndex,
       .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
       .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
       .srcAccessMask = 0,
       .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
       .dependencyFlags = 0});

  return builder.build(device);
}

} // namespace

VulkanRenderPass::VulkanRenderPass(
    VulkanGraphicsDevice& device, VkFormat imageFormat)
    : renderPass_(makeRenderPass(device.getRawDevice(), imageFormat)) {}

} // namespace blocks::render
