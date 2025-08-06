#pragma once

#include <cstdint>
#include <string_view>
#include "util/Generator.hpp"

namespace util {

Generator<uint32_t> unicodeDecode(std::string_view str);

}
