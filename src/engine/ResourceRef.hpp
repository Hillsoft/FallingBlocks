#pragma once

namespace blocks::engine {

template <typename T>
class ResourceRef {
 public:
  explicit ResourceRef(T* ptr) : ref_(ptr) {}

  T& operator*() { return *ref_; }
  T* operator->() { return ref_; }

 private:
  T* ref_;
};

} // namespace blocks::engine
