#include "render/VulkanTexture.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "loader/Image.hpp"
#include "loader/LoadImage.hpp"
#include "render/VulkanBuffer.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanDeviceMemory.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/CommandBufferBuilder.hpp"
#include "render/vulkan/FenceBuilder.hpp"
#include "render/vulkan/ImageViewBuilder.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

namespace {

void transitionImageLayout(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkFormat /* format */,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    uint32_t mipLevels) {
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = mipLevels;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags sourceStage{};
  VkPipelineStageFlags destinationStage{};

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (
      oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
      newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    throw std::runtime_error{"Bad transition"};
  }

  vkCmdPipelineBarrier(
      commandBuffer,
      sourceStage,
      destinationStage,
      0,
      0,
      nullptr,
      0,
      nullptr,
      1,
      &barrier);
}

void generateMipMaps(
    VkCommandBuffer commandBuffer,
    VkImage image,
    uint32_t mipLevels,
    int32_t texWidth,
    int32_t texHeight) {
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image = image;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.layerCount = 1;

  int32_t mipWidth = texWidth;
  int32_t mipHeight = texHeight;

  for (uint32_t i = 1; i < mipLevels; i++) {
    barrier.subresourceRange.baseMipLevel = i - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier);

    VkImageBlit blit{};
    blit.srcOffsets[0] = {.x = 0, .y = 0, .z = 0};
    blit.srcOffsets[1] = {.x = mipWidth, .y = mipHeight, .z = 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = i - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = {.x = 0, .y = 0, .z = 0};
    blit.dstOffsets[1] = {
        .x = mipWidth > 1 ? mipWidth / 2 : 1,
        .y = mipHeight > 1 ? mipHeight / 2 : 1,
        .z = 1};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = i;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(
        commandBuffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &blit,
        VK_FILTER_LINEAR);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier);

    if (mipWidth > 1) {
      mipWidth /= 2;
    }
    if (mipHeight > 1) {
      mipHeight /= 2;
    }
  }

  barrier.subresourceRange.baseMipLevel = mipLevels - 1;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

  vkCmdPipelineBarrier(
      commandBuffer,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0,
      0,
      nullptr,
      0,
      nullptr,
      1,
      &barrier);
}

} // namespace

VulkanTexture::VulkanTexture(
    VulkanGraphicsDevice& device,
    VulkanCommandPool& commandPool,
    const std::filesystem::path& source)
    : deviceMemory_(nullptr, nullptr),
      buffer_(nullptr, nullptr),
      image_(nullptr, nullptr),
      imageView_(nullptr, nullptr),
      sampler_(nullptr, nullptr) {
  loader::Image tex = loader::loadImage(source);
  const auto mipLevels =
      static_cast<uint32_t>(
          std::floor(std::log2(std::max(tex.width, tex.height)))) +
      1;

  VulkanBuffer stagingBuffer(
      device, tex.pixelData, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

  VkImage textureImage = nullptr;

  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = static_cast<uint32_t>(tex.width);
  imageInfo.extent.height = static_cast<uint32_t>(tex.height);
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = mipLevels;
  imageInfo.arrayLayers = 1;
  imageInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  // NOLINTNEXTLINE(hicpp-signed-bitwise)
  imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
      VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.flags = 0;

  if (vkCreateImage(
          device.getRawDevice(), &imageInfo, nullptr, &textureImage) !=
      VK_SUCCESS) {
    throw std::runtime_error{"Failed to allocate image memory"};
  }
  image_ = vulkan::UniqueHandle<VkImage>{textureImage, device.getRawDevice()};

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(
      device.getRawDevice(), textureImage, &memRequirements);
  VkPhysicalDeviceMemoryProperties memProperties{};
  vkGetPhysicalDeviceMemoryProperties(
      device.physicalInfo().device, &memProperties);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(
      memRequirements, memProperties, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VkDeviceMemory deviceMemory = nullptr;
  if (vkAllocateMemory(
          device.getRawDevice(), &allocInfo, nullptr, &deviceMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error{"Failed to allocate image memory"};
  }
  deviceMemory_ =
      vulkan::UniqueHandle<VkDeviceMemory>(deviceMemory, device.getRawDevice());

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = tex.pixelData.size();
  bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkBuffer buffer = nullptr;
  const VkResult result =
      vkCreateBuffer(device.getRawDevice(), &bufferInfo, nullptr, &buffer);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create buffer"};
  }

  buffer_ = vulkan::UniqueHandle<VkBuffer>{buffer, device.getRawDevice()};
  vkBindBufferMemory(device.getRawDevice(), buffer, deviceMemory, 0);

  vkBindImageMemory(
      device.getRawDevice(), textureImage, deviceMemory_.get(), 0);

  const vulkan::UniqueHandle<VkCommandBuffer> commandBuffer =
      vulkan::CommandBufferBuilder(
          commandPool.getRawCommandPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY)
          .build(device.getRawDevice());

  vulkan::beginSingleTimeCommandBuffer(commandBuffer.get());

  transitionImageLayout(
      commandBuffer.get(),
      textureImage,
      VK_FORMAT_B8G8R8A8_SRGB,
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      mipLevels);

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {.x = 0, .y = 0, .z = 0};
  region.imageExtent = {
      .width = static_cast<uint32_t>(tex.width),
      .height = static_cast<uint32_t>(tex.height),
      .depth = 1};
  vkCmdCopyBufferToImage(
      commandBuffer.get(),
      stagingBuffer.getRawBuffer(),
      textureImage,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      1,
      &region);

  generateMipMaps(
      commandBuffer.get(),
      image_.get(),
      mipLevels,
      static_cast<int32_t>(tex.width),
      static_cast<int32_t>(tex.height));

  const vulkan::UniqueHandle<VkFence> fence =
      vulkan::FenceBuilder().build(device.getRawDevice());
  vulkan::endSingleTimeCommandBuffer(
      commandBuffer.get(), commandPool.getQueue(), {}, {}, fence.get());
  vkWaitForFences(device.getRawDevice(), 1, &fence.get(), VK_TRUE, UINT32_MAX);
  imageView_ =
      vulkan::ImageViewBuilder(textureImage, VK_FORMAT_B8G8R8A8_SRGB)
          .setMipLevels(mipLevels)
          .build(device.getRawDevice());

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
  samplerInfo.anisotropyEnable = VK_FALSE;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = VK_LOD_CLAMP_NONE;

  VkSampler sampler = nullptr;
  if (vkCreateSampler(device.getRawDevice(), &samplerInfo, nullptr, &sampler) !=
      VK_SUCCESS) {
    throw std::runtime_error{"Failed to create sampler"};
  }
  sampler_ = vulkan::UniqueHandle<VkSampler>{sampler, device.getRawDevice()};
}

} // namespace blocks::render
