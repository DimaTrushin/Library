#ifndef OBSERVER_H
#define OBSERVER_H

#include "Impl/ObservableImpl.h"

namespace NSLibrary {
namespace NSObservableDataDetail {

template<class TData>
class CStorage {
  using CDataOptional = std::optional<TData>;
public:
  template<class... TArgs>
  CStorage(TArgs&& ... args) : Data_(std::forward<TArgs>(args)...) {}

protected:
  template<class... TArgs>
  void set(TArgs&& ... args) {
    Data_.emplace(std::forward<TArgs>(args)...);
  }

  CDataOptional Data_{};
};


template<class TData>
class CObservableData : protected CStorage<TData>, protected CObservable<TData> {
  using CStorage = CStorage<TData>;
  using CObservable = CObservable<TData>;
public:
  template<class... TArgs>
  CObservableData(TArgs&& ... args)
    : CStorage(std::forward<TArgs>(args)...),
      CObservable([&Data = CStorage::Data_]() -> typename CSource<TData>::CGetType {
    return Data;
  }) {}

  template<class... TArgs>
  void set(TArgs&& ... args) {
    CStorage::set(std::forward<TArgs>(args)...);
    CObservable::notify();
  }

  using CObservable::subscribe;
  using CObservable::notify;
};
} // NSObservableDataDetail

template<class TData>
using CObservableData = NSObservableDataDetail::CObservableData<TData>;


using CNotifier = CObservable<void>;


template<class TData>
class CObserverStrict : public CObserver<TData> {
  using CBase = CObserver<TData>;
public:
  template<class T1, class T2, class T3>
  CObserverStrict(T1&& OnSubscribe, T2&& OnNotify, T3&& OnUnsubscribe)
    : CBase(
        [OnSubscribe](typename CBase::CGetType optData) {
    if (optData.has_value())
      OnSubscribe(*optData);
  },
  [OnNotify](typename CBase::CGetType optData) {
    if (optData.has_value())
      OnNotify(*optData);
  },
  [OnUnsubscribe](typename CBase::CGetType optData) {
    if (optData.has_value())
      OnUnsubscribe(*optData);
  }) {}
};


template<>
class CObserverStrict<void> : public CObserver<void> {
  using CBase = CObserver<void>;
public:
  using CBase::CBase;
};


template<class TData>
class CObserverHot : public CObserver<TData> {
  using CBase = CObserver<TData>;
public:
  template<class T1, class T2>
  CObserverHot(T1&& OnSubscribe, T2&& OnNotify)
    : CBase(std::forward<T1>(OnSubscribe),
            std::forward<T2>(OnNotify),
            CBase::doNothing) {}
};

template<class TData>
class CObserverCold : public CObserver<TData> {
  using CBase = CObserver<TData>;
public:
  template<class T>
  CObserverCold(T&& OnNotify)
    : CBase(CBase::doNothing,
            std::forward<T>(OnNotify),
            CBase::doNothing) {}
};


template<class TData>
class CObserverHotStrict : public CObserverHot<TData> {
  using CBase = CObserverHot<TData>;
public:
  template<class T1, class T2>
  CObserverHotStrict(T1&& OnSubscribe, T2&& OnNotify)
    : CBase(
        [OnSubscribe](typename CBase::CGetType optData) {
    if (optData.has_value())
      OnSubscribe(*optData);
  },
  [OnNotify](typename CBase::CGetType optData) {
    if (optData.has_value())
      OnNotify(*optData);
  }) {}
};


template<class TData>
class CHotInput : public CObserverHotStrict<TData> {
  using CBase = CObserverHotStrict<TData>;
public:
  template<class T>
  CHotInput(T Action) : CBase(Action, Action) {}
};


template<class TData>
class CObserverColdStrict : public CObserverCold<TData> {
  using CBase = CObserverCold<TData>;
public:
  template<class T>
  CObserverColdStrict(T&& OnNotify)
    : CBase(
        [OnNotify](typename CBase::CGetType optData) {
    if (optData.has_value())
      OnNotify(*optData);
  }) {}
};


template<class TData>
using CColdInput = CObserverColdStrict<TData>;


template<class TData>
class CHotActiveInput : public CObserver<TData> {
  using CBase = CObserver<TData>;
public:
  template<class T>
  CHotActiveInput(T Action)
    : CBase(
        [Action, this](typename CBase::CGetType optData) {
    if (optData.has_value()) {
      activate();
      Action(*optData);
    }
  },
  [Action, this](typename CBase::CGetType optData) {
    if (optData.has_value()) {
      activate();
      Action(*optData);
    }
  },
  [this](typename CBase::CGetType) {
    deactivate();
  }) {}

  bool isActive() const {
    assert(!isActive_ || CBase::hasValue());
    return isActive_;
  }

  void deactivate() {
    isActive_ = false;
  }

private:
  void activate() {
    isActive_ = true;
  }

  bool isActive_ = false;
};


template<class TData>
class CColdActiveInput : public CObserver<TData> {
  using CBase = CObserver<TData>;
public:
  template<class T>
  CColdActiveInput(T Action)
    : CBase(
        CObserver<TData>::doNothing,
        [Action, this](typename CBase::CGetType optData) {
    if (optData.has_value()) {
      activate();
      Action(*optData);
    }
  },
  [this](typename CBase::CGetType) {
    deactivate();
  }) {}

  bool isActive() const {
    assert(!isActive_ || CBase::hasValue());
    return isActive_;
  }

  void deactivate() {
    isActive_ = false;
  }

private:
  void activate() {
    isActive_ = true;
    assert(CBase::hasValue());
  }

  bool isActive_ = false;
};

} // NSLibrary

#endif // OBSERVER_H
