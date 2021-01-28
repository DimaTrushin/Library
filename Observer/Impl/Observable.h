#ifndef IMPL_OBSERVABLE_H
#define IMPL_OBSERVABLE_H

#include "Source.h"

#include <list>

namespace NSLibrary {

namespace NSObserverDetail {
template<class TData>
class CObserver;
}

template<class TData>
using CObserver = NSObserverDetail::CObserver<TData>;

namespace NSObservableDetail {

template<class TData>
class CObservableBase {
public:

  using CData = TData;
  using CObserverContainer = std::list<CObserver<CData>*>;

  CObservableBase() = default;
  CObservableBase(const CObservableBase&) = delete;
  CObservableBase(CObservableBase&&) noexcept = delete;
  CObservableBase& operator=(const CObservableBase&) = delete;
  CObservableBase& operator=(CObservableBase&&) noexcept = delete;

  ~CObservableBase() {
    unsubscribeAll();
  }

  void notify() {
    for (CObserver<CData>* Observer : Observers_)
      notifyOne(Observer);
  }

protected:
  class CUnsubscriber {
  public:
    CUnsubscriber() = default;
    CUnsubscriber(CObserver<TData>* Observer, CObserverContainer* Observers)
      : Observer_(Observer), Observers_(Observers) {
      assert(Observer_);
      assert(Observers_);
    }

    void unsubscribe();

    bool isSubscribed() const {
      return Observers_ != nullptr;
    }

  private:
    CObserver<TData>* Observer_ = nullptr;
    CObserverContainer* Observers_ = nullptr;
  };

  CUnsubscriber makeUnsubscriber(CObserver<TData>* Observer) {
    assert(Observer);
    return CUnsubscriber(Observer, &Observers_);
  }

  CObserverContainer* observers() {
    return &Observers_;
  }

private:
  void notifyOne(CObserver<CData>* Observer);
  void unsubscribeAll();

  CObserverContainer Observers_;
};


template<class TData, class TBase>
class CConnectorImpl : public TBase {
  using CBase = TBase;
  using CBase::CBase;
  using CGetAction = typename CSource<TData>::CGetAction;
public:
  class CConnection {
  public:
    using CUnsubscriber = typename CBase::CUnsubscriber;
    using CGetType = typename CSource<TData>::CGetType;
    using CGetAction = typename CSource<TData>::CGetAction;

    CConnection() = default;
    CConnection(CUnsubscriber Unsubscriber, CSource<TData>* Source)
      : Unsubscriber_(std::move(Unsubscriber)),
        Source_(Source) {
      assert(Unsubscriber_.isSubscribed());
      assert(Source_);
    }

    bool isSubscribed() const {
      return Unsubscriber_.isSubscribed();
    }

    void unsubscribe() {
      if (!isSubscribed())
        return;
      Unsubscriber_.unsubscribe();
      Source_ = nullptr;
    }

    CGetAction Getter() const {
      if (Source_ == nullptr)
        return CSource<TData>::getNothing;
      assert(Source_);
      return Source_->Getter();
    }

    CGetType get() const {
      if (Source_ == nullptr)
        return CSource<TData>::getNothing();
      assert(Source_);
      return Source_->get();
    }

    bool hasValue() const {
      if (Source_ == nullptr)
        return false;
      assert(Source_);
      return Source_->hasValue();
    }

  private:
    CUnsubscriber Unsubscriber_{};
    CSource<TData>* Source_ = nullptr;
  };

  CConnectorImpl() = default;
  CConnectorImpl(CGetAction Action) : Source_(std::move(Action)) {
    assert(Source_.hasGetter());
  }

  void setSource(CGetAction Action) {
    assert(Action);
    Source_.set(std::move(Action));
    CBase::notify();
  }

protected:
  CConnection makeConnection(CObserver<TData>* Observer) {
    assert(Observer);
    return CConnection(CBase::makeUnsubscriber(Observer), &Source_);
  }

private:
  CSource<TData> Source_{};
};


template<class TBase>
class CConnectorImpl<void, TBase> : public TBase {
  using CBase = TBase;
  using CBase::CBase;
public:
  class CConnection {
  public:
    using CUnsubscriber = typename CBase::CUnsubscriber;
    using CGetType = void;
    using CGetAction = void;

    CConnection() = default;
    CConnection(CUnsubscriber Unsubscriber)
      : Unsubscriber_(Unsubscriber) {
      assert(Unsubscriber_.isSubscribed());
    }

    bool isSubscribed() const {
      return Unsubscriber_.isSubscribed();
    }

    void unsubscribe() {
      if (!isSubscribed())
        return;
      Unsubscriber_.unsubscribe();
    }

  private:
    CUnsubscriber Unsubscriber_{};
  };

protected:
  CConnection makeConnection(CObserver<void>* Observer) {
    assert(Observer);
    return CConnection(CBase::makeUnsubscriber(Observer));
  }
};


template<class TBase>
using CConnector = CConnectorImpl<typename TBase::CData, TBase>;


template<class TData, class TBase>
class CSubscriberImpl : public TBase {
  using CBase = TBase;
  using CBase::CBase;
  using CData = typename CBase::CData;
public:
  void subscribe(CObserver<CData>* Observer);
};


template<class TBase>
using CObservableSubscriber = CSubscriberImpl<typename TBase::CData, TBase>;


template<class TData>
using CObservable = CObservableSubscriber<CConnector<CObservableBase<TData>>>;

} // NSObservableDetail

template<class TData>
using CObservable = NSObservableDetail::CObservable<TData>;

} // NSLibrary

#endif // IMPL_OBSERVABLE_H
