#include "util/file.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <stdexcept>
#include <vector>

namespace util {

std::vector<std::byte> readFileBytes(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error{"Failed to open file"};
  }

  size_t fileSize = file.tellg();
  std::vector<std::byte> buffer;
  buffer.resize(fileSize);

  file.seekg(0);
  file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

  return buffer;
}

std::vector<char> readFileChars(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error{"Failed to open file"};
  }

  size_t fileSize = file.tellg();
  std::vector<char> buffer;
  buffer.resize(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  return buffer;
}

} // namespace util
