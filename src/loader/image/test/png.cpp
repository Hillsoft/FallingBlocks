#include <gtest/gtest.h>

#include <filesystem>
#include "loader/Image.hpp"
#include "loader/image/Png.hpp"

TEST(PngTest, HeaderLoad) {
  const blocks::loader::Image result = blocks::loader::loadPng(
      std::filesystem::path(RESOURCE_DIR "/mandelbrot set.png"));
}
