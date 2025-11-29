#include "loader/image/Bitmap.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>
#include "loader/Image.hpp"
#include "util/file.hpp"

static_assert(std::endian::native == std::endian::little);

namespace blocks::loader {

namespace {

#pragma pack(push, 1)
struct BitmapHeader {
  // NOLINTNEXTLINE(*-avoid-c-arrays)
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
  // NOLINTNEXTLINE(performance-enum-size)
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

Image loadBitmap(const std::vector<std::byte>& data) {
  if (data.size() < sizeof(BitmapHeader)) {
    throw std::runtime_error{"Corrupt bitmap"};
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto* header = reinterpret_cast<const BitmapHeader*>(data.data());
  if (header->header[0] != 'B' || header->header[1] != 'M') {
    throw std::runtime_error{"Corrupt bitmap"};
  }

  if (header->fileSize != data.size()) {
    throw std::runtime_error{"Corrupt bitmap"};
  }

  if (data.size() - sizeof(BitmapHeader) < sizeof(BitmapInfoHeader)) {
    throw std::runtime_error{"Corrupt bitmap"};
  }

  const uint32_t infoHeaderSize =
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,cppcoreguidelines-pro-bounds-pointer-arithmetic)
      *reinterpret_cast<const uint32_t*>(data.data() + sizeof(BitmapHeader));
  if (infoHeaderSize != 40) {
    throw std::runtime_error{"Unsupported bitmap format"};
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto* infoHeader = reinterpret_cast<const BitmapInfoHeader*>(
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      data.data() + sizeof(BitmapHeader));

  const uint32_t imageWidth = infoHeader->imageWidth;
  const uint32_t imageHeight = infoHeader->imageHeight;

  if (infoHeader->colorPlanes != 1 || infoHeader->bitsPerPixel != 24 ||
      infoHeader->compressionInfo != BitmapInfoHeader::CompressionInfo::RGB ||
      infoHeader->paletteSize != 0 || infoHeader->importantColors != 0) {
    throw std::runtime_error{"Unsupported bitmap format"};
  }

  const uint32_t rowSize = ((imageWidth * 3 + 3) / 4) * 4;
  const uint32_t bitmapSize = rowSize * imageHeight;

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
  const std::byte* bitmapData =
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      data.data() + sizeof(BitmapHeader) + sizeof(BitmapInfoHeader);

  std::vector<std::byte> outData;
  outData.resize(static_cast<size_t>(imageWidth) * imageHeight * 4);

  for (uint32_t y = 0; y < imageHeight; y++) {
    for (uint32_t x = 0; x < imageWidth; x++) {
      const std::byte* pixel =
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
          bitmapData +
          ((static_cast<size_t>(y) * rowSize) + (static_cast<size_t>(x) * 3));
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      const std::byte r = pixel[0];
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      const std::byte g = pixel[1];
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      const std::byte b = pixel[2];

      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      std::byte* outPixel = &outData[0] +
          (4ull * ((imageHeight - 1 - y) * imageWidth + (imageWidth - 1 - x)));
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      outPixel[0] = r;
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      outPixel[1] = g;
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      outPixel[2] = b;
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      outPixel[3] =
          static_cast<std::byte>(std::numeric_limits<unsigned char>::max());
    }
  }

  return Image{
      .width = imageWidth,
      .height = imageHeight,
      .pixelData = std::move(outData)};
}

} // namespace blocks::loader
