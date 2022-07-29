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
class CObservableMono : protected CObservable<TData, TSendBy> {
  using CBase = CObservable<TData, TSendBy>;

public:
  using CData = TData;
  using CSendBy = TSendBy;

  using CObserver = typename CBase::CObserver;

  using CSource = typename CBase::CSource;
  using CSendWrapper = typename CSource::CSendWrapper;

  using CDataType = typename CSource::CDataType;
  using CGetType = typename CSource::CGetType;

  using CBase::CBase;

  using CBase::data_;
  using CBase::hasData;
  using CBase::notify;

  void subscribe(typename CBase::CObserver* obs) {
    CBase::unsubscribeAll();
    CBase::subscribe(obs);
  }
};

namespace NSCObservalbeDataDetail {

template<class TData, class TSendBy,
         template<class T1, class T2> class TObservable>
class CObservableDataImpl : protected NSCObservalbeDataDetail::CStorage<TData>,
                            public TObservable<TData, TSendBy> {
  using CStorageBase = NSCObservalbeDataDetail::CStorage<TData>;
  using CObservableBase = TObservable<TData, TSendBy>;

public:
  template<class... TArgs>
  explicit CObservableDataImpl(TArgs&&... args)
      : CStorageBase(std::forward<TArgs>(args)...),
        CObservableBase([&Data = CStorageBase::Data_]() ->
                        typename CObservableBase::CGetType { return Data; }) {
  }

  template<class... TArgs>
  void set(TArgs&&... args) {
    CStorageBase::set(std::forward<TArgs>(args)...);
    CObservableBase::notify();
  }
};
} // namespace NSCObservalbeDataDetail

template<class TData, class TSendBy = AutoSendBy<TData>>
using CObservableData =
    NSCObservalbeDataDetail::CObservableDataImpl<TData, TSendBy, CObservable>;

template<class TData, class TSendBy = AutoSendBy<TData>>
using CObservableDataMono =
    NSCObservalbeDataDetail::CObservableDataImpl<TData, TSendBy,
                                                 CObservableMono>;

template<class TData, class TSendBy = AutoSendBy<TData>>
class CInput : public CObserver<TData, TSendBy> {
  using CBase = CObserver<TData, TSendBy>;

public:
  template<class T1, class T2>
  explicit CInput(T1&& OnSubscribe, T2&& OnNotify)
      : CBase(std::forward<T1>(OnSubscribe), std::forward<T2>(OnNotify),
              CBase::doNothing) {
  }
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
