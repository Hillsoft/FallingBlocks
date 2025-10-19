#pragma once

#include <memory>
#include <type_traits>
#include "util/debug.hpp"

namespace util {

template <typename T>
struct NotNullPointerTraits {
  static constexpr bool isValid = false;
};

template <typename T>
struct NotNullPointerTraits<T*> {
  static constexpr bool isValid = true;
  static constexpr bool enableImplicitConstructFromReference = true;

  using TTarget = T;
};

template <typename T>
struct NotNullPointerTraits<std::unique_ptr<T>> {
  static constexpr bool isValid = true;
  static constexpr bool enableImplicitConstructFromReference = false;

  using TTarget = T;
};

template <typename T>
struct NotNullPointerTraits<std::shared_ptr<T>> {
  static constexpr bool isValid = true;
  static constexpr bool enableImplicitConstructFromReference = false;

  using TTarget = T;
};

template <typename T>
struct NotNullPointerTraits<std::weak_ptr<T>> {
  static constexpr bool isValid = true;
  static constexpr bool enableImplicitConstructFromReference = false;

  using TTarget = T;
};

template <typename TPtr>
  requires(NotNullPointerTraits<TPtr>::isValid)
class NotNullBase {
 public:
  using TTarget = NotNullPointerTraits<TPtr>::TTarget;

  NotNullBase(std::nullptr_t) = delete;
  explicit NotNullBase(TPtr val) : val_(std::move(val)) {
    DEBUG_ASSERT(val_ != nullptr);
  }
  /* implicit */ NotNullBase(TTarget& val)
    requires(NotNullPointerTraits<TPtr>::enableImplicitConstructFromReference)
      : val_(&val) {}

  template <typename UPtr>
  operator NotNullBase<UPtr>() && {
    return NotNullBase<UPtr>(std::move(val_));
  }

  template <typename UPtr>
  operator NotNullBase<UPtr>() const& {
    return NotNullBase<UPtr>(val_);
  }

  TTarget& operator*() const { return *val_; }
  TTarget* operator->() const { return &*val_; }

  auto operator<=>(const NotNullBase& other) const {
    return val_ <=> other.val_;
  }
  auto operator<=>(const TPtr& other) const { return val_ <=> other; }
  bool operator==(std::nullptr_t) const { return false; }
  bool operator!=(std::nullptr_t) const { return true; }

 private:
  TPtr val_;
};

template <typename T>
using NotNullPtr = NotNullBase<T*>;

template <typename T>
using NotNullUniquePtr = NotNullBase<std::unique_ptr<T>>;

template <typename T>
using NotNullSharedPtr = NotNullBase<std::shared_ptr<T>>;

template <typename T>
using NotNullWeakPtr = NotNullBase<std::weak_ptr<T>>;

template <typename T, typename... Args>
NotNullUniquePtr<T> makeNotNullUnique(Args&&... args) {
  return NotNullUniquePtr<T>{std::make_unique<T>(std::forward<Args>(args)...)};
}

template <typename T, typename... Args>
NotNullSharedPtr<T> makeNotNullShared(Args&&... args) {
  return NotNullSharedPtr<T>{std::make_shared<T>(std::forward<Args>(args)...)};
}

} // namespace util
