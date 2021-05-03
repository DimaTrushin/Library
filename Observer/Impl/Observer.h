#ifndef IMPL_OBSERVER_H
#define IMPL_OBSERVER_H

#include "Observable.h"

#include <functional>

namespace NSLibrary {
namespace NSObserverDetail {

template<class TData>
class CObserverBase {
public:
  using CConnection = typename CObservable<TData>::CConnection;
  using CGetType = typename CConnection::CGetType;
  using CGetAction = typename CConnection::CGetAction;
  using CData = TData;

  CObserverBase() = default;
  CObserverBase(const CObserverBase&) = delete;
  CObserverBase(CObserverBase&&) noexcept = delete;
  CObserverBase& operator=(const CObserverBase&) = delete;
  CObserverBase& operator=(CObserverBase&&) noexcept = delete;

  bool isSubscribed() const {
    return Connection_.isSubscribed();
  }

  void _setConnection(CConnection Connection) {
    assert(Connection.isSubscribed());
    Connection_ = std::move(Connection);
  }

  bool hasValue() const {
    return Connection_.hasValue();
  }

  CGetType data() const {
    return Connection_.get();
  }

  CGetAction Getter() const {
    return Connection_.Getter();
  }

protected:
  CConnection& Connection() {
    return Connection_;
  }

private:
  CConnection Connection_{};
};

template<class TData, class TBase>
class CObserverReactorImpl : public TBase {
  using CArgType = typename TBase::CGetType;
  using CBase = TBase;

public:
  using CData = TData;
  using CMethod = std::function<void(CArgType)>;

  CObserverReactorImpl(CMethod OnSubscribe, CMethod OnNotify,
                       CMethod OnUnsubscribe)
      : OnSubscribe_(std::move(OnSubscribe)), OnNotify_(std::move(OnNotify)),
        OnUnsubscribe_(std::move(OnUnsubscribe)) {
    assert(OnSubscribe_);
    assert(OnNotify_);
    assert(OnUnsubscribe_);
  }

  void onSubscribe() {
    if (CBase::isSubscribed()) {
      assert(OnSubscribe_);
      OnSubscribe_(CBase::data());
    }
  }

  void onNotify() {
    if (CBase::isSubscribed()) {
      assert(OnNotify_);
      OnNotify_(CBase::data());
    }
  }

  void onUnsubscribe() {
    if (CBase::isSubscribed()) {
      assert(OnUnsubscribe_);
      OnUnsubscribe_(CBase::data());
    }
  }

  static void doNothing(CArgType) {
  }

protected:
  CMethod OnSubscribe_;
  CMethod OnNotify_;
  CMethod OnUnsubscribe_;
};

template<class TBase>
class CObserverReactorImpl<void, TBase> : public TBase {
  using CBase = TBase;

public:
  using CData = void;
  using CMethod = std::function<void(void)>;

  CObserverReactorImpl(CMethod OnSubscribe, CMethod OnNotify,
                       CMethod OnUnsubscribe)
      : OnSubscribe_(std::move(OnSubscribe)), OnNotify_(std::move(OnNotify)),
        OnUnsubscribe_(std::move(OnUnsubscribe)) {
    assert(OnSubscribe_);
    assert(OnNotify_);
    assert(OnUnsubscribe_);
  }

  void onSubscribe() {
    if (CBase::isSubscribed()) {
      assert(OnSubscribe_);
      OnSubscribe_();
    }
  }

  void onNotify() {
    if (CBase::isSubscribed()) {
      assert(OnNotify_);
      OnNotify_();
    }
  }

  void onUnsubscribe() {
    if (CBase::isSubscribed()) {
      assert(OnUnsubscribe_);
      OnUnsubscribe_();
    }
  }

  static void doNothing() {
  }

protected:
  CMethod OnSubscribe_;
  CMethod OnNotify_;
  CMethod OnUnsubscribe_;
};

template<class TBase>
using CObserverReactor = CObserverReactorImpl<typename TBase::CData, TBase>;

template<class TData>
class CObserver : public CObserverReactor<CObserverBase<TData>> {
  using CBase = CObserverReactor<CObserverBase<TData>>;

public:
  using CData = TData;
  using CMethod = typename CBase::CMethod;

  template<class T1, class T2, class T3>
  CObserver(T1&& OnSubscribe, T2&& OnNotify, T3&& OnUnsubscribe)
      : CBase(std::forward<T1>(OnSubscribe), std::forward<T2>(OnNotify),
              std::forward<T3>(OnUnsubscribe)) {
  }

  ~CObserver() {
    unsubscribe();
  }

  void unsubscribe() {
    CBase::Connection().unsubscribe();
  }

  void setSubscribe(CMethod OnSubscribe) {
    assert(OnSubscribe);
    CBase::OnSubscribe_ = std::move(OnSubscribe);
  }

  void setNotify(CMethod OnNotify) {
    assert(OnNotify);
    CBase::OnNotify_ = std::move(OnNotify);
  }

  void setUnsubscribe(CMethod OnUnsubscribe) {
    assert(OnUnsubscribe);
    CBase::OnUnsubscribe_ = std::move(OnUnsubscribe);
  }
};
} // namespace NSObserverDetail

template<class TData>
using CObserver = NSObserverDetail::CObserver<TData>;

} // namespace NSLibrary

#endif // IMPL_OBSERVER_H
