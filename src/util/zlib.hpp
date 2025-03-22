#pragma once

#include <span>
#include <vector>

namespace util {

std::vector<unsigned char> zlibDecompress(std::span<const unsigned char> data);

}
