#ifndef IMPL_LIBRARY_OBSERVABLEIMPL_H
#define IMPL_LIBRARY_OBSERVABLEIMPL_H

#include "Observer.h"

namespace NSLibrary {

template<class TData, class TSendBy>
void CObservable<TData, TSendBy>::subscribe(CObserver* obs) {
  assert(obs);
  if (obs->isSubscribed())
    obs->unsubscribe();
  Observers_.push_back(obs);
  obs->_setConnection(makeConnection(obs));
  if (hasData())
    obs->onSubscribe(data_());
}

template<class TData, class TSendBy>
void CObservable<TData, TSendBy>::notify() {
  if (!hasData())
    return;
  for (auto obs : Observers_)
    obs->onNotify(data_());
}

template<class TData, class TSendBy>
void CObservable<TData, TSendBy>::CConnection::unsubscribe() {
  if (!isSubscribed())
    return;
  assert(Observer_);
  if (hasData())
    Observer_->onUnsubscribe(data_());
  assert(Observers_);
  Observers_->remove(Observer_);
  Observers_ = nullptr;
  Observer_ = nullptr;
}

template<class TData, class TSendBy>
void CObservable<TData, TSendBy>::unsubscribeAll() {
  while (!Observers_.empty()) {
    assert(Observers_.front());
    Observers_.front()->unsubscribe();
  }
}

void CObservable<void, void>::subscribe(CObserver* obs) {
  assert(obs);
  if (obs->isSubscribed())
    obs->unsubscribe();
  Observers_.push_back(obs);
  obs->_setConnection(makeConnection(obs));
  obs->onSubscribe();
}

void CObservable<void, void>::notify() {
  for (auto obs : Observers_)
    obs->onNotify();
}

void CObservable<void, void>::CConnection::unsubscribe() {
  if (!isSubscribed())
    return;
  assert(Observer_);
  Observer_->onUnsubscribe();
  assert(Observers_);
  Observers_->remove(Observer_);
  Observers_ = nullptr;
  Observer_ = nullptr;
}

void CObservable<void, void>::unsubscribeAll() {
  while (!Observers_.empty()) {
    assert(Observers_.front());
    Observers_.front()->unsubscribe();
  }
}
} // namespace NSLibrary
#endif // IMPL_LIBRARY_OBSERVABLEIMPL_H
