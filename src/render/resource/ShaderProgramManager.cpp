#include "render/resource/ShaderProgramManager.hpp"

#include <typeindex>
#include <utility>
#include "render/VulkanShaderProgram.hpp"
#include "util/debug.hpp"

namespace blocks::render {

VulkanShaderProgram* ShaderProgramManager::get(std::type_index index) {
  auto it = programs_.find(index);
  if (it != programs_.end()) {
    return &it->second;
  }
  return nullptr;
}

VulkanShaderProgram& ShaderProgramManager::insert(
    std::type_index index, VulkanShaderProgram&& program) {
  DEBUG_ASSERT(programs_.find(index) == programs_.end());
  auto inserted = programs_.emplace(index, std::move(program));
  return inserted.first->second;
}

} // namespace blocks::render
