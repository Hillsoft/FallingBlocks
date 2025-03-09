#include "render/VulkanShader.hpp"

#include "util/file.hpp"

namespace blocks::render {

VulkanShader::VulkanShader(
    VulkanGraphicsDevice& device, const std::filesystem::path& shaderPath)
    : device_(&device), shaderModule_(nullptr) {
  std::vector<char> shaderData = util::readFileBytes(shaderPath);

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = shaderData.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderData.data());

  VkResult result = vkCreateShaderModule(
      device.getRawDevice(), &createInfo, nullptr, &shaderModule_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create shader"};
  }
}

VulkanShader::~VulkanShader() {
  if (shaderModule_ != nullptr) {
    vkDestroyShaderModule(device_->getRawDevice(), shaderModule_, nullptr);
  }
}

VulkanShader::VulkanShader(VulkanShader&& other) noexcept
    : device_(other.device_), shaderModule_(other.shaderModule_) {
  other.device_ = nullptr;
  other.shaderModule_ = nullptr;
}

VulkanShader& VulkanShader::operator=(VulkanShader&& other) noexcept {
  std::swap(device_, other.device_);
  std::swap(shaderModule_, other.shaderModule_);

  return *this;
}

} // namespace blocks::render
