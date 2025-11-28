#pragma once

#include <memory>
#include <type_traits>
#include <vector>

namespace util {

class BlockForwardAllocatedArena {
 public:
  BlockForwardAllocatedArena();

  void* allocateRaw(size_t count, size_t align);
  void reset();

  template <typename T, typename... TArgs>
    requires std::is_trivially_destructible_v<T> &&
      std::is_constructible_v<T, TArgs&&...>
  T* allocate(TArgs&&... args) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    return new (allocateRaw(sizeof(T), alignof(T)))
        T(std::forward<TArgs>(args)...);
  }

 private:
  void* allocateInCurrentBlock(size_t count, size_t align);

  // NOLINTNEXTLINE(*-avoid-c-arrays)
  std::vector<std::unique_ptr<char[]>> data_;
  size_t curBlock_ = 0;
  size_t blockOffset_ = 0;
};

} // namespace util
