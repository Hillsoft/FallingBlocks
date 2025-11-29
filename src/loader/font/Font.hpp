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
  int16_t rawValue = 0;

  FWord& operator+=(const FWord& other) {
    // NOLINTNEXTLINE(*-narrowing-conversions)
    rawValue += other.rawValue;
    return *this;
  }

  FWord operator-() const { return FWord{static_cast<int16_t>(-rawValue)}; }

  FWord operator-(const FWord& other) const {
    return FWord{static_cast<int16_t>(rawValue - other.rawValue)};
  }

  FWord operator/(int16_t divisor) const {
    return FWord{static_cast<int16_t>(rawValue / divisor)};
  }
};

struct UFWord {
  uint16_t rawValue = 0;
};

class CharToGlyphMap {
 public:
  CharToGlyphMap() = default;

  CharToGlyphMap(const CharToGlyphMap& other) = default;
  CharToGlyphMap& operator=(const CharToGlyphMap& other) = default;

  CharToGlyphMap(CharToGlyphMap&& other) = default;
  CharToGlyphMap& operator=(CharToGlyphMap&& other) = default;

  virtual ~CharToGlyphMap() = default;

  virtual uint16_t mapChar(uint32_t unicodeChar) = 0;
};

struct SimpleGlyphData {
  std::vector<uint16_t> endPoints;
  std::vector<int16_t> xCoords;
  std::vector<int16_t> yCoords;
  std::vector<bool> onCurve;
};

struct CompoundGlyphData {
  uint16_t glpyhIndex = 0;
  float a = 0.0f;
  float b = 0.0f;
  float c = 0.0f;
  float d = 0.0f;
  int32_t e = 0;
  int32_t f = 0;
  bool areOffsets = false; // otherwise are points
};

struct GlyphContourData {
  FWord xMin{};
  FWord yMin{};
  FWord xMax{};
  FWord yMax{};
  std::variant<std::monostate, SimpleGlyphData, std::vector<CompoundGlyphData>>
      data;
};

struct GlyphHorizontalMetrics {
  UFWord advanceWidth;
  FWord leftSideBearing;
};

struct GlyphVerticalMetrics {
  UFWord advanceHeight;
  FWord topSideBearing;
};

struct GlyphData {
  GlyphContourData contourData;
  GlyphHorizontalMetrics horizontalMetrics;
  GlyphVerticalMetrics verticalMetrics;
};

// NOLINTNEXTLINE(bugprone-exception-escape)
class KerningTable {
 public:
  KerningTable() = default;
  explicit KerningTable(std::unordered_map<uint32_t, FWord> data)
      : data_(std::move(data)) {}

  [[nodiscard]] FWord apply(uint16_t leftGlyph, uint16_t rightGlyph) const;

 private:
  std::unordered_map<uint32_t, FWord> data_;
};

// NOLINTNEXTLINE(bugprone-exception-escape)
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
