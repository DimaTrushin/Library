#ifndef MVCWRAPPERS_H
#define MVCWRAPPERS_H

#include <memory>

namespace NSLibrary {

template<class TImpl>
class CBasicWrapper {
public:
  using CImplementation = TImpl;
  CBasicWrapper() : Impl_(std::make_unique<TImpl>()) {
  }
  template<class... TArgs>
  CBasicWrapper(TArgs&&... args)
      : Impl_(std::make_unique<TImpl>(std::forward<TArgs>(args)...)) {
  }

  bool isDefined() const {
    return Impl_.operator bool();
  }

  const TImpl* operator->() const {
    return Impl_.get();
  }

  TImpl* operator->() {
    return Impl_.get();
  }

protected:
  std::unique_ptr<TImpl> Impl_;
};

template<class TImpl>
class CModelWrapper : public CBasicWrapper<TImpl> {
  using CBase = CBasicWrapper<TImpl>;

public:
  using CBase::CBase;
  TImpl* model() const {
    return CBase::Impl_.get();
  }
};

template<class TImpl>
class CViewWrapper : public CBasicWrapper<TImpl> {
  using CBase = CBasicWrapper<TImpl>;

public:
  using CBase::CBase;
};

template<class TImpl>
class CControllerWrapper : public CBasicWrapper<TImpl> {
  using CBase = CBasicWrapper<TImpl>;

public:
  CControllerWrapper() = delete;
  using CBase::CBase;
};

} // namespace NSLibrary
#endif // MVCWRAPPERS_H
