#ifndef IMPL_LIBRARY_OBSERVER_H
#define IMPL_LIBRARY_OBSERVER_H

#include "Observable.h"

namespace NSLibrary {

template<class TData, class TSendBy = AutoSendBy<TData>>
class CObserver {
public:
  using CData = TData;
  using CSendBy = TSendBy;

  using CConnection = typename CObservable<CData, CSendBy>::CConnection;
  using CDataType = typename CObservable<CData, CSendBy>::CDataType;

  using CSignature = void(CDataType);
  using CAction = std::function<CSignature>;

  template<class T1, class T2, class T3>
  CObserver(T1&& OnSubscribe, T2&& OnNotify, T3&& OnUnsubscribe)
      : OnSubscribe_(std::forward<T1>(OnSubscribe)),
        OnNotify_(std::forward<T2>(OnNotify)),
        OnUnsubscribe_(std::forward<T3>(OnUnsubscribe)) {
  }

  CObserver(const CObserver&) = delete;
  CObserver(CObserver&&) noexcept = delete;
  CObserver& operator=(const CObserver&) = delete;
  CObserver& operator=(CObserver&&) noexcept = delete;
  ~CObserver() {
    unsubscribe();
  }

  bool isSubscribed() const {
    return Connection_.isSubscribed();
  }

  bool hasData() const {
    return Connection_.hasData();
  }

  CDataType data_() const {
    return Connection_.data_();
  }

  void onSubscribe(CDataType Data) {
    OnSubscribe_(Data);
  }

  void onNotify(CDataType Data) {
    OnNotify_(Data);
  }

  void onUnsubscribe(CDataType Data) {
    OnUnsubscribe_(Data);
  }

  void unsubscribe() {
    Connection_.unsubscribe();
  }

  static void doNothing(CDataType) {
  }

private:
  friend CObservable<TData, TSendBy>;

  void _setConnection(CConnection Connection) {
    assert(Connection.isSubscribed());
    Connection_ = std::move(Connection);
  }

  CConnection Connection_;
  CAction OnSubscribe_;
  CAction OnNotify_;
  CAction OnUnsubscribe_;
};

template<>
class CObserver<void, void> {
public:
  using CData = void;
  using CSendBy = void;

  using CConnection = typename CObservable<CData, CSendBy>::CConnection;

  using CSignature = void();
  using CAction = std::function<CSignature>;

  template<class T1, class T2, class T3>
  CObserver(T1&& OnSubscribe, T2&& OnNotify, T3&& OnUnsubscribe)
      : OnSubscribe_(std::forward<T1>(OnSubscribe)),
        OnNotify_(std::forward<T2>(OnNotify)),
        OnUnsubscribe_(std::forward<T3>(OnUnsubscribe)) {
  }

  CObserver(const CObserver&) = delete;
  CObserver(CObserver&&) noexcept = delete;
  CObserver& operator=(const CObserver&) = delete;
  CObserver& operator=(CObserver&&) noexcept = delete;
  ~CObserver() {
    unsubscribe();
  }

  bool isSubscribed() const {
    return Connection_.isSubscribed();
  }

  void onSubscribe() {
    OnSubscribe_();
  }

  void onNotify() {
    OnNotify_();
  }

  void onUnsubscribe() {
    OnUnsubscribe_();
  }

  void unsubscribe() {
    Connection_.unsubscribe();
  }

  static void doNothing() {
  }

private:
  friend CObservable<void, void>;

  void _setConnection(CConnection Connection) {
    assert(Connection.isSubscribed());
    Connection_ = std::move(Connection);
  }

  CConnection Connection_;
  CAction OnSubscribe_;
  CAction OnNotify_;
  CAction OnUnsubscribe_;
};
} // namespace NSLibrary
#endif // IMPL_LIBRARY_OBSERVER_H
