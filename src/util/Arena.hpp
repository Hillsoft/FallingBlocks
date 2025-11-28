#pragma once

#include <utility>

namespace util {

class HeapArena {
 public:
  explicit HeapArena(size_t size) : size_(size) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory,cppcoreguidelines-prefer-member-initializer)
    data_ = new char[size];
  }
  ~HeapArena() { delete[] data_; }

  HeapArena(const HeapArena& other) = delete;
  HeapArena& operator=(const HeapArena& other) = delete;

  HeapArena(HeapArena&& other) noexcept
      : data_(other.data_), cursor_(other.cursor_), size_(other.size_) {
    other.data_ = nullptr;
  }
  HeapArena& operator=(HeapArena&& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(cursor_, other.cursor_);
    return *this;
  }

  void* allocate(size_t count, size_t align);
  void free(void* address);
  void reset();

  template <typename T>
  class Allocator {
   public:
    using value_type = T;
    explicit Allocator(HeapArena& arena) : arena_(&arena) {}

    template <class U>
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    Allocator(const Allocator<U>& other) noexcept : arena_(other.arena_) {}

    template <class U>
    bool operator==(const Allocator<U>& other) const noexcept {
      return arena_ == other.arena_;
    }
    template <class U>
    bool operator!=(const Allocator<U>& other) const noexcept {
      return arena_ != other.arena_;
    }

    T* allocate(size_t n) const {
      return static_cast<T*>(arena_->allocate(n * sizeof(T), alignof(T)));
    }
    void deallocate(T* p, size_t /* size */) const { arena_->free(p); }

   private:
    HeapArena* arena_;

    template <typename U>
    friend class Allocator;
  };

 private:
  char* data_ = nullptr;
  size_t cursor_ = 0;
  size_t size_ = 0;

  template <typename T>
  friend class Allocator;
};

} // namespace util
