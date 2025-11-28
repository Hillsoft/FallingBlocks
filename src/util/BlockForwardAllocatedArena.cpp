#include "util/BlockForwardAllocatedArena.hpp"

#include <cstddef>
#include <memory>
#include <stdexcept>

namespace util {

namespace {

constexpr size_t kBlockSize = 4ull * 1024ull;

} // namespace

BlockForwardAllocatedArena::BlockForwardAllocatedArena() {
  // NOLINTNEXTLINE(*-avoid-c-arrays)
  data_.emplace_back(std::make_unique<char[]>(kBlockSize));
}

void* BlockForwardAllocatedArena::allocateRaw(size_t count, size_t align) {
  if (count > kBlockSize) {
    throw std::runtime_error{"Maximum block forward allocator size exceeded"};
  }

  void* result = nullptr;

  while (result = allocateInCurrentBlock(count, align), result == nullptr) {
    if (kBlockSize - blockOffset_ < count) {
      curBlock_++;
      blockOffset_ = 0;

      if (curBlock_ == data_.size()) {
        // NOLINTNEXTLINE(*-avoid-c-arrays)
        data_.emplace_back(std::make_unique<char[]>(kBlockSize));
      }
    }
  }

  return result;
}

void* BlockForwardAllocatedArena::allocateInCurrentBlock(
    size_t count, size_t align) {
  char* result = &data_[curBlock_][blockOffset_];
  const size_t alignOffset =
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      (((reinterpret_cast<size_t>(result) + (align - 1)) / align) * align) -
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<size_t>(result);

  if (blockOffset_ + count + alignOffset > kBlockSize) {
    return nullptr;
  }

  blockOffset_ += count + alignOffset;

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  return result + alignOffset;
}

void BlockForwardAllocatedArena::reset() {
  curBlock_ = 0;
  blockOffset_ = 0;
}

} // namespace util
