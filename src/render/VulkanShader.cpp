#include "render/VulkanShader.hpp"

#include "util/file.hpp"

namespace blocks::render {

VulkanShader::VulkanShader(
    VulkanGraphicsDevice& device, const std::filesystem::path& shaderPath)
    : shaderModule_(nullptr, nullptr) {
  std::vector<char> shaderData = util::readFileBytes(shaderPath);

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
      VulkanUniqueHandle<VkShaderModule>{shaderModule, device.getRawDevice()};
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
