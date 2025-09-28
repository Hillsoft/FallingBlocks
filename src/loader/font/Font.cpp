#include "loader/font/Font.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include "util/debug.hpp"
#include "util/file.hpp"

namespace blocks::loader {

namespace {

struct Fixed {
  uint32_t rawValue;

  auto operator<=>(const Fixed&) const = default;
};

struct UFWord {
  uint16_t rawValue;
};

constexpr Fixed kExpectedVersion = {0x00010000};

constexpr uint32_t kTagCmap = 0x636D6170;
constexpr uint32_t kTagGlyf = 0x676C7966;
constexpr uint32_t kTagHead = 0x68656164;
constexpr uint32_t kTagHhea = 0x68686561;
constexpr uint32_t kTagHmtx = 0x686D7478;
constexpr uint32_t kTagKern = 0x6B65726E;
constexpr uint32_t kTagLoca = 0x6C6F6361;
constexpr uint32_t kTagMaxp = 0x6D617870;
constexpr uint32_t kTagName = 0x6E616D65;

template <typename TVal>
  requires std::is_integral_v<TVal>
TVal readBigEndian(std::span<const std::byte>& data) {
  DEBUG_ASSERT(data.size() >= sizeof(TVal));
  std::array<std::byte, sizeof(TVal)> flippedData;
  for (int i = 0; i < sizeof(TVal); i++) {
    flippedData[sizeof(TVal) - 1 - i] = data[i];
  }
  data = data.subspan(sizeof(TVal));
  return *reinterpret_cast<TVal*>(&*flippedData.begin());
}

struct OffsetSubtable {
  uint32_t scalerType;
  uint16_t numTables;
  uint16_t searchRange;
  uint16_t entrySelector;
  uint16_t rangeShift;
};
static_assert(sizeof(OffsetSubtable) == 12);

OffsetSubtable readOffsetSubtable(std::span<const std::byte> data) {
  if (data.size() < 12) {
    throw std::runtime_error{"Corrupt font file"};
  }

  return OffsetSubtable{
      .scalerType = readBigEndian<uint32_t>(data),
      .numTables = readBigEndian<uint16_t>(data),
      .searchRange = readBigEndian<uint16_t>(data),
      .entrySelector = readBigEndian<uint16_t>(data),
      .rangeShift = readBigEndian<uint16_t>(data)};
}

struct TableDirectoryEntry {
  uint32_t tag;
  uint32_t checksum;
  uint32_t offset;
  uint32_t length;
};
static_assert(sizeof(TableDirectoryEntry) == 16);

TableDirectoryEntry readTableDirectory(std::span<const std::byte> data) {
  return TableDirectoryEntry{
      .tag = readBigEndian<uint32_t>(data),
      .checksum = readBigEndian<uint32_t>(data),
      .offset = readBigEndian<uint32_t>(data),
      .length = readBigEndian<uint32_t>(data)};
}

struct HeadTable {
  Fixed version;
  Fixed fontRevision;
  uint32_t checkSumAdjustment;
  uint32_t magicNumber;
  uint16_t flags;
  uint16_t unitsPerEm;
  uint64_t created;
  uint64_t modified;
  FWord xMin;
  FWord yMin;
  FWord xMax;
  FWord yMax;
  uint16_t style;
  uint16_t lowestRecPPEM;
  int16_t directionHint;
  int16_t indexToLocFormat;
  int16_t glyphDataFormat;
};

HeadTable readHeadTable(std::span<const std::byte> data) {
  if (data.size() != 54) {
    throw std::runtime_error{"Corrupt font file"};
  }

  return {
      .version = {readBigEndian<uint32_t>(data)},
      .fontRevision = {readBigEndian<uint32_t>(data)},
      .checkSumAdjustment = readBigEndian<uint32_t>(data),
      .magicNumber = readBigEndian<uint32_t>(data),
      .flags = readBigEndian<uint16_t>(data),
      .unitsPerEm = readBigEndian<uint16_t>(data),
      .created = readBigEndian<uint64_t>(data),
      .modified = readBigEndian<uint64_t>(data),
      .xMin = {readBigEndian<int16_t>(data)},
      .yMin = {readBigEndian<int16_t>(data)},
      .xMax = {readBigEndian<int16_t>(data)},
      .yMax = {readBigEndian<int16_t>(data)},
      .style = readBigEndian<uint16_t>(data),
      .lowestRecPPEM = readBigEndian<uint16_t>(data),
      .directionHint = readBigEndian<int16_t>(data),
      .indexToLocFormat = readBigEndian<int16_t>(data),
      .glyphDataFormat = readBigEndian<int16_t>(data)};
}

struct NameRecord {
  uint16_t platformId;
  uint16_t patformSpecificId;
  uint16_t language;
  uint16_t nameId;
  std::string_view value;
};

struct NameTable {
  uint16_t format;
  uint16_t count;
  uint16_t stringOffset;
  std::vector<NameRecord> names;
};

NameTable readNameTable(std::span<const std::byte> data) {
  if (data.size() < 6) {
    throw std::runtime_error{"Corrupt font file"};
  }

  const std::span<const std::byte> originalData = data;

  NameTable result{
      .format = readBigEndian<uint16_t>(data),
      .count = readBigEndian<uint16_t>(data),
      .stringOffset = readBigEndian<uint16_t>(data),
      .names = {}};

  if (result.format != 0) {
    throw std::runtime_error{"Corrupt font file"};
  }
  if (result.stringOffset != 6 + 12 * result.count) {
    throw std::runtime_error{"Corrupt font file"};
  }

  const std::span<const std::byte> stringData =
      originalData.subspan(result.stringOffset);

  result.names.reserve(result.count);
  for (int i = 0; i < result.count; i++) {
    NameRecord curRecord{
        .platformId = readBigEndian<uint16_t>(data),
        .patformSpecificId = readBigEndian<uint16_t>(data),
        .language = readBigEndian<uint16_t>(data),
        .nameId = readBigEndian<uint16_t>(data)};
    uint16_t length = readBigEndian<uint16_t>(data);
    uint16_t offset = readBigEndian<uint16_t>(data);
    if (offset + length > stringData.size()) {
      throw std::runtime_error{"Corrupt font file"};
    }
    curRecord.value = std::string_view{
        reinterpret_cast<const char*>(&stringData[offset]), length};
    result.names.emplace_back(curRecord);
  }

  return result;
}

// Two-byte encoding, sparse data
class Format4Map : public CharToGlyphMap {
 public:
  Format4Map(
      uint16_t segCount,
      std::vector<uint16_t> endCodes,
      std::vector<uint16_t> startCodes,
      std::vector<uint16_t> idDelta,
      std::vector<uint16_t> idRangeOffset,
      std::vector<uint16_t> glyphIndices)
      : segCount_(segCount),
        endCodes_(std::move(endCodes)),
        startCodes_(std::move(startCodes)),
        idDelta_(std::move(idDelta)),
        idRangeOffset_(std::move(idRangeOffset)),
        glyphIndices_(std::move(glyphIndices)) {
    DEBUG_ASSERT(endCodes_.size() == segCount_);
    DEBUG_ASSERT(startCodes_.size() == segCount_);
    DEBUG_ASSERT(idDelta_.size() == segCount_);
    DEBUG_ASSERT(idRangeOffset_.size() == segCount_);
  }

  uint16_t mapChar(uint32_t unicodeChar) final {
    if (unicodeChar > 0xFFFF) {
      return 0;
    }
    uint16_t charValue = static_cast<uint16_t>(unicodeChar);

    int segmentIndex = 0;
    for (; segmentIndex < segCount_; segmentIndex++) {
      if (endCodes_[segmentIndex] >= charValue) {
        break;
      }
    }

    if (startCodes_[segmentIndex] > charValue) {
      return 0;
    }

    if (idRangeOffset_[segmentIndex] != 0) {
      int glyphIndexArrayIndex = idRangeOffset_[segmentIndex] / 2 + charValue -
          startCodes_[segmentIndex] - (segCount_ - segmentIndex);
      return glyphIndices_[glyphIndexArrayIndex];
    } else {
      return idDelta_[segmentIndex] + charValue;
    }
  }

 private:
  uint16_t segCount_;
  std::vector<uint16_t> endCodes_;
  std::vector<uint16_t> startCodes_;
  std::vector<uint16_t> idDelta_;
  std::vector<uint16_t> idRangeOffset_;
  std::vector<uint16_t> glyphIndices_;
};

Format4Map readCharMapSubtableFormat4(std::span<const std::byte> data) {
  uint16_t length = readBigEndian<uint16_t>(data);
  // We've already read the format and length
  if (length < 14 || data.size() < length - 4) {
    throw std::runtime_error{"Corrupt font file"};
  }
  data = data.subspan(0, length - 4);

  // language code, unused
  readBigEndian<uint16_t>(data);
  uint16_t segCount2 = readBigEndian<uint16_t>(data);
  uint16_t segCount = segCount2 / 2;
  // search range, unused
  readBigEndian<uint16_t>(data);
  // entry selector, unused
  readBigEndian<uint16_t>(data);
  // range shift, unused
  readBigEndian<uint16_t>(data);

  if (length < 16 + 8 * segCount) {
    throw std::runtime_error{"Corrupt font file"};
  }

  auto readSegCountVec = [&]() {
    std::vector<uint16_t> result;
    result.reserve(segCount);
    for (int i = 0; i < segCount; i++) {
      result.emplace_back(readBigEndian<uint16_t>(data));
    }
    return result;
  };

  std::vector<uint16_t> endCodes = readSegCountVec();

  uint16_t padding = readBigEndian<uint16_t>(data);
  if (padding != 0) {
    throw std::runtime_error{"Corrupt font file"};
  }

  std::vector<uint16_t> startCodes = readSegCountVec();
  std::vector<uint16_t> idDelta = readSegCountVec();
  std::vector<uint16_t> idRangeOffset = readSegCountVec();

  size_t glyphIndexCount = data.size() / 2;
  std::vector<uint16_t> glyphIndices;
  glyphIndices.reserve(glyphIndexCount);
  for (int i = 0; i < glyphIndexCount; i++) {
    glyphIndices.emplace_back(readBigEndian<uint16_t>(data));
  }

  for (int i = 0; i < segCount; i++) {
    // validate glyph indices lookup
    if (idRangeOffset[i] == 0) {
      continue;
    }

    int glyphIndexArrayIndex =
        idRangeOffset[i] / 2 + startCodes[i] - startCodes[i] - (segCount - i);
    if (glyphIndexArrayIndex < 0 ||
        glyphIndexArrayIndex >= glyphIndices.size()) {
      throw std::runtime_error{"Corrupt font file"};
    }

    glyphIndexArrayIndex =
        idRangeOffset[i] / 2 + endCodes[i] - startCodes[i] - (segCount - i);
    if (glyphIndexArrayIndex < 0 ||
        glyphIndexArrayIndex >= glyphIndices.size()) {
      throw std::runtime_error{"Corrupt font file"};
    }
  }

  return Format4Map(
      segCount,
      std::move(endCodes),
      std::move(startCodes),
      std::move(idDelta),
      std::move(idRangeOffset),
      std::move(glyphIndices));
}

std::unique_ptr<CharToGlyphMap> readCharMapTable(
    std::span<const std::byte> data) {
  struct SubtableHeader {
    uint16_t platformId;
    uint16_t platformSpecificId;
    uint32_t offset;
  };

  const std::span<const std::byte> originalData = data;

  if (data.size() < 4) {
    throw std::runtime_error{"Corrupt font file"};
  }
  uint16_t version = readBigEndian<uint16_t>(data);
  if (version != 0) {
    throw std::runtime_error{"Unsupported font file"};
  }
  uint16_t subtableCount = readBigEndian<uint16_t>(data);

  if (data.size() < 8 * static_cast<size_t>(subtableCount)) {
    throw std::runtime_error{"Corrupt font file"};
  }

  std::optional<SubtableHeader> m_selectedSubtable;

  for (int i = 0; i < subtableCount; i++) {
    SubtableHeader current{
        .platformId = readBigEndian<uint16_t>(data),
        .platformSpecificId = readBigEndian<uint16_t>(data),
        .offset = readBigEndian<uint32_t>(data)};
    // Only support one cmap type for now
    if ((current.platformId == 0 && current.platformSpecificId == 3) ||
        (current.platformId == 3 && current.platformSpecificId == 1)) {
      m_selectedSubtable.emplace(current);
    }
  }
  if (!m_selectedSubtable.has_value()) {
    throw std::runtime_error{"Unsupported font file"};
  }
  const SubtableHeader& selectedSubtable = *m_selectedSubtable;

  data = originalData.subspan(selectedSubtable.offset);
  if (data.size() < 2) {
    throw std::runtime_error{"Corrupt font file"};
  }

  uint16_t format = readBigEndian<uint16_t>(data);

  switch (format) {
    case 4:
      return std::make_unique<Format4Map>(readCharMapSubtableFormat4(data));
    default:
      throw std::runtime_error{"Unsupported font file"};
  }
}

struct MaximumProfile {
  Fixed version;
  uint16_t numGlyphs;
  uint16_t maxPoints;
  uint16_t maxContours;
  uint16_t maxComponentPoints;
  uint16_t maxComponentContours;
  uint16_t maxZones;
  uint16_t maxTwilightPoints;
  uint16_t maxStorage;
  uint16_t maxFunctionDefs;
  uint16_t maxInstructionDefs;
  uint16_t maxStackElements;
  uint16_t maxSizeOfInstructions;
  uint16_t maxComponentElements;
  uint16_t maxComponentDepth;
};

MaximumProfile readMaximumProfile(std::span<const std::byte> data) {
  if (data.size() != 32) {
    throw std::runtime_error{"Corrupt font file"};
  }
  MaximumProfile profile{
      .version = {readBigEndian<uint32_t>(data)},
      .numGlyphs = readBigEndian<uint16_t>(data),
      .maxPoints = readBigEndian<uint16_t>(data),
      .maxContours = readBigEndian<uint16_t>(data),
      .maxComponentPoints = readBigEndian<uint16_t>(data),
      .maxComponentContours = readBigEndian<uint16_t>(data),
      .maxZones = readBigEndian<uint16_t>(data),
      .maxTwilightPoints = readBigEndian<uint16_t>(data),
      .maxStorage = readBigEndian<uint16_t>(data),
      .maxFunctionDefs = readBigEndian<uint16_t>(data),
      .maxInstructionDefs = readBigEndian<uint16_t>(data),
      .maxStackElements = readBigEndian<uint16_t>(data),
      .maxSizeOfInstructions = readBigEndian<uint16_t>(data),
      .maxComponentElements = readBigEndian<uint16_t>(data),
      .maxComponentDepth = readBigEndian<uint16_t>(data)};
  if (profile.version != Fixed{0x00010000} || profile.maxZones != 2) {
    throw std::runtime_error{"Corrupt font file"};
  }
  return profile;
}

struct GlyphLocations {
  std::vector<uint32_t> offsets;
};

template <bool isShort>
GlyphLocations readGlyphLocationsImpl(
    uint16_t numGlyphs, std::span<const std::byte> data) {
  using TRawType = std::conditional_t<isShort, uint16_t, uint32_t>;
  if (data.size() != numGlyphs * sizeof(TRawType)) {
    throw std::runtime_error{"Corrupt font file"};
  }

  GlyphLocations result;
  result.offsets.reserve(numGlyphs);
  for (int i = 0; i < numGlyphs; i++) {
    uint32_t curOffset = static_cast<uint32_t>(readBigEndian<TRawType>(data));
    if constexpr (isShort) {
      curOffset *= 2;
    }
    result.offsets.emplace_back(curOffset);
  }
  return result;
}

GlyphLocations readGlyphLocations(
    const HeadTable& header,
    const MaximumProfile& maxProfile,
    std::span<const std::byte> data) {
  if (header.indexToLocFormat == 0) {
    return readGlyphLocationsImpl<true>(maxProfile.numGlyphs + 1, data);
  } else {
    return readGlyphLocationsImpl<false>(maxProfile.numGlyphs + 1, data);
  }
}

SimpleGlyphData readSimpleGlyph(
    int16_t numContours, std::span<const std::byte>& data) {
  struct GlyphFlags {
    uint8_t value;
    bool onCurve() const { return (value & 0x1) > 0; }
    bool xShort() const { return (value & 0x2) > 0; }
    bool yShort() const { return (value & 0x4) > 0; }
    bool repeat() const { return (value & 0x8) > 0; }
    bool xSame() const { return (value & 0x10) > 0; }
    bool ySame() const { return (value & 0x20) > 0; }
  };

  DEBUG_ASSERT(numContours >= 0);
  if (data.size() < 2 * static_cast<size_t>(numContours) + 2) {
    throw std::runtime_error{"Corrupt font file"};
  }

  std::vector<uint16_t> endPoints;
  endPoints.reserve(numContours);

  if (numContours > 0) {
    endPoints.emplace_back(readBigEndian<uint16_t>(data));
    for (int i = 1; i < numContours; i++) {
      endPoints.emplace_back(readBigEndian<uint16_t>(data));
      if (endPoints[i] < endPoints[i - 1]) {
        throw std::runtime_error{"Corrupt font file"};
      }
    }
  }

  uint16_t numPoints = numContours > 0 ? 1 + endPoints.back() : 0;

  uint16_t instructionLength = readBigEndian<uint16_t>(data);
  if (data.size() < static_cast<size_t>(instructionLength)) {
    throw std::runtime_error{"Corrupt font file"};
  }
  data = data.subspan(instructionLength);

  std::vector<GlyphFlags> flags;
  flags.reserve(numPoints);

  size_t pointBytes = 0;

  for (int i = 0; i < numPoints; i++) {
    if (data.size() < 1) {
      throw std::runtime_error{"Corrupt font file"};
    }
    GlyphFlags current{readBigEndian<uint8_t>(data)};
    flags.emplace_back(current);

    pointBytes += current.xShort() ? 1 : (current.xSame() ? 0 : 2);
    pointBytes += current.yShort() ? 1 : (current.ySame() ? 0 : 2);

    if (current.repeat()) {
      if (data.size() < 1) {
        throw std::runtime_error{"Corrupt font file"};
      }
      int repetitions = readBigEndian<uint8_t>(data);

      pointBytes += static_cast<size_t>(repetitions) *
          (current.xShort() ? 1 : (current.xSame() ? 0 : 2));
      pointBytes += static_cast<size_t>(repetitions) *
          (current.yShort() ? 1 : (current.ySame() ? 0 : 2));

      if (i + repetitions >= numPoints) {
        throw std::runtime_error{"Corrupt font file"};
      }
      for (int j = 0; j < repetitions; j++) {
        flags.emplace_back(current);
        i++;
      }
    }
  }

  if (data.size() < pointBytes) {
    throw std::runtime_error{"Corrupt font file"};
  }

  int16_t baseXCoord = 0;
  std::vector<int16_t> xCoords;
  xCoords.reserve(numPoints);
  for (int i = 0; i < numPoints; i++) {
    GlyphFlags current = flags[i];
    if (current.xShort()) {
      int16_t curOffset = static_cast<int16_t>(readBigEndian<uint8_t>(data));
      if (!current.xSame()) {
        curOffset *= -1;
      }
      baseXCoord += curOffset;
    } else {
      if (!current.xSame()) {
        int16_t curOffset = readBigEndian<int16_t>(data);
        baseXCoord += curOffset;
      }
    }

    xCoords.emplace_back(baseXCoord);
  }

  int16_t baseYCoord = 0;
  std::vector<int16_t> yCoords;
  yCoords.reserve(numPoints);
  for (int i = 0; i < numPoints; i++) {
    GlyphFlags current = flags[i];
    if (current.yShort()) {
      int16_t curOffset = static_cast<int16_t>(readBigEndian<uint8_t>(data));
      if (!current.ySame()) {
        curOffset *= -1;
      }
      baseYCoord += curOffset;
    } else {
      if (!current.ySame()) {
        int16_t curOffset = readBigEndian<int16_t>(data);
        baseYCoord += curOffset;
      }
    }

    yCoords.emplace_back(baseYCoord);
  }

  std::vector<bool> onCurve;
  onCurve.reserve(numPoints);
  for (const auto& flag : flags) {
    onCurve.emplace_back(flag.onCurve());
  }

  return {
      std::move(endPoints),
      std::move(xCoords),
      std::move(yCoords),
      std::move(onCurve)};
}

std::vector<CompoundGlyphData> readCompoundGlyph(
    std::span<const std::byte>& data) {
  struct CompoundFlag {
    uint16_t rawValue;
    bool argsAreWords() const { return (rawValue & 0x1) > 0; }
    bool argsAreXY() const { return (rawValue & 0x2) > 0; }
    bool roundToGrid() const { return (rawValue & 0x4) > 0; }
    bool hasSimpleScale() const { return (rawValue & 0x8) > 0; }
    bool moreComponents() const { return (rawValue & 0x20) > 0; }
    bool hasXYScale() const { return (rawValue & 0x40) > 0; }
    bool hasTransform() const { return (rawValue & 0x80) > 0; }
    bool hasInstructions() const { return (rawValue & 0x100) > 0; }
    bool useMyMetrics() const { return (rawValue & 0x200) > 0; }
    bool overlapCompound() const { return (rawValue & 0x400) > 0; }
  };

  auto convertScaleFloat = [](int16_t rawValue) {
    return static_cast<float>(rawValue) / static_cast<float>(1 << 14);
  };

  std::vector<CompoundGlyphData> result;
  CompoundFlag curFlags{0};
  bool hasInstructions = false;

  do {
    if (data.size() < 4) {
      throw std::runtime_error{"Corrupt font file"};
    }

    curFlags = {readBigEndian<uint16_t>(data)};
    hasInstructions |= curFlags.hasInstructions();

    uint16_t glyphIndex = readBigEndian<uint16_t>(data);

    if ((curFlags.argsAreWords() && data.size() < 4) ||
        (!curFlags.argsAreWords() && data.size() < 2)) {
      throw std::runtime_error{"Corrupt font file"};
    }

    int32_t argument1;
    int32_t argument2;
    if (curFlags.argsAreWords() && curFlags.argsAreXY()) {
      argument1 = readBigEndian<int16_t>(data);
      argument2 = readBigEndian<int16_t>(data);
    } else if (curFlags.argsAreWords() && !curFlags.argsAreXY()) {
      argument1 = readBigEndian<uint16_t>(data);
      argument2 = readBigEndian<uint16_t>(data);
    } else if (!curFlags.argsAreWords() && curFlags.argsAreXY()) {
      argument1 = readBigEndian<int8_t>(data);
      argument2 = readBigEndian<int8_t>(data);
    } else {
      argument1 = readBigEndian<uint8_t>(data);
      argument2 = readBigEndian<uint8_t>(data);
    }

    // Defaults
    float a = 1.0f;
    float b = 0.0f;
    float c = 0.0f;
    float d = 1.0f;
    if (curFlags.hasSimpleScale()) {
      if (data.size() < 2) {
        throw std::runtime_error{"Corrupt font file"};
      }
      a = convertScaleFloat(readBigEndian<int16_t>(data));
      d = a;
    } else if (curFlags.hasXYScale()) {
      if (data.size() < 4) {
        throw std::runtime_error{"Corrupt font file"};
      }
      a = convertScaleFloat(readBigEndian<int16_t>(data));
      d = convertScaleFloat(readBigEndian<int16_t>(data));
    } else if (curFlags.hasTransform()) {
      if (data.size() < 8) {
        throw std::runtime_error{"Corrupt font file"};
      }
      a = convertScaleFloat(readBigEndian<int16_t>(data));
      b = convertScaleFloat(readBigEndian<int16_t>(data));
      c = convertScaleFloat(readBigEndian<int16_t>(data));
      d = convertScaleFloat(readBigEndian<int16_t>(data));
    }
    result.emplace_back(
        glyphIndex, a, b, c, d, argument1, argument2, curFlags.argsAreXY());
  } while (curFlags.moreComponents());

  if (hasInstructions) {
    uint16_t instructionCount = readBigEndian<uint16_t>(data);
    if (data.size() < instructionCount) {
      throw std::runtime_error{"Corrupt font file"};
    }
    data = data.subspan(instructionCount);
  }

  return result;
}

std::vector<GlyphData> readGlyphTable(
    const GlyphLocations& glyphLocations, std::span<const std::byte> data) {
  std::vector<GlyphData> result;
  result.reserve(glyphLocations.offsets.size());

  for (size_t i = 0; i < glyphLocations.offsets.size(); i++) {
    long long curSize =
        static_cast<size_t>(i) + 1 < glyphLocations.offsets.size()
        ? glyphLocations.offsets[i + 1] - glyphLocations.offsets[i]
        : data.size() - glyphLocations.offsets[i];
    if (curSize < 0) {
      throw std::runtime_error{"Corrupt font file"};
    }
    if (curSize == 0) {
      result.emplace_back(GlyphData{{0}, {0}, {0}, {0}, std::monostate{}});
      continue;
    }

    std::span<const std::byte> glyphData =
        data.subspan(glyphLocations.offsets[i], curSize);

    if (glyphData.size() < 10) {
      throw std::runtime_error{"Corrupt font file"};
    }
    int16_t numContours = readBigEndian<int16_t>(glyphData);

    FWord xMin{readBigEndian<int16_t>(glyphData)};
    FWord yMin{readBigEndian<int16_t>(glyphData)};
    FWord xMax{readBigEndian<int16_t>(glyphData)};
    FWord yMax{readBigEndian<int16_t>(glyphData)};

    if (numContours >= 0) {
      result.emplace_back(GlyphData{
          xMin, yMin, xMax, yMax, {readSimpleGlyph(numContours, glyphData)}});
    } else {
      result.emplace_back(
          GlyphData{xMin, yMin, xMax, yMax, {readCompoundGlyph(glyphData)}});
    }
    if (glyphData.size() >= 2) {
      throw std::runtime_error{"Corrupt font file"};
    }
  }

  if (result.size() < 2) {
    throw std::runtime_error{"Corrupt font file"};
  }

  return result;
}

struct HorizontalHeader {
  Fixed version;
  FWord ascent;
  FWord descent;
  FWord lineGap;
  UFWord advanceWidthMax;
  FWord minLeftSideBearing;
  FWord minRightSideBearing;
  FWord xMaxExtent;
  int16_t caretSlopeRise;
  int16_t caretSlopeRun;
  FWord caretOffset;
  int16_t metricDataFormat;
  uint16_t numOfLongHorMetrics;
};

HorizontalHeader readHorizontalHeader(std::span<const std::byte> data) {
  if (data.size() != 36) {
    throw std::runtime_error{"Corrupt font file"};
  }
  HorizontalHeader header{};
  header.version = {readBigEndian<uint32_t>(data)};
  if (header.version != Fixed{0x00010000}) {
    throw std::runtime_error{"Unsupported font file"};
  }
  header.ascent = {readBigEndian<int16_t>(data)};
  header.descent = {readBigEndian<int16_t>(data)};
  header.lineGap = {readBigEndian<int16_t>(data)};
  header.advanceWidthMax = {readBigEndian<uint16_t>(data)};
  header.minLeftSideBearing = {readBigEndian<int16_t>(data)};
  header.minRightSideBearing = {readBigEndian<int16_t>(data)};
  header.xMaxExtent = {readBigEndian<int16_t>(data)};
  header.caretSlopeRise = readBigEndian<int16_t>(data);
  header.caretSlopeRun = readBigEndian<int16_t>(data);
  header.caretOffset = {readBigEndian<int16_t>(data)};
  data = data.subspan(8);
  header.metricDataFormat = readBigEndian<int16_t>(data);
  header.numOfLongHorMetrics = readBigEndian<uint16_t>(data);
  if (header.numOfLongHorMetrics == 0) {
    throw std::runtime_error{"Corrupt font file"};
  }
  return header;
}

HorizontalMetrics readHorizontalMetrics(
    const MaximumProfile& maxProfile,
    const HorizontalHeader& hheader,
    std::span<const std::byte> data) {
  if (data.size() !=
      2 * static_cast<size_t>(maxProfile.numGlyphs) +
          2 * static_cast<size_t>(hheader.numOfLongHorMetrics)) {
    throw std::runtime_error{"Corrupt font file"};
  }

  HorizontalMetrics metrics;
  metrics.data.reserve(maxProfile.numGlyphs);

  uint16_t lastAdvanceWidth = 0;
  for (int i = 0; i < hheader.numOfLongHorMetrics; i++) {
    HorizontalMetrics::Entry cur{
        readBigEndian<uint16_t>(data), readBigEndian<int16_t>(data)};
    lastAdvanceWidth = cur.advanceWidth;
    metrics.data.emplace_back(cur);
  }

  for (int i = 0; i < maxProfile.numGlyphs - hheader.numOfLongHorMetrics; i++) {
    metrics.data.emplace_back(lastAdvanceWidth, readBigEndian<int16_t>(data));
  }

  return metrics;
}

uint32_t kerningMapKey(uint16_t left, uint16_t right) {
  return (static_cast<uint32_t>(left) << 16) + right;
}

void readKerningSubtable(
    std::span<const std::byte>& data,
    std::unordered_map<uint32_t, FWord>& kernValues) {
  if (data.size() < 6) {
    throw std::runtime_error{"Corrupt font file"};
  }

  uint16_t version = readBigEndian<uint16_t>(data);
  if (version != 0) {
    throw std::runtime_error{"Unsupported font file"};
  }

  readBigEndian<uint16_t>(data); // length
  uint8_t format = readBigEndian<uint8_t>(data);
  readBigEndian<uint8_t>(data); // flags

  if (format != 0) {
    throw std::runtime_error{"Unsupported font file"};
  }

  // Format 0 header
  if (data.size() < 8) {
    throw std::runtime_error{"Corrupt font file"};
  }

  uint16_t numPairs = readBigEndian<uint16_t>(data);
  readBigEndian<uint16_t>(data); // search range
  readBigEndian<uint16_t>(data); // entry selector
  readBigEndian<uint16_t>(data); // range shift

  if (data.size() < 6 * static_cast<size_t>(numPairs)) {
    throw std::runtime_error{"Corrupt font file"};
  }

  kernValues.reserve(kernValues.size() + numPairs);
  for (int i = 0; i < numPairs; i++) {
    uint16_t left = readBigEndian<uint16_t>(data);
    uint16_t right = readBigEndian<uint16_t>(data);
    FWord value{readBigEndian<int16_t>(data)};

    uint32_t mapKey = kerningMapKey(left, right);

    if (auto it = kernValues.find(mapKey); it != kernValues.end()) {
      it->second += value;
    } else {
      kernValues.emplace(mapKey, value);
    }
  }
}

KerningTable readKerningTable(std::span<const std::byte> data) {
  if (data.size() < 4) {
    throw std::runtime_error{"Corrupt font file"};
  }

  uint16_t version = readBigEndian<uint16_t>(data);
  if (version != 0) {
    throw std::runtime_error{"Unsupported font file"};
  }
  uint16_t numTables = readBigEndian<uint16_t>(data);

  std::unordered_map<uint32_t, FWord> kernValues;
  for (int i = 0; i < numTables; i++) {
    readKerningSubtable(data, kernValues);
  }

  return KerningTable{std::move(kernValues)};
}

std::span<const std::byte> getTableContents(
    std::span<const std::byte> data,
    const TableDirectoryEntry& tableDescriptor) {
  auto result = data.subspan(tableDescriptor.offset, tableDescriptor.length);
  if (result.size() != tableDescriptor.length) {
    throw std::runtime_error{"Corrupt font file"};
  }
  return result;
}

std::span<const std::byte> getTableContentsWithPadding(
    std::span<const std::byte> data,
    const TableDirectoryEntry& tableDescriptor) {
  size_t paddedLength = 4ull * ((tableDescriptor.length + 3ull) / 4ull);
  auto result = data.subspan(tableDescriptor.offset, paddedLength);
  if (result.size() != paddedLength) {
    throw std::runtime_error{"Corrupt font file"};
  }
  return result;
}

uint32_t getTableChecksum(std::span<const std::byte> tableData) {
  uint32_t sum = 0;
  size_t nLongs = (tableData.size() + 3) / 4;
  while (nLongs-- > 0) {
    sum += readBigEndian<uint32_t>(tableData);
  }
  return sum;
}

const TableDirectoryEntry* lookupTable(
    std::span<const TableDirectoryEntry> tableDirectory, uint32_t tag) {
  for (const auto& entry : tableDirectory) {
    if (entry.tag == tag) {
      return &entry;
    }
  }
  return nullptr;
}

} // namespace

KerningTable::KerningTable() {}

KerningTable::KerningTable(std::unordered_map<uint32_t, FWord> data)
    : data_(std::move(data)) {}

FWord KerningTable::apply(uint16_t leftGlyph, uint16_t rightGlyph) const {
  if (auto it = data_.find(kerningMapKey(leftGlyph, rightGlyph));
      it != data_.end()) {
    return it->second;
  } else {
    return FWord{0};
  }
}

Font loadFont(const std::filesystem::path& path) {
  return loadFont(util::readFileBytes(path));
}

Font loadFont(const std::span<const std::byte> data) {
  const OffsetSubtable offsetSubtable =
      readOffsetSubtable(data.subspan(0, sizeof(OffsetSubtable)));

  if (data.size() < offsetSubtable.numTables * sizeof(TableDirectoryEntry)) {
    throw std::runtime_error{"Corrupt font file"};
  }

  std::vector<TableDirectoryEntry> tableDirectory;
  tableDirectory.reserve(offsetSubtable.numTables);
  for (int i = 0; i < offsetSubtable.numTables; i++) {
    tableDirectory.emplace_back(readTableDirectory(data.subspan(
        sizeof(OffsetSubtable) + i * sizeof(TableDirectoryEntry),
        sizeof(TableDirectoryEntry))));
  }

  // Validate checksums

  for (const auto& entry : tableDirectory) {
    bool checksumValid =
        getTableChecksum(getTableContentsWithPadding(data, entry)) ==
        entry.checksum;

    // special handling for 'head'
    if (entry.tag == kTagHead) {
      checksumValid = getTableChecksum(data) == 0xB1B0AFBA;
    }

    if (!checksumValid) {
      throw std::runtime_error{"Corrupt font file"};
    }
  }

  HeadTable headEntry = [&]() {
    const auto entryPtr = lookupTable(tableDirectory, kTagHead);
    if (entryPtr == nullptr) {
      throw std::runtime_error{"Corrupt font file"};
    }
    return readHeadTable(getTableContents(data, *entryPtr));
  }();

  if (headEntry.version != kExpectedVersion) {
    throw std::runtime_error{"Unsupported font file"};
  }
  if (headEntry.magicNumber != 0x5F0F3CF5) {
    throw std::runtime_error{"Corrupt font file"};
  }

  NameTable nameTable = [&]() {
    const auto entryPtr = lookupTable(tableDirectory, kTagName);
    if (entryPtr == nullptr) {
      throw std::runtime_error{"Corrupt font file"};
    }
    return readNameTable(getTableContents(data, *entryPtr));
  }();

  std::unique_ptr<CharToGlyphMap> charMap = [&]() {
    const auto entryPtr = lookupTable(tableDirectory, kTagCmap);
    if (entryPtr == nullptr) {
      throw std::runtime_error{"Corrupt font file"};
    }
    return readCharMapTable(getTableContents(data, *entryPtr));
  }();

  MaximumProfile maxProfile = [&]() {
    const auto entryPtr = lookupTable(tableDirectory, kTagMaxp);
    if (entryPtr == nullptr) {
      throw std::runtime_error{"Corrupt font file"};
    }
    return readMaximumProfile(getTableContents(data, *entryPtr));
  }();

  GlyphLocations glyphLocations = [&]() {
    const auto entryPtr = lookupTable(tableDirectory, kTagLoca);
    if (entryPtr == nullptr) {
      throw std::runtime_error{"Corrupt font file"};
    }
    return readGlyphLocations(
        headEntry, maxProfile, getTableContents(data, *entryPtr));
  }();

  std::vector<GlyphData> glyphs = [&]() {
    const auto entryPtr = lookupTable(tableDirectory, kTagGlyf);
    if (entryPtr == nullptr) {
      throw std::runtime_error{"Corrupt font file"};
    }
    return readGlyphTable(glyphLocations, getTableContents(data, *entryPtr));
  }();

  HorizontalHeader horizontalHeader = [&]() {
    const auto entryPtr = lookupTable(tableDirectory, kTagHhea);
    if (entryPtr == nullptr) {
      throw std::runtime_error{"Corrupt font file"};
    }
    return readHorizontalHeader(getTableContents(data, *entryPtr));
  }();

  HorizontalMetrics horizontalMetrics = [&]() {
    const auto entryPtr = lookupTable(tableDirectory, kTagHmtx);
    if (entryPtr == nullptr) {
      throw std::runtime_error{"Corrupt font file"};
    }
    return readHorizontalMetrics(
        maxProfile, horizontalHeader, getTableContents(data, *entryPtr));
  }();

  KerningTable kerning = [&]() {
    const auto entryPtr = lookupTable(tableDirectory, kTagKern);
    if (entryPtr == nullptr) {
      return KerningTable{};
    }
    return readKerningTable(getTableContents(data, *entryPtr));
  }();

  return Font{
      .charMap = std::move(charMap),
      .glyphs = std::move(glyphs),
      .horizontalMetrics = std::move(horizontalMetrics),
      .unitsPerEm = headEntry.unitsPerEm,
      .kerning = std::move(kerning)};
}

} // namespace blocks::loader
