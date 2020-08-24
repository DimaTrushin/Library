#ifndef IMPL_OBSERVABLEIMPL_H
#define IMPL_OBSERVABLEIMPL_H

#include "Observable.h"
#include "Observer.h"

namespace NSLibrary {

namespace NSObservableDetail {
template<class TData>
void CObservableBase<TData>::notify(CObserver<CData>* Observer) {
  assert(Observer);
  Observer->onNotify();
}

template<class TData>
void CObservableBase<TData>::unsubscribeAll() {
  while (!Observers_.empty()) {
    assert(Observers_.front());
    Observers_.front()->unsubscribe();
  }
}

template<class TData>
void CObservableBase<TData>::CUnsubscriber::unsubscribe() {
  if (!isSubscribed())
    return;
  assert(Observer_);
  Observer_->onUnsubscribe();
  assert(Observers_);
  Observers_->remove(Observer_);
  Observers_ = nullptr;
  Observer_ = nullptr;
}


template<class TData, class TBase>
void CSubscriberImpl<TData, TBase>::subscribe(CObserver<CData>* Observer) {
  assert(Observer);
  if (Observer->isSubscribed())
    Observer->unsubscribe();
  observers()->push_back(Observer);
  Observer->_setConnection(makeConnection(Observer));
  Observer->onSubscribe();
}

} // NSObservableDetail
} // NSLibrary

#endif // IMPL_OBSERVABLEIMPL_H
