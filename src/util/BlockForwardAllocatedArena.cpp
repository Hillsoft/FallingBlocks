#include "util/BlockForwardAllocatedArena.hpp"

#include <memory>
#include <stdexcept>

namespace util {

namespace {

constexpr size_t kBlockSize = 4 * 1024;

} // namespace

BlockForwardAllocatedArena::BlockForwardAllocatedArena()
    : curBlock_(0), blockOffset_(0) {
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

      if (curBlock_ == data_.size()) {
        data_.emplace_back(std::make_unique<char[]>(kBlockSize));
      }
    }
  }

  return result;
}

void* BlockForwardAllocatedArena::allocateInCurrentBlock(
    size_t count, size_t align) {
  char* result = &data_[curBlock_][blockOffset_];
  size_t alignOffset =
      ((reinterpret_cast<size_t>(result) + (align - 1)) / align) * align -
      reinterpret_cast<size_t>(result);

  if (blockOffset_ + count + alignOffset > kBlockSize) {
    return nullptr;
  }

  blockOffset_ += count + alignOffset;

  return result + alignOffset;
}

void BlockForwardAllocatedArena::reset() {
  curBlock_ = 0;
  blockOffset_ = 0;
}

} // namespace util
