#include "util/unicode.hpp"

#include <bit>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string_view>
#include "util/Generator.hpp"

namespace util {

namespace {

constexpr unsigned char kContinuationByteMask = 0b00111111;
constexpr unsigned char kTwoByteHeaderMask = 0b00011111;
constexpr unsigned char kThreeByteHeaderMask = 0b00001111;
constexpr unsigned char kFourByteHeaderMask = 0b00000111;

} // namespace

Generator<uint32_t> unicodeDecode(std::string_view str) {
  std::span<const unsigned char> strBytes{
      reinterpret_cast<const unsigned char*>(str.data()), str.size()};

  while (strBytes.size() > 0) {
    auto leadingOnes = std::countl_one(strBytes[0]);
    uint32_t curByte;
    switch (leadingOnes) {
      case 0:
        curByte = strBytes[0];
        strBytes = strBytes.subspan(1);
        break;

      case 2:
        if (strBytes.size() < 2 || (strBytes[1] >> 6) != 0b10) {
          throw std::runtime_error{"Invalid UTF-8 sequence"};
        }
        curByte =
            (static_cast<uint32_t>(strBytes[0] & kTwoByteHeaderMask) << 6) |
            static_cast<uint32_t>(strBytes[1] & kContinuationByteMask);
        strBytes = strBytes.subspan(2);
        break;

      case 3:
        if (strBytes.size() < 3 || (strBytes[1] >> 6) != 0b10 ||
            (strBytes[2] >> 6) != 0b10) {
          throw std::runtime_error{"Invalid UTF-8 sequence"};
        }
        curByte =
            (static_cast<uint32_t>(strBytes[0] & kThreeByteHeaderMask) << 12) |
            (static_cast<uint32_t>(strBytes[1] & kContinuationByteMask) << 6) |
            static_cast<uint32_t>(strBytes[2] & kContinuationByteMask);
        strBytes = strBytes.subspan(3);
        break;

      case 4:
        if (strBytes.size() < 4 || (strBytes[1] >> 6) != 0b10 ||
            (strBytes[2] >> 6) != 0b10 || (strBytes[3] >> 6) != 0b10) {
          throw std::runtime_error{"Invalid UTF-8 sequence"};
        }
        curByte =
            (static_cast<uint32_t>(strBytes[0] & kFourByteHeaderMask) << 18) |
            (static_cast<uint32_t>(strBytes[1] & kContinuationByteMask) << 12) |
            (static_cast<uint32_t>(strBytes[2] & kContinuationByteMask) << 6) |
            static_cast<uint32_t>(strBytes[3] & kContinuationByteMask);
        strBytes = strBytes.subspan(4);
        break;

      default:
        throw std::runtime_error{"Invalid UTF-8 sequence"};
    }

    co_yield curByte;
  }
  co_return;
}

} // namespace util
