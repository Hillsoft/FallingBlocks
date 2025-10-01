#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <span>
#include <unordered_map>
#include <variant>
#include <vector>

namespace blocks::loader {

struct FWord {
  int16_t rawValue;

  FWord& operator+=(const FWord& other) {
    rawValue += other.rawValue;
    return *this;
  }
};

struct UFWord {
  uint16_t rawValue;
};

class CharToGlyphMap {
 public:
  virtual ~CharToGlyphMap() {}

  virtual uint16_t mapChar(uint32_t unicodeChar) = 0;
};

struct SimpleGlyphData {
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

struct GlyphContourData {
  FWord xMin;
  FWord yMin;
  FWord xMax;
  FWord yMax;
  std::variant<std::monostate, SimpleGlyphData, std::vector<CompoundGlyphData>>
      data;
};

struct GlyphHorizontalMetrics {
  UFWord advanceWidth;
  FWord leftSideBearing;
};

struct GlyphData {
  GlyphContourData contourData;
  GlyphHorizontalMetrics horizontalMetrics;
};

class KerningTable {
 public:
  KerningTable();
  explicit KerningTable(std::unordered_map<uint32_t, FWord> data);

  FWord apply(uint16_t leftGlyph, uint16_t rightGlyph) const;

 private:
  std::unordered_map<uint32_t, FWord> data_;
};

struct Font {
  std::unique_ptr<CharToGlyphMap> charMap;
  std::vector<GlyphData> glyphs;
  uint16_t unitsPerEm;
  KerningTable kerning;
  FWord ascenderHeight;
  FWord descenderHeight;
  FWord lineGap;
};

Font loadFont(const std::filesystem::path& path);
Font loadFont(std::span<const std::byte> data);

} // namespace blocks::loader
