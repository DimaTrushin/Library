#pragma once
#include <utility>

namespace NSLibrary {
namespace detail {
namespace NSAlias {

template<class T, class Tag>
struct Enum {
  enum type : T;
};
} // namespace NSAlias
} // namespace detail

template<class T, class Tag>
using AliasE = typename detail::NSAlias::Enum<T, Tag>::type;

template<class T, class Tag>
struct Alias {
  static_assert(std::is_scalar_v<T>, "The type of Alias MUST be scalar type!");

  explicit Alias(T value) : value_(value) {
  }
  operator T() const {
    return value_;
  }

private:
  T value_;
};
} // namespace NSLibrary
