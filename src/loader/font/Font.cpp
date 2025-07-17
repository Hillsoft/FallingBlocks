#include "loader/font/Font.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <vector>
#include "util/debug.hpp"
#include "util/file.hpp"

namespace blocks::loader {

namespace {

class CharToGlyphMap {
 public:
  virtual ~CharToGlyphMap() {}

  virtual uint16_t mapChar(uint32_t unicodeChar) = 0;
};

struct Fixed {
  uint32_t rawValue;

  auto operator<=>(const Fixed&) const = default;
};

struct FWord {
  int16_t rawValue;
};

struct UFWord {
  uint16_t rawValue;
};

constexpr Fixed kExpectedVersion = {0x00010000};

constexpr uint32_t kTagCmap = 0x636D6170;
constexpr uint32_t kTagHead = 0x68656164;
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

    if (startCodes_[segmentIndex] < charValue) {
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
    if (current.platformId == 0 && current.platformSpecificId == 3) {
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

void loadFont(const std::filesystem::path& path) {
  return loadFont(util::readFileBytes(path));
}

void loadFont(const std::span<const std::byte> data) {
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

    std::cout
        << static_cast<char>((entry.tag >> 24) & 0xff)
        << static_cast<char>((entry.tag >> 16) & 0xff)
        << static_cast<char>((entry.tag >> 8) & 0xff)
        << static_cast<char>((entry.tag >> 0) & 0xff) << ": "
        << (checksumValid ? "Checksum valid" : "Checksum invalid") << ", "
        << entry.length << " bytes" << std::endl;

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
}

} // namespace blocks::loader
