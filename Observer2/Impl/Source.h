#ifndef IMPL_LIBRARY_SOURCE_H
#define IMPL_LIBRARY_SOURCE_H

#include "TypeHelper.h"

#include <cassert>
#include <functional>
#include <optional>

namespace NSLibrary {

struct CByValue;
struct CByReference;

namespace NSSourceDetail {

template<class TData, class TSendBy>
struct CSendWrapperImpl;

template<class TData>
struct CSendWrapperImpl<TData, CByValue> {
  using CType = TData;
};

template<class TData>
struct CSendWrapperImpl<TData, CByReference> {
  using CType = std::reference_wrapper<std::add_const_t<TData>>;
};

template<class TData, class TSendBy>
struct CDataTypeImpl;

template<class TData>
struct CDataTypeImpl<TData, CByValue> {
  using CType = TData;
};

template<class TData>
struct CDataTypeImpl<TData, CByReference> {
  using CType = const TData&;
};

template<bool TFlag>
struct AutoSendByImpl;

template<>
struct AutoSendByImpl<true> {
  using CType = CByValue;
};

template<>
struct AutoSendByImpl<false> {
  using CType = CByReference;
};

} // namespace NSSourceDetail

template<class TData, class TSendBy>
using CSendWrapper =
    typename NSSourceDetail::CSendWrapperImpl<TData, TSendBy>::CType;

template<class TData, class TSendBy>
using CDataType = typename NSSourceDetail::CDataTypeImpl<TData, TSendBy>::CType;

template<class TData>
using AutoSendBy = std::conditional_t<
    std::is_same_v<TData, void>, void,
    typename NSSourceDetail::AutoSendByImpl<isSimpleClass<TData>>::CType>;

template<class TData, class TSendBy = AutoSendBy<TData>>
class CSource {
public:
  using CData = TData;
  using CSendBy = TSendBy;
  using CSendWrapper = CSendWrapper<CData, CSendBy>;

  using CDataType = CDataType<CData, CSendBy>;
  using CGetType = std::optional<CSendWrapper>;
  using CSignature = CGetType();
  using CGetAction = std::function<CSignature>;

  CSource() = default;
  template<class TGetter>
  explicit CSource(TGetter&& Getter) : Getter_(std::forward<TGetter>(Getter)) {
    assert(hasGetter());
  }

  CSource(const CSource&) = default;
  CSource(CSource&& other) noexcept : Getter_(std::move(other.Getter_)) {
    assert(hasGetter());
    other.Getter_ = getNothing;
  }
  CSource& operator=(const CSource&) = default;
  CSource& operator=(CSource&& other) noexcept {
    Getter_ = std::move(other.Getter_);
    assert(hasGetter());
    other.Getter_ = getNothing;
    return *this;
  }

  template<class TGetter>
  void set(TGetter&& Getter) {
    Getter_ = std::forward<TGetter>(Getter);
    assert(hasGetter());
  }

  bool hasData() const {
    assert(hasGetter());
    return Getter_().has_value();
  }

  CGetType get() const {
    assert(hasGetter());
    return Getter_();
  }

  CDataType operator()() const {
    return *get();
  }

  const CGetAction getter() const {
    assert(hasGetter());
    return Getter_;
  }

  static CGetType getNothing() {
    return std::nullopt;
  }

private:
  bool hasGetter() const {
    return static_cast<bool>(Getter_);
  }

  CGetAction Getter_ = getNothing;
};
} // namespace NSLibrary
#endif // IMPL_LIBRARY_SOURCE_H
