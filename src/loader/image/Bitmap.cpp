#include "loader/image/Bitmap.hpp"

#include <bit>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include "loader/Image.hpp"
#include "util/file.hpp"

static_assert(std::endian::native == std::endian::little);

namespace blocks::loader {

namespace {

#pragma pack(push, 1)
struct BitmapHeader {
  char header[2];
  uint32_t fileSize;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t pixelArrayOffset;
};
#pragma pack(pop)
static_assert(sizeof(BitmapHeader) == 14);

#pragma pack(push, 1)
struct BitmapInfoHeader {
  enum CompressionInfo : uint32_t { RGB = 0, RLE8 = 1, RLE4 = 2 };

  uint32_t headerSize;
  uint32_t imageWidth;
  uint32_t imageHeight;
  uint16_t colorPlanes;
  uint16_t bitsPerPixel;
  CompressionInfo compressionInfo;
  uint32_t rawBitmapSize;
  uint32_t pixelPerMetreHorizontal;
  uint32_t pixelPerMetreVertical;
  uint32_t paletteSize;
  uint32_t importantColors;
};
#pragma pack(pop)
static_assert(sizeof(BitmapInfoHeader) == 40);

} // namespace

Image loadBitmap(const std::filesystem::path& path) {
  return loadBitmap(util::readFileBytes(path));
}

Image loadBitmap(const std::vector<char>& data) {
  if (data.size() < sizeof(BitmapHeader)) {
    throw std::runtime_error{"Corrupt bitmap"};
  }

  const BitmapHeader* header =
      reinterpret_cast<const BitmapHeader*>(data.data() + 0);
  if (header->header[0] != 'B' || header->header[1] != 'M') {
    throw std::runtime_error{"Corrupt bitmap"};
  }

  if (header->fileSize != data.size()) {
    throw std::runtime_error{"Corrupt bitmap"};
  }

  if (data.size() - sizeof(BitmapHeader) < sizeof(BitmapInfoHeader)) {
    throw std::runtime_error{"Corrupt bitmap"};
  }

  uint32_t infoHeaderSize =
      *reinterpret_cast<const uint32_t*>(data.data() + sizeof(BitmapHeader));
  if (infoHeaderSize != 40) {
    throw std::runtime_error{"Unsupported bitmap format"};
  }

  const BitmapInfoHeader* infoHeader =
      reinterpret_cast<const BitmapInfoHeader*>(
          data.data() + sizeof(BitmapHeader));

  uint32_t imageWidth = infoHeader->imageWidth;
  uint32_t imageHeight = infoHeader->imageHeight;

  if (infoHeader->colorPlanes != 1 || infoHeader->bitsPerPixel != 24 ||
      infoHeader->compressionInfo != BitmapInfoHeader::CompressionInfo::RGB ||
      infoHeader->paletteSize != 0 || infoHeader->importantColors != 0) {
    throw std::runtime_error{"Unsupported bitmap format"};
  }

  uint32_t rowSize = ((imageWidth * 3 + 3) / 4) * 4;
  uint32_t bitmapSize = rowSize * imageHeight;

  if (infoHeader->rawBitmapSize != 0 &&
      infoHeader->rawBitmapSize != bitmapSize) {
    throw std::runtime_error{"Corrupt bitmap"};
  }
  if (data.size() - sizeof(BitmapHeader) - sizeof(BitmapInfoHeader) <
      bitmapSize) {
    throw std::runtime_error{"Corrupt bitmap"};
  }

  if (header->pixelArrayOffset !=
      sizeof(BitmapHeader) + sizeof(BitmapInfoHeader)) {
    throw std::runtime_error{"Corrupt bitmap"};
  }
  const char* bitmapData =
      data.data() + sizeof(BitmapHeader) + sizeof(BitmapInfoHeader);

  std::vector<char> outData;
  outData.reserve(imageWidth * imageHeight * 4);

  for (uint32_t y = 0; y < imageHeight; y++) {
    for (uint32_t x = 0; x < imageWidth; x++) {
      const char* pixel = bitmapData + y * rowSize + x * 3;
      char r = pixel[0];
      char g = pixel[1];
      char b = pixel[2];

      outData.emplace_back(r);
      outData.emplace_back(g);
      outData.emplace_back(b);
      outData.emplace_back(std::numeric_limits<char>::max());
    }
  }

  return Image{
      .width = imageWidth,
      .height = imageHeight,
      .pixelData = std::move(outData)};
}

} // namespace blocks::loader
