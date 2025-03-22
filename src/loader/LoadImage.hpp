#pragma once

#include <filesystem>
#include "loader/Image.hpp"

namespace blocks::loader {

Image loadImage(const std::filesystem::path& path);

}
