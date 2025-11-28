#include "util/file.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <vector>

namespace util {

std::vector<std::byte> readFileBytes(const std::filesystem::path& path) {
  // NOLINTNEXTLINE(hicpp-signed-bitwise)
  std::ifstream file(path, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error{"Failed to open file"};
  }

  const std::streamsize fileSize = file.tellg();
  std::vector<std::byte> buffer;
  buffer.resize(fileSize);

  file.seekg(0);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

  return buffer;
}

std::vector<char> readFileChars(const std::filesystem::path& path) {
  // NOLINTNEXTLINE(hicpp-signed-bitwise)
  std::ifstream file(path, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error{"Failed to open file"};
  }

  const std::streamsize fileSize = file.tellg();
  std::vector<char> buffer;
  buffer.resize(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  return buffer;
}

} // namespace util
