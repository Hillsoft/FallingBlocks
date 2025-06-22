#include "render/VulkanShader.hpp"

#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <GLFW/glfw3.h>

#include <cstddef>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"
#include "util/file.hpp"

namespace blocks::render {

VulkanShader::VulkanShader(
    VulkanGraphicsDevice& device, const std::filesystem::path& shaderPath)
    : shaderModule_(nullptr, nullptr) {
  std::vector<std::byte> shaderData = util::readFileBytes(shaderPath);

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = shaderData.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderData.data());

  VkShaderModule shaderModule;
  VkResult result = vkCreateShaderModule(
      device.getRawDevice(), &createInfo, nullptr, &shaderModule);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create shader"};
  }

  shaderModule_ =
      vulkan::UniqueHandle<VkShaderModule>{shaderModule, device.getRawDevice()};
}

VulkanVertexShader::VulkanVertexShader(
    VulkanGraphicsDevice& device,
    std::vector<VkVertexInputBindingDescription> bindingDescriptions,
    std::vector<VkVertexInputAttributeDescription> inputAttributeDescription,
    const std::filesystem::path& shaderPath)
    : shader_(device, shaderPath),
      bindingDescriptions_(std::move(bindingDescriptions)),
      inputAttributeDescription_(std::move(inputAttributeDescription)) {}

} // namespace blocks::render
