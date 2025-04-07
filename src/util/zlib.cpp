#include "util/zlib.hpp"

#include <array>
#include <cstdint>
#include <limits>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "util/debug.hpp"

namespace util {

namespace {

class BitStream {
 public:
  BitStream(std::span<const unsigned char> data)
      : data_(data), byteOffset_(0) {}

  uint64_t peekBits(int bitCount) const {
    DEBUG_ASSERT(bitCount <= 64);

    int maxDataOffset = (byteOffset_ + bitCount - 1) / 8;
    if (maxDataOffset >= data_.size()) {
      throw std::runtime_error{"Out-of-bounds bitstream read"};
    }

    int shift = 0;
    uint64_t result = 0;
    size_t dataOffset = 0;
    int curByteOffset = byteOffset_;

    while (bitCount > 0) {
      int curBitCount = std::min(8 - curByteOffset, bitCount);
      uint8_t mask = 0xFF >> (8 - curBitCount - curByteOffset);
      uint64_t curParts =
          static_cast<uint64_t>(mask & data_[dataOffset]) >> curByteOffset;

      result |= curParts << shift;

      shift += curBitCount;
      bitCount -= curBitCount;
      dataOffset++;
      curByteOffset = 0;
    }

    DEBUG_ASSERT(bitCount == 0 || dataOffset - 1 == maxDataOffset);

    return result;
  }

  uint64_t consumeBits(int bitCount) {
    uint64_t result = peekBits(bitCount);

    byteOffset_ += bitCount;
    data_ = data_.subspan(byteOffset_ / 8);
    byteOffset_ %= 8;

    return result;
  }

  bool consumeBit() {
    if (data_.size() == 0) {
      throw std::runtime_error{"Out-of-bounds bitstream read"};
    }
    bool result = ((data_[0] >> byteOffset_) & 0b1) > 0;
    byteOffset_ += 1;
    data_ = data_.subspan(byteOffset_ / 8);
    byteOffset_ %= 8;
    return result;
  }

  void byteAlign() {
    if (byteOffset_ != 0) {
      data_ = data_.subspan(1);
      byteOffset_ = 0;
    }
  }

 private:
  std::span<const unsigned char> data_;
  int byteOffset_;
};

constexpr size_t kMaxHuffmanCodeLength = 15;

class HuffmanTree {
 private:
  struct Node {
    bool isLeaf = false;
    short leafValueIndex = std::numeric_limits<short>::max();
    short l = std::numeric_limits<short>::max();
    short r = std::numeric_limits<short>::max();
  };

 public:
  template <typename TIntegral>
  HuffmanTree(std::span<const TIntegral> codeLengths)
    requires(std::is_integral_v<TIntegral>)
      : nodes_() {
    nodes_.reserve(2 * codeLengths.size() - 1);

#ifndef NDEBUG
    for (const auto& l : codeLengths) {
      DEBUG_ASSERT(l <= kMaxHuffmanCodeLength);
    }
#endif

    std::array<unsigned int, kMaxHuffmanCodeLength + 1> blCount{};
    for (const auto& l : codeLengths) {
      if (l > 0) {
        blCount[l]++;
      }
    }

    std::array<size_t, kMaxHuffmanCodeLength + 1> nextCode{};
    size_t code = 0;
    for (int bits = 1; bits <= kMaxHuffmanCodeLength; bits++) {
      code = (code + blCount[bits - 1]) << 1;
      nextCode[bits] = code;
      if (blCount[bits] > 0 && nextCode[bits] >= (1ULL << bits)) {
        throw std::runtime_error{"Corrupt zlib data"};
      }
    }

    nodes_.emplace_back();
    for (size_t i = 0; i < codeLengths.size(); i++) {
      size_t len = codeLengths[i];
      size_t c = nextCode[len];
      nextCode[len]++;

      if (len != 0) {
        Node* curNode = &nodes_[0];
        for (int j = static_cast<int>(len) - 1; j >= 0; j--) {
          bool isRight = ((c >> j) & 0b1) > 0;
          short& nextNodeIndexRef = isRight ? curNode->r : curNode->l;
          short nextNodeIndex = nextNodeIndexRef;
          if (nextNodeIndexRef == std::numeric_limits<short>::max()) {
            DEBUG_ASSERT(!curNode->isLeaf);
            nextNodeIndexRef = static_cast<short>(nodes_.size());
            nextNodeIndex = nextNodeIndexRef;
            nodes_.emplace_back();
          }
          curNode = &nodes_[nextNodeIndex];
        }
        DEBUG_ASSERT(!curNode->isLeaf);
        DEBUG_ASSERT(curNode->l == std::numeric_limits<short>::max());
        DEBUG_ASSERT(curNode->r == std::numeric_limits<short>::max());
        curNode->isLeaf = true;
        curNode->leafValueIndex = static_cast<short>(i);
      }
    }
  }

  short lookup(BitStream& stream) const {
    DEBUG_ASSERT(nodes_.size() > 0);
    const Node* curNode = &nodes_[0];

    while (!curNode->isLeaf) {
      bool rightNext = stream.consumeBit();
      short nextNodeIndex = rightNext ? curNode->r : curNode->l;
      if (nextNodeIndex == std::numeric_limits<short>::max()) {
        throw std::runtime_error{"Corrupt zlib data"};
      }
      curNode = &nodes_[nextNodeIndex];
    }

    return curNode->leafValueIndex;
  }

 private:
  std::vector<Node> nodes_;
};

struct ExtraBitCode {
  unsigned short extraBits;
  unsigned short offset;
};

constexpr std::array<ExtraBitCode, 29> kLengthCodes{
    {{0, 3},   {0, 4},   {0, 5},   {0, 6},   {0, 7},  {0, 8},
     {0, 9},   {0, 10},  {1, 11},  {1, 13},  {1, 15}, {1, 17},
     {2, 19},  {2, 23},  {2, 27},  {2, 31},  {3, 35}, {3, 43},
     {3, 51},  {3, 59},  {4, 67},  {4, 83},  {4, 99}, {4, 115},
     {5, 131}, {5, 163}, {5, 195}, {5, 227}, {0, 258}}};

constexpr std::array<ExtraBitCode, 30> kDistanceCodes{
    {{0, 1},     {0, 2},     {0, 3},      {0, 4},      {1, 5},
     {1, 7},     {2, 9},     {2, 13},     {3, 17},     {3, 25},
     {4, 33},    {4, 49},    {5, 65},     {5, 97},     {6, 129},
     {6, 193},   {7, 257},   {7, 385},    {8, 513},    {8, 769},
     {9, 1025},  {9, 1537},  {10, 2049},  {10, 3073},  {11, 4097},
     {11, 6145}, {12, 8193}, {12, 12289}, {13, 16385}, {13, 24577}}};

struct CompressedBlockTrees {
  HuffmanTree literalHuffmanEncoding;
  HuffmanTree distanceHuffmanEncoding;
};

CompressedBlockTrees fixedTrees = {
    .literalHuffmanEncoding =
        []() {
          std::array<short, 288> lengths{};
          for (int i = 0; i <= 143; i++) {
            lengths[i] = 8;
          }
          for (int i = 144; i <= 255; i++) {
            lengths[i] = 9;
          }
          for (int i = 256; i <= 279; i++) {
            lengths[i] = 7;
          }
          for (int i = 280; i <= 287; i++) {
            lengths[i] = 9;
          }
          return HuffmanTree{std::span<const short>{lengths}};
        }(),
    .distanceHuffmanEncoding =
        []() {
          std::array<short, 32> lengths{};
          for (int i = 0; i < 32; i++) {
            lengths[i] = 5;
          }
          return HuffmanTree{std::span<const short>{lengths}};
        }()};

struct ZlibHeader {
  uint8_t compressionMethod;
  uint8_t compressionInfo;
  bool presetDictionary;
};

ZlibHeader readZlibHeader(
    unsigned char compressionByte, unsigned char additionalFlags) {
  uint16_t combined = compressionByte << 8 | additionalFlags;
  if (combined % 31 != 0) {
    throw std::runtime_error{"Corrupt zlib data"};
  }

  ZlibHeader header{};
  header.compressionMethod = compressionByte & 0b1111;
  header.compressionInfo = (compressionByte >> 4) & 0b1111;
  header.presetDictionary = (additionalFlags & (1 << 5)) > 0;
  return header;
}

struct BlockHeader {
  enum class BlockType {
    NO_COMPRESSION = 0,
    FIXED_CODES = 1,
    DYNAMIC_CODES = 2
  };
  bool isFinal;
  BlockType type;
};

BlockHeader readBlockHeader(BitStream& stream) {
  BlockHeader header{};
  header.isFinal = stream.consumeBits(1) > 0;
  uint8_t rawType = static_cast<uint8_t>(stream.consumeBits(2));
  if (rawType > 2) {
    throw std::runtime_error{"Corrupt zlib data"};
  }
  header.type = static_cast<BlockHeader::BlockType>(rawType);
  return header;
}

constexpr int kMaxLiteralCount = 286;
constexpr int kMaxDistanceCodeCount = 30;
constexpr int kMaxLengthCodeCount = 4 + 0b1111;

constexpr std::array<short, 19> kCodeLengthAlphabet{
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

CompressedBlockTrees readDynamicCodesBlockHeader(BitStream& stream) {
  short literalCodeCount = static_cast<short>(257 + stream.consumeBits(5));
  if (literalCodeCount > kMaxLiteralCount) {
    throw std::runtime_error{"Corrupt zlib data"};
  }

  short distanceCodeCount = static_cast<short>(1 + stream.consumeBits(5));
  if (distanceCodeCount > kMaxDistanceCodeCount) {
    throw std::runtime_error{"Corrupt zlib data"};
  }
  short lengthCodeCount = static_cast<short>(4 + stream.consumeBits(4));
  DEBUG_ASSERT(lengthCodeCount <= kMaxLengthCodeCount);

  std::array<short, kMaxLengthCodeCount> lengthCodeLengths{};

  for (int i = 0; i < lengthCodeCount; i++) {
    lengthCodeLengths[kCodeLengthAlphabet[i]] =
        static_cast<short>(stream.consumeBits(3));
  }

  HuffmanTree huffmanCodeLengths(
      std::span<const short>(lengthCodeLengths.begin(), kMaxLengthCodeCount));

  std::array<short, kMaxLiteralCount + kMaxDistanceCodeCount>
      literalCodeLengths{};

  int codeLengthCount = literalCodeCount + distanceCodeCount;
  short prevCode = -1;
  for (int i = 0; i < codeLengthCount; i++) {
    short alphabetSymbol = huffmanCodeLengths.lookup(stream);
    if (0 <= alphabetSymbol && alphabetSymbol <= 15) {
      literalCodeLengths[i] = alphabetSymbol;
      prevCode = alphabetSymbol;
    } else if (alphabetSymbol == 16) {
      if (prevCode == -1) {
        throw std::runtime_error{"Corrupt zlib data"};
      }
      short repeatCount = 3 + static_cast<uint8_t>(stream.consumeBits(2));
      if (repeatCount + i > codeLengthCount) {
        throw std::runtime_error{"Corrupt zlib data"};
      }

      for (int j = 0; j < repeatCount; j++) {
        literalCodeLengths[i + j] = prevCode;
      }
      i += repeatCount - 1;
    } else {
      prevCode = 0;
      short repeatCount = alphabetSymbol == 17
          ? 3 + static_cast<uint8_t>(stream.consumeBits(3))
          : 11 + static_cast<uint8_t>(stream.consumeBits(7));

      if (repeatCount + i > codeLengthCount) {
        throw std::runtime_error{"Corrupt zlib data"};
      }

      for (int j = 0; j < repeatCount; j++) {
        literalCodeLengths[i + j] = 0;
      }
      i += repeatCount - 1;
    }
  }

  return {
      .literalHuffmanEncoding = HuffmanTree(
          std::span<const short>(literalCodeLengths.begin(), literalCodeCount)),
      .distanceHuffmanEncoding = HuffmanTree(std::span<const short>(
          literalCodeLengths.begin() + literalCodeCount, distanceCodeCount))};
}

void decodeBlock(
    const CompressedBlockTrees& trees,
    BitStream& stream,
    std::vector<unsigned char>& out) {
  while (true) {
    short symbol = trees.literalHuffmanEncoding.lookup(stream);
    if (symbol < 256) {
      out.emplace_back(static_cast<unsigned char>(symbol));
    } else if (symbol == 256) {
      break;
    } else if (symbol > 256) {
      ExtraBitCode lengthCode = kLengthCodes[symbol - 257];
      unsigned short length = lengthCode.offset +
          static_cast<unsigned short>(stream.consumeBits(lengthCode.extraBits));
      ExtraBitCode distanceCode =
          kDistanceCodes[trees.distanceHuffmanEncoding.lookup(stream)];
      unsigned short distance = distanceCode.offset +
          static_cast<unsigned short>(stream.consumeBits(
              distanceCode.extraBits));

      if (distance > out.size()) {
        throw std::runtime_error{"Corrupt zlib data"};
      }
      size_t readCursor = out.size() - distance;
      for (int i = 0; i < length; i++) {
        out.emplace_back(out[readCursor + i]);
      }
    }
  }
}

} // namespace

std::vector<unsigned char> zlibDecompress(std::span<const unsigned char> data) {
  if (data.size() < 2) {
    throw std::runtime_error{"Corrupt zlib data"};
  }
  readZlibHeader(data[0], data[1]);

  data = data.subspan(2);
  BitStream stream(data);

  std::vector<unsigned char> result;

  while (true) {
    BlockHeader blockHead = readBlockHeader(stream);

    switch (blockHead.type) {
      case BlockHeader::BlockType::NO_COMPRESSION:
        throw std::runtime_error{"Unsupported zlib format"};

      case BlockHeader::BlockType::FIXED_CODES:
        decodeBlock(fixedTrees, stream, result);
        break;

      case BlockHeader::BlockType::DYNAMIC_CODES:
        CompressedBlockTrees dcHeader = readDynamicCodesBlockHeader(stream);
        decodeBlock(dcHeader, stream, result);
        break;
    }

    if (blockHead.isFinal) {
      break;
    }
  }

  return result;
}

} // namespace util
