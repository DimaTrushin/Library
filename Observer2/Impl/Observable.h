#ifndef IMPL_LIBRARY_OBSERVABLE_H
#define IMPL_LIBRARY_OBSERVABLE_H

#include "Source.h"

#include <list>

namespace NSLibrary {

template<class TData, class TSendBy>
class CObserver;

template<class TData, class TSendBy = AutoSendBy<TData>>
class CObservable {
public:
  using CData = TData;
  using CSendBy = TSendBy;

  using CObserver = CObserver<CData, CSendBy>;
  using CObserversContainer = std::list<CObserver*>;

  using CSource = CSource<CData, CSendBy>;
  using CSendWrapper = typename CSource::CSendWrapper;

  using CDataType = typename CSource::CDataType;
  using CGetType = typename CSource::CGetType;

  template<class TSource>
  explicit CObservable(TSource&& Source)
      : Source_(std::forward<TSource>(Source)) {
  }
  CObservable(const CObservable&) = delete;
  CObservable(CObservable&&) noexcept = delete;
  CObservable& operator=(const CObservable&) = delete;
  CObservable& operator=(CObservable&&) noexcept = delete;
  ~CObservable() {
    unsubscribeAll();
  }

  bool hasData() const {
    return Source_.hasData();
  }

  CDataType data_() const {
    return Source_();
  }

  void subscribe(CObserver* obs);
  void notify();

  class CConnection {
  public:
    CConnection() = default;
    CConnection(CObserver* Observer, CSource* Source,
                CObserversContainer* Observers)
        : Observer_(Observer), Source_(Source), Observers_(Observers) {
      assert(Observer_);
      assert(Source_);
      assert(Observers_);
    }

    bool isSubscribed() const {
      return Observer_ != nullptr;
    }

    void unsubscribe();

    bool hasData() const {
      return isSubscribed() && Source_->hasData();
    }

    CDataType data_() const {
      return (*Source_)();
    }

  private:
    CObserver* Observer_ = nullptr;
    CSource* Source_ = nullptr;
    CObserversContainer* Observers_ = nullptr;
  };

private:
  CConnection makeConnection(CObserver* obs) {
    return CConnection(obs, &Source_, &Observers_);
  }

  void unsubscribeAll();

  CSource Source_;
  CObserversContainer Observers_;
};

template<>
class CObservable<void, void> {
public:
  using CData = void;
  using CSendBy = void;

  using CObserver = CObserver<CData, CSendBy>;
  using CObserversContainer = std::list<CObserver*>;

  CObservable() = default;
  CObservable(const CObservable&) = delete;
  CObservable(CObservable&&) noexcept = delete;
  CObservable& operator=(const CObservable&) = delete;
  CObservable& operator=(CObservable&&) noexcept = delete;
  ~CObservable() {
    unsubscribeAll();
  }

  void subscribe(CObserver* obs);
  void notify();

  class CConnection {
  public:
    CConnection() = default;
    CConnection(CObserver* Observer, CObserversContainer* Observers)
        : Observer_(Observer), Observers_(Observers) {
      assert(Observer_);
      assert(Observers_);
    }

    bool isSubscribed() const {
      return Observer_ != nullptr;
    }

    void unsubscribe();

  private:
    CObserver* Observer_ = nullptr;
    CObserversContainer* Observers_ = nullptr;
  };

private:
  CConnection makeConnection(CObserver* obs) {
    return CConnection(obs, &Observers_);
  }

  void unsubscribeAll();

  CObserversContainer Observers_;
};
} // namespace NSLibrary
#endif // IMPL_LIBRARY_OBSERVABLE_H
