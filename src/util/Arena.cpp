#include "util/Arena.hpp"

#include <stdexcept>

namespace util {

void* HeapArena::allocate(size_t count, size_t align) {
  size_t curAddress = reinterpret_cast<size_t>(data_ + cursor_);
  size_t alignOffset = align - (curAddress % align);
  alignOffset %= align; // make offset 0 if we're already aligned

  cursor_ += alignOffset;
  void* result = data_ + cursor_;
  cursor_ += count;
  if (cursor_ + count >= size_) {
    throw std::runtime_error{"Overflowed arena memory"};
  }
  return result;
}

void HeapArena::free(void* address) {}

void HeapArena::reset() {
  cursor_ = 0;
}

} // namespace util
