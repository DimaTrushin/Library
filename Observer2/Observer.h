#ifndef LIBRARY_OBSERVER_H
#define LIBRARY_OBSERVER_H

#include "Impl/ObservableImpl.h"

namespace NSLibrary {

namespace NSCObservalbeDataDetail {

template<class TData>
class CStorage {
  using CDataOptional = std::optional<TData>;

public:
  template<class... TArgs>
  explicit CStorage(TArgs&&... args) : Data_(std::forward<TArgs>(args)...) {
  }

protected:
  template<class... TArgs>
  void set(TArgs&&... args) {
    Data_.emplace(std::forward<TArgs>(args)...);
  }

  CDataOptional Data_{};
};
} // namespace NSCObservalbeDataDetail

template<class TData, class TSendBy = AutoSendBy<TData>>
class CObservableData : protected NSCObservalbeDataDetail::CStorage<TData>,
                        protected CObservable<TData, TSendBy> {
  using CStorageBase = NSCObservalbeDataDetail::CStorage<TData>;
  using CObservableBase = CObservable<TData, TSendBy>;

public:
  using CData = TData;
  using CSendBy = TSendBy;

  using CObserver = typename CObservableBase::CObserver;

  using CSource = typename CObservableBase::CSource;
  using CSendWrapper = typename CSource::CSendWrapper;

  using CDataType = typename CSource::CDataType;
  using CGetType = typename CSource::CGetType;

  template<class... TArgs>
  explicit CObservableData(TArgs&&... args)
      : CStorageBase(std::forward<TArgs>(args)...),
        CObservableBase(
            [&Data = CStorageBase::Data_]() -> CGetType { return Data; }) {
  }

  template<class... TArgs>
  void set(TArgs&&... args) {
    CStorageBase::set(std::forward<TArgs>(args)...);
    CObservableBase::notify();
  }

  using CObservableBase::data_;
  using CObservableBase::hasData;
  using CObservableBase::notify;
  using CObservableBase::subscribe;
};

template<class TData, class TSendBy = AutoSendBy<TData>>
class CHotInput : public CObserver<TData, TSendBy> {
  using CBase = CObserver<TData, TSendBy>;

public:
  template<class T>
  explicit CHotInput(const T& Action)
      : CBase(Action, Action, CBase::doNothing) {
  }
};

template<class TData, class TSendBy = AutoSendBy<TData>>
class CColdInput : public CObserver<TData, TSendBy> {
  using CBase = CObserver<TData, TSendBy>;

public:
  template<class T>
  explicit CColdInput(T&& Action)
      : CBase(CBase::doNothing, std::forward<T>(Action), CBase::doNothing) {
  }
};

} // namespace NSLibrary

#endif // LIBRARY_OBSERVER_H
