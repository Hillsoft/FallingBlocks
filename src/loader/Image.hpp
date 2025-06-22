#pragma once

#include <cstddef>
#include <vector>

namespace blocks::loader {

class Image {
 public:
  size_t width;
  size_t height;
  std::vector<std::byte> pixelData;
};

} // namespace blocks::loader
