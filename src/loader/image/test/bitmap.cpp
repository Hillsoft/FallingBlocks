#include <gtest/gtest.h>

#include "loader/Image.hpp"
#include "loader/image/Bitmap.hpp"

TEST(BitmapTest, HeaderLoad) {
  blocks::loader::Image result =
      blocks::loader::loadBitmap(RESOURCE_DIR "/test_image.bmp");
}
