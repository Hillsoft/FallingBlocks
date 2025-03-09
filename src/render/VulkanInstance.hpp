#pragma once

#include <GLFW/glfw3.h>

namespace blocks::render {

class VulkanInstance {
 public:
  VulkanInstance();
  ~VulkanInstance();

  VulkanInstance(const VulkanInstance& other) = delete;
  VulkanInstance(VulkanInstance&& other) = delete;

  VulkanInstance& operator=(const VulkanInstance& other) = delete;
  VulkanInstance& operator=(VulkanInstance&& other) = delete;

  VkInstance getRawInstance() { return instance_; }

 private:
  VkInstance instance_;
};

} // namespace blocks::render
