#pragma once

#include "Alias.h"
#include "ElementContainer.h"

namespace NSLibrary {
namespace Loop {

using IndexType = int64_t;
using From = AliasE<IndexType, struct FromSizeTTag>;
using To = AliasE<IndexType, struct ToSizeTTag>;
using Step = AliasE<IndexType, struct StepSizeTTag>;

namespace detail {

template<IndexType from, IndexType to, IndexType step, bool in_range>
struct MakeSequenceImpl {
  static_assert(in_range, "Internal error of MakeSequence!");
  static_assert(from < to, "Incorrect call to implementation of MakeSequence!");
  using type =
      EC::Prepend1<from, typename MakeSequenceImpl<from + step, to, step,
                                                   (step < to - from)>::type>;
};

template<IndexType from, IndexType to, IndexType step>
struct MakeSequenceImpl<from, to, step, false> {
  using type = EC::List<IndexType>;
};

template<IndexType from, IndexType to, IndexType step>
using MakeSequence =
    typename MakeSequenceImpl<from, to, step, (from < to)>::type;

template<class Seq>
struct ForImpl {};

template<class Type, template<class, Type...> class TContainer,
         IndexType... TElements>
struct ForImpl<TContainer<Type, TElements...>> {
  template<template<IndexType> class TF, class... TArgs>
  static void Do(TArgs&&... args) {
    (TF<TElements>()(std::forward<TArgs>(args)...), ...);
  }

  template<template<IndexType> class TF, class... TArgs>
  static auto Accumulate(TArgs&&... args) {
    using FType = TF<EC::ElementAt<EC::List<IndexType, TElements...>, 0>>;
    using ReturnType = decltype(FType()(std::forward<TArgs>(args)...));
    ReturnType value{};
    ((value = TF<TElements>()(value,
                              TF<TElements>()(std::forward<TArgs>(args)...))),
     ...);
    return value;
  }
};
} // namespace detail

template<From from, To to, Step step = Step{1}>
struct For : detail::ForImpl<detail::MakeSequence<from, to, step>> {};

} // namespace Loop
} // namespace NSLibrary
