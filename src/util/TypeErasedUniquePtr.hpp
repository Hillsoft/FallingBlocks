#pragma once

#include <memory>
#include <typeindex>
#include <utility>

namespace util {

class TypeErasedUniquePtr {
 public:
  TypeErasedUniquePtr()
      : ptr_(nullptr), typeIndex_(typeid(void)), deleter_(nullptr) {}

  template <typename T>
  explicit TypeErasedUniquePtr(std::unique_ptr<T> ptr)
      : ptr_(ptr.release()), typeIndex_(typeid(T)), deleter_([](void* ptr) {
          delete reinterpret_cast<T*>(ptr);
        }) {}

  TypeErasedUniquePtr(const TypeErasedUniquePtr& other) = delete;
  TypeErasedUniquePtr& operator=(const TypeErasedUniquePtr& other) = delete;

  TypeErasedUniquePtr(TypeErasedUniquePtr&& other)
      : ptr_(other.ptr_),
        typeIndex_(other.typeIndex_),
        deleter_(other.deleter_) {
    other.ptr_ = nullptr;
    other.typeIndex_ = typeid(void);
    other.deleter_ = nullptr;
  }
  TypeErasedUniquePtr& operator=(TypeErasedUniquePtr&& other) noexcept {
    std::swap(ptr_, other.ptr_);
    std::swap(typeIndex_, other.typeIndex_);
    std::swap(deleter_, other.deleter_);
    return *this;
  }

  ~TypeErasedUniquePtr() {
    if (ptr_ != nullptr) {
      deleter_(ptr_);
    }
  }

  template <typename T>
  bool holdsType() const {
    return typeid(T) == typeIndex_;
  }

  template <typename T>
  T* get() const {
    DEBUG_ASSERT(holdsType<T>());
    return reinterpret_cast<T*>(ptr_);
  }

 private:
  void* ptr_;
  std::type_index typeIndex_;
  void (*deleter_)(void*);
};

} // namespace util
