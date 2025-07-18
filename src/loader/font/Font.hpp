#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <span>
#include <variant>
#include <vector>

namespace blocks::loader {

struct FWord {
  int16_t rawValue;
};

class CharToGlyphMap {
 public:
  virtual ~CharToGlyphMap() {}

  virtual uint16_t mapChar(uint32_t unicodeChar) = 0;
};

struct SimpleGlpyhData {
  std::vector<uint16_t> endPoints;
  std::vector<int16_t> xCoords;
  std::vector<int16_t> yCoords;
  std::vector<bool> onCurve;
};

struct CompoundGlyphData {
  uint16_t glpyhIndex;
  float a;
  float b;
  float c;
  float d;
  int32_t e;
  int32_t f;
  bool areOffsets; // otherwise are points
};

struct GlyphData {
  FWord xMin;
  FWord yMin;
  FWord xMax;
  FWord yMax;
  std::variant<std::monostate, SimpleGlpyhData, std::vector<CompoundGlyphData>>
      data;
};

struct HorizontalMetrics {
  struct Entry {
    uint16_t advanceWidth;
    int16_t leftSideBearing;
  };

  std::vector<Entry> data;
};

struct Font {
  std::unique_ptr<CharToGlyphMap> charMap;
  std::vector<GlyphData> glyphs;
  HorizontalMetrics horizontalMetrics;
};

Font loadFont(const std::filesystem::path& path);
Font loadFont(std::span<const std::byte> data);

} // namespace blocks::loader
