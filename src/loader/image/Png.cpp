#include "loader/image/Png.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <limits>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "loader/Image.hpp"
#include "math/vec.hpp"
#include "util/debug.hpp"
#include "util/file.hpp"
#include "util/zlib.hpp"

static_assert(std::endian::native == std::endian::little);

namespace blocks::loader {

namespace {

constexpr unsigned short sZero = static_cast<unsigned short>(0);

using crc_type = uint32_t;

/* Table of CRCs of all 8-bit messages. */
constexpr std::array<crc_type, 256> crc_table = []() {
  std::array<crc_type, 256> crc_table{};

  for (int n = 0; n < 256; n++) {
    crc_type c = static_cast<crc_type>(n);
    for (int k = 0; k < 8; k++) {
      if (c & 1) {
        c = 0xedb88320L ^ (c >> 1);
      } else {
        c = c >> 1;
      }
    }
    crc_table[n] = c;
  }

  return crc_table;
}();

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
   should be initialized to all 1's, and the transmitted value
   is the 1's complement of the final running CRC (see the
   crc() routine below)). */
crc_type update_crc(crc_type c, const std::byte* buf, size_t len) {
  for (size_t n = 0; n < len; n++) {
    c = crc_table[(c ^ static_cast<crc_type>(buf[n])) & 0xff] ^ (c >> 8);
  }
  return c;
}

/* Return the CRC of the bytes buf[0..len-1]. */
crc_type crc(std::span<const std::byte> data) {
  return update_crc(0xffffffffL, data.data(), data.size()) ^ 0xffffffffL;
}

constexpr std::array<std::byte, 8> kPngHeader{
    std::byte{137},
    std::byte{80},
    std::byte{78},
    std::byte{71},
    std::byte{13},
    std::byte{10},
    std::byte{26},
    std::byte{10}};

bool checkSignature(std::span<const std::byte>& data) {
  if (data.size() < kPngHeader.size()) {
    return false;
  }
  for (size_t i = 0; i < kPngHeader.size(); i++) {
    if (data[i] != kPngHeader[i]) {
      return false;
    }
  }
  data = data.subspan(8);
  return true;
}

uint32_t readBigEndian(std::span<const std::byte> data) {
  DEBUG_ASSERT(data.size() >= 4);
  return (static_cast<uint32_t>(data[0]) << 24) |
      (static_cast<uint32_t>(data[1]) << 16) |
      (static_cast<uint32_t>(data[2]) << 8) | static_cast<uint32_t>(data[3]);
}

struct Chunk {
  std::array<std::byte, 4> type;
  std::span<const std::byte> data;
};

Chunk readChunk(std::span<const std::byte>& data) {
  Chunk outChunk{};

  if (data.size() < 8) {
    throw std::runtime_error{"Corrupt PNG"};
  }

  uint32_t chunkLength = readBigEndian(data);
  if (chunkLength > 1 << 31) {
    throw std::runtime_error{"Corrupt PNG"};
  }

  data = data.subspan(4);

  for (int i = 0; i < 4; i++) {
    outChunk.type[i] = data[i];
  }

  if (data.size() < chunkLength + 8) {
    throw std::runtime_error{"Corrupt PNG"};
  }

  uint32_t actualCrc = crc(data.subspan(0, chunkLength + 4));

  data = data.subspan(4);

  outChunk.data = data.subspan(0, chunkLength);

  uint32_t chunkCrc = readBigEndian(data.subspan(chunkLength));
  if (chunkCrc != actualCrc) {
    throw std::runtime_error{"Corrupt PNG"};
  }
  data = data.subspan(chunkLength + 4);

  return outChunk;
}

struct PngHeader {
  uint32_t width;
  uint32_t height;
  uint8_t bitDepth;
  uint8_t colorType;
  uint8_t compression;
  uint8_t filter;
  uint8_t interlace;
};

PngHeader readPngHeader(std::span<const std::byte> data) {
  DEBUG_ASSERT(data.size() == 13);

  PngHeader header{};
  header.width = readBigEndian(data.subspan(0));
  header.height = readBigEndian(data.subspan(4));
  header.bitDepth = static_cast<uint8_t>(data[8]);
  header.colorType = static_cast<uint8_t>(data[9]);
  header.compression = static_cast<uint8_t>(data[10]);
  header.filter = static_cast<uint8_t>(data[11]);
  header.interlace = static_cast<uint8_t>(data[12]);

  return header;
}

enum FilterMode {
  NONE = 0,
  SUB = 1,
  UP = 2,
  AVERAGE = 3,
  PAETH = 4,
};

template <size_t pixelSizeBytes>
std::vector<std::byte> defilter(
    const PngHeader& header, std::span<const std::byte> decompressedData)
  requires(pixelSizeBytes == 3 || pixelSizeBytes == 4)
{
  using Color = math::Vec<unsigned short, pixelSizeBytes>;

  size_t outRowSize = 4 * header.width;
  size_t rowSize = pixelSizeBytes * header.width + 1;
  std::vector<std::byte> outData;
  outData.reserve(header.width * header.height * 4);

  auto readDecompressed = [&](size_t x, size_t y) {
    size_t pixelOffset = 1 + pixelSizeBytes * x + rowSize * y;
    Color out;
    for (int i = 0; i < pixelSizeBytes; i++) {
      out.at(i) =
          static_cast<unsigned short>(decompressedData[pixelOffset + i]);
    }
    return out;
  };
  auto readOut = [&](size_t x, size_t y) {
    size_t pixelOffset = 4 * x + y * outRowSize;
    Color out;
    for (int i = 0; i < pixelSizeBytes; i++) {
      out.at(i) = static_cast<unsigned short>(outData[pixelOffset + i]);
    }
    return out;
  };

  auto writeOut = [&](Color col) {
    outData.emplace_back(static_cast<std::byte>(col.at(0)));
    outData.emplace_back(static_cast<std::byte>(col.at(1)));
    outData.emplace_back(static_cast<std::byte>(col.at(2)));
    if constexpr (pixelSizeBytes == 4) {
      outData.emplace_back(static_cast<std::byte>(col.at(3)));
    } else {
      outData.emplace_back(
          static_cast<std::byte>(std::numeric_limits<unsigned char>::max()));
    }
  };

  for (size_t y = 0; y < header.height; y++) {
    unsigned char rawFilterMode =
        static_cast<unsigned char>(decompressedData[rowSize * y]);
    if (rawFilterMode > 4) {
      throw std::runtime_error{"Corrupt PNG"};
    }
    FilterMode filterMode = static_cast<FilterMode>(rawFilterMode);

    for (size_t x = 0; x < header.width; x++) {
      if (filterMode == FilterMode::NONE) {
        Color col = readDecompressed(x, y);
        writeOut(col);
      } else if (filterMode == FilterMode::SUB) {
        Color prev = x != 0 ? readOut(x - 1, y) : Color{sZero, sZero, sZero};
        Color raw = readDecompressed(x, y);
        writeOut(raw + prev);
      } else if (filterMode == FilterMode::UP) {
        Color prev = y != 0 ? readOut(x, y - 1) : Color{sZero, sZero, sZero};
        Color raw = readDecompressed(x, y);
        writeOut(raw + prev);
      } else if (filterMode == FilterMode::AVERAGE) {
        Color prevX = x != 0 ? readOut(x - 1, y) : Color{sZero, sZero, sZero};
        Color prevY = y != 0 ? readOut(x, y - 1) : Color{sZero, sZero, sZero};
        Color raw = readDecompressed(x, y);
        writeOut(raw + (prevX + prevY) / static_cast<unsigned short>(2));
      } else if (filterMode == FilterMode::PAETH) {
        Color prevX = x != 0 ? readOut(x - 1, y) : Color{sZero, sZero, sZero};
        Color prevY = y != 0 ? readOut(x, y - 1) : Color{sZero, sZero, sZero};
        Color prevXY = x != 0 && y != 0
            ? readOut(x - 1, y - 1)
            : Color{sZero, sZero, sZero};
        Color raw = readDecompressed(x, y);
        Color predictor;

        for (size_t i = 0; i < pixelSizeBytes; i++) {
          int a = prevX.at(i);
          int b = prevY.at(i);
          int c = prevXY.at(i);

          int thresh = c * 3 - (a + b);
          int lo = a < b ? a : b;
          int hi = a < b ? b : a;
          int t0 = (hi <= thresh) ? lo : c;
          int t1 = (thresh <= lo) ? hi : t0;
          int presult = t1;

          predictor.at(i) = static_cast<unsigned short>(presult);
        }

        writeOut(raw + predictor);
      } else {
        DEBUG_ASSERT(false);
      }
    }
  }

  return outData;
}

} // namespace

Image loadPng(const std::filesystem::path& path) {
  return loadPng(util::readFileBytes(path));
}

Image loadPng(std::span<const std::byte> data) {
  if (!checkSignature(data)) {
    throw std::runtime_error{"Corrupt PNG"};
  }

  Chunk headerChunk = readChunk(data);
  PngHeader header = readPngHeader(headerChunk.data);

  if (header.bitDepth != 8 ||
      (header.colorType != 2 && header.colorType != 6) ||
      header.compression != 0) {
    throw std::runtime_error{"Unsupported PNG format"};
  }

  bool hasAlpha = header.colorType == 6;

  std::vector<std::byte> imgData;

  while (data.size() > 0) {
    Chunk chunk = readChunk(data);
    if (chunk.type ==
        std::array<std::byte, 4>{
            std::byte{'I'}, std::byte{'D'}, std::byte{'A'}, std::byte{'T'}}) {
      for (auto c : chunk.data) {
        imgData.push_back(c);
      }
    }
  }

  std::vector<std::byte> decompressedData =
      util::zlibDecompress(std::span(imgData.begin(), imgData.end()));

  size_t pixelSizeBytes = hasAlpha ? 4 : 3;

  size_t rowSize = pixelSizeBytes * header.width + 1;
  if (decompressedData.size() != rowSize * header.height) {
    throw std::runtime_error{"Corrupt PNG"};
  }

  size_t outRowSize = 4 * header.width;
  std::vector<std::byte> outData = hasAlpha
      ? defilter<4>(header, decompressedData)
      : defilter<3>(header, decompressedData);

  // Adjust to the expected BGR format
  for (size_t y = 0; y < header.height; y++) {
    for (size_t x = 0; x < header.width; x++) {
      size_t pixelOffset = 4 * x + y * outRowSize;
      std::swap(outData[pixelOffset], outData[pixelOffset + 2]);
    }
  }

  return Image{
      .width = header.width,
      .height = header.height,
      .pixelData = std::move(outData)};
}

} // namespace blocks::loader
