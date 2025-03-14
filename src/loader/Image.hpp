#pragma once

#include <vector>

namespace blocks::loader {

class Image {
 public:
  size_t width;
  size_t height;
  std::vector<char> pixelData;
};

} // namespace blocks::loader
