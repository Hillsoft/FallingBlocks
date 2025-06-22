#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace util {

std::vector<std::byte> zlibDecompress(std::span<const std::byte> data);

}
