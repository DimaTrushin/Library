#ifndef IMPL_SOURCE_H
#define IMPL_SOURCE_H

#include "TypeHelper.h"

#include <cassert>
#include <functional>
#include <optional>

namespace NSLibrary {

template<class TData, class = NSType::EnableIfNotRef<TData>>
class CSource {
public:
  using CReturnValueType =
    std::conditional_t<
    NSType::isArithmetic<TData>,
    TData,
    NSType::ConstRefWrapp<TData>>;
  using CGetType = std::optional<CReturnValueType>;
  using CGetSignature = CGetType();
  using CGetAction = std::function<CGetSignature>;

  CSource() = default;
  CSource(CGetAction Action) : GetAction_(std::move(Action)) {
    assert(hasGetter());
  }

  bool hasGetter() const {
    return static_cast<bool>(GetAction_);
  }

  CGetType operator()() const {
    return get();
  }

  void set(CGetAction Action) {
    assert(Action);
    GetAction_ = std::move(Action);
  }

  CGetType get() const {
    if (!hasGetter())
      return getNothing();
    assert(hasGetter());
    return GetAction_();
  }

  CGetAction Getter() const {
    if (!hasGetter())
      return getNothing;
    assert(hasGetter());
    return GetAction_;
  }

  bool hasValue() const {
    if (!hasGetter())
      return false;
    assert(hasGetter());
    return GetAction_().has_value();
  }

  static CGetType getNothing() {
    return CGetType();
  }

private:
  CGetAction GetAction_ = getNothing;
};

} // NSLibrary

#endif // IMPL_SOURCE_H
