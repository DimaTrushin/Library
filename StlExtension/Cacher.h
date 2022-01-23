#ifndef CACHER_H
#define CACHER_H

#include <functional>
#include <iostream>
#include <optional>
#include <unordered_map>

namespace NSLibrary {

template<class TKey, class TData, class THash = std::hash<TKey>>
class CCacher {

  template<class TKey1, class TData1>
  struct CCacherNode {
    std::pair<const TKey1, CCacherNode>* Next;
    TData1 Data;
  };

  using CContainer = std::unordered_map<TKey, CCacherNode<TKey, TData>, THash>;
  using iterator = typename CContainer::iterator;
  using const_iterator = typename CContainer::const_iterator;
  using CLink = std::pair<const TKey, CCacherNode<TKey, TData>>*;
  using CFindType = std::optional<std::reference_wrapper<const TData>>;

  static constexpr const size_t kDefaultBucketCount = 64;

public:
  CCacher(size_t Size = kDefaultBucketCount) : MaxSize_(Size), Data_(Size) {
    Data_.max_load_factor(1.);
  }

  const TData* insert(const TKey key, TData&& data) {
    if (Data_.empty()) {
      auto pair = Data_.emplace(
          key, CCacherNode<TKey, TData>{nullptr, std::move(data)});
      First_ = &*pair.first;
      Last_ = First_;
      return &pair.first->second.Data;
    } else {
      if (size() == max_size())
        delete_last();
      auto pair = Data_.emplace(
          key, CCacherNode<TKey, TData>{nullptr, std::move(data)});
      if (pair.second) {
        First_->second.Next = &*pair.first;
        First_ = First_->second.Next;
        return &pair.first->second.Data;
      } else {
        return nullptr;
      }
    }
  }

  CFindType find(TKey key) const {
    auto iter = Data_.find(key);
    if (iter == Data_.end())
      return std::nullopt;
    return std::cref(iter->second.Data);
  }

  size_t size() const {
    return Data_.size();
  }

  size_t max_size() const {
    return MaxSize_;
  }

private:
  void delete_last() {
    if (Last_ != nullptr) {
      TKey last_key = Last_->first;
      Last_ = Last_->second.Next;
      Data_.erase(last_key);
      if (Data_.size() == 0)
        First_ = nullptr;
    }
  }

  CLink First_ = nullptr;
  CLink Last_ = nullptr;
  size_t MaxSize_ = kDefaultBucketCount;
  CContainer Data_;
};
} // namespace NSLibrary
#endif // CACHER_H
