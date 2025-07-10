#include <gtest/gtest.h>

#include "loader/font/Font.hpp"

TEST(FontTest, LoadFont) {
  blocks::loader::loadFont(RESOURCE_DIR "/times.ttf");
}
