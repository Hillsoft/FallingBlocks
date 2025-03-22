#include "loader/image/Png.hpp"

#include <array>
#include <bit>
#include <cstdint>
#include <filesystem>
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

using Color = math::Vec<unsigned short, 3>;

namespace {

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
crc_type update_crc(crc_type c, const unsigned char* buf, size_t len) {
  for (size_t n = 0; n < len; n++) {
    c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
  }
  return c;
}

/* Return the CRC of the bytes buf[0..len-1]. */
crc_type crc(std::span<const unsigned char> data) {
  return update_crc(0xffffffffL, data.data(), data.size()) ^ 0xffffffffL;
}

constexpr std::array<unsigned char, 8> kPngHeader{
    137, 80, 78, 71, 13, 10, 26, 10};

bool checkSignature(std::span<const unsigned char>& data) {
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

uint32_t readBigEndian(std::span<const unsigned char> data) {
  DEBUG_ASSERT(data.size() >= 4);
  return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}

struct Chunk {
  std::array<unsigned char, 4> type;
  std::span<const unsigned char> data;
};

Chunk readChunk(std::span<const unsigned char>& data) {
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

PngHeader readPngHeader(std::span<const unsigned char> data) {
  DEBUG_ASSERT(data.size() == 13);

  PngHeader header{};
  header.width = readBigEndian(data.subspan(0));
  header.height = readBigEndian(data.subspan(4));
  header.bitDepth = data[8];
  header.colorType = data[9];
  header.compression = data[10];
  header.filter = data[11];
  header.interlace = data[12];

  return header;
}

enum FilterMode {
  NONE = 0,
  SUB = 1,
  UP = 2,
  AVERAGE = 3,
  PAETH = 4,
};

} // namespace

Image loadPng(const std::filesystem::path& path) {
  std::vector<char> data = util::readFileBytes(path);
  return loadPng(std::span<unsigned char>(
      reinterpret_cast<unsigned char*>(data.data()), data.size()));
}

Image loadPng(std::span<const unsigned char> data) {
  if (!checkSignature(data)) {
    throw std::runtime_error{"Corrupt PNG"};
  }

  Chunk headerChunk = readChunk(data);
  PngHeader header = readPngHeader(headerChunk.data);

  if (header.bitDepth != 8 || header.colorType != 2 ||
      header.compression != 0) {
    throw std::runtime_error{"Unsupported PNG format"};
  }

  std::vector<unsigned char> imgData;

  while (data.size() > 0) {
    Chunk chunk = readChunk(data);
    if (chunk.type == std::array<unsigned char, 4>{'I', 'D', 'A', 'T'}) {
      for (auto c : chunk.data) {
        imgData.push_back(c);
      }
    }
  }

  std::vector<unsigned char> decompressedData =
      util::zlibDecompress(std::span(imgData.begin(), imgData.end()));

  size_t rowSize = 3 * header.width + 1;
  if (decompressedData.size() != rowSize * header.height) {
    throw std::runtime_error{"Corrupt PNG"};
  }

  size_t outRowSize = 4 * header.width;
  std::vector<char> outData;
  outData.reserve(header.width * header.height * 4);

  auto readDecompressed = [&](size_t x, size_t y) {
    size_t pixelOffset = 1 + 3 * x + rowSize * y;
    math::Vec<unsigned short, 3> out{
        decompressedData[pixelOffset],
        decompressedData[pixelOffset + 1],
        decompressedData[pixelOffset + 2]};
    return out;
  };
  auto readOut = [&](size_t x, size_t y) {
    size_t pixelOffset = 4 * x + y * outRowSize;
    math::Vec<unsigned short, 3> out{
        static_cast<unsigned char>(outData[pixelOffset]),
        static_cast<unsigned char>(outData[pixelOffset + 1]),
        static_cast<unsigned char>(outData[pixelOffset + 2])};
    return out;
  };

  auto writeOut = [&](Color col) {
    outData.emplace_back(static_cast<unsigned char>(col.at(0)));
    outData.emplace_back(static_cast<unsigned char>(col.at(1)));
    outData.emplace_back(static_cast<unsigned char>(col.at(2)));
    outData.emplace_back(std::numeric_limits<char>::max());
  };

  for (size_t y = 0; y < header.height; y++) {
    unsigned char rawFilterMode = decompressedData[rowSize * y];
    if (rawFilterMode > 4) {
      throw std::runtime_error{"Corrupt PNG"};
    }
    FilterMode filterMode = static_cast<FilterMode>(rawFilterMode);

    for (size_t x = 0; x < header.width; x++) {
      size_t pixelOffset = 1 + 3 * x + rowSize * y;
      if (filterMode == FilterMode::NONE) {
        Color col = readDecompressed(x, y);
        writeOut(col);
      } else if (filterMode == FilterMode::SUB) {
        Color prev = x != 0 ? readOut(x - 1, y) : Color{0, 0, 0};
        Color raw = readDecompressed(x, y);
        writeOut(raw + prev);
      } else if (filterMode == FilterMode::UP) {
        Color prev = y != 0 ? readOut(x, y - 1) : Color{0, 0, 0};
        Color raw = readDecompressed(x, y);
        writeOut(raw + prev);
      } else if (filterMode == FilterMode::AVERAGE) {
        Color prevX = x != 0 ? readOut(x - 1, y) : Color{0, 0, 0};
        Color prevY = y != 0 ? readOut(x, y - 1) : Color{0, 0, 0};
        Color raw = readDecompressed(x, y);
        writeOut(raw + (prevX + prevY) / static_cast<unsigned short>(2));
      } else if (filterMode == FilterMode::PAETH) {
        Color prevX = x != 0 ? readOut(x - 1, y) : Color{0, 0, 0};
        Color prevY = y != 0 ? readOut(x, y - 1) : Color{0, 0, 0};
        Color prevXY =
            x != 0 && y != 0 ? readOut(x - 1, y - 1) : Color{0, 0, 0};
        Color raw = readDecompressed(x, y);
        Color predictor;

        for (size_t i = 0; i < 3; i++) {
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
