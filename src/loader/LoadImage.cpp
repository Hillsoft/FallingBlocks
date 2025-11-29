#include "loader/LoadImage.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>
#include "loader/Image.hpp"
#include "loader/image/Bitmap.hpp"
#include "loader/image/Png.hpp"
#include "util/string.hpp"

namespace blocks::loader {

Image loadImage(const std::filesystem::path& path) {
  const std::filesystem::path extension = path.extension();

  if (extension == ".bmp") {
    return loadBitmap(path);
  }
  if (extension == ".png") {
    return loadPng(path);
  }

  throw std::runtime_error{
      util::toString("Unknown image format: ", extension.string().c_str())};
}

} // namespace blocks::loader
