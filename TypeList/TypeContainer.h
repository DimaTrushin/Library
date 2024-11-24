#pragma once

#include "Logic.h"

#include <tuple>

namespace NSLibrary {

namespace TC {
//--------------------------------------------------------------------------------
// List
//--------------------------------------------------------------------------------

template<class... TTypes>
struct List {};

template<class T, T... TValues>
struct ListE {};

template<int... TValues>
struct ListInt : ListE<int, TValues...> {};
//--------------------------------------------------------------------------------
// IfElse
//--------------------------------------------------------------------------------

using Logic::IfElse;
//--------------------------------------------------------------------------------
// And, Or
//--------------------------------------------------------------------------------
using Logic::And;
using Logic::And_t;

using Logic::Or;
using Logic::Or_t;

using Logic::Not;
using Logic::Not_t;
//--------------------------------------------------------------------------------
// isTypeContainer
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class T>
struct isTypeContainerImpl : std::false_type {};

template<template<class...> class TContainer, class... TTypes>
struct isTypeContainerImpl<TContainer<TTypes...>> : std::true_type {};
} // namespace NSTypeContainerDetail

template<class T>
struct isTypeContainer_t : NSTypeContainerDetail::isTypeContainerImpl<T> {};

template<class T>
constexpr bool isTypeContainer =
    NSTypeContainerDetail::isTypeContainerImpl<T>::value;
//--------------------------------------------------------------------------------
// areSame2
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {
template<class T1, class T2>
struct areSame2Impl : std::false_type {};

template<template<class...> class TContainer, class... TTypes1,
         class... TTypes2>
struct areSame2Impl<TContainer<TTypes1...>, TContainer<TTypes2...>>
    : std::true_type {};

} // namespace NSTypeContainerDetail

template<class T1, class T2>
struct areSame2_t : NSTypeContainerDetail::areSame2Impl<T1, T2> {};

template<class T1, class T2>
constexpr bool areSame2 = areSame2_t<T1, T2>::value;
//--------------------------------------------------------------------------------
// areSame
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class... TTypes>
struct areSameImpl : std::false_type {};

template<class TContainer, class... TContainers>
struct areSameImpl<TContainer, TContainers...> {
  template<class T>
  using isAsFirstOne = areSame2_t<TContainer, T>;
  using areAllSame = And_t<isAsFirstOne<TContainers>...>;
  static constexpr bool value = areAllSame::value;
};

template<template<class...> class TContainer, class... TTypes>
struct areSameImpl<TContainer<TTypes...>> : std::true_type {};

template<>
struct areSameImpl<> : std::true_type {};
} // namespace NSTypeContainerDetail

template<class... TTypes>
struct areSame_t : NSTypeContainerDetail::areSameImpl<TTypes...> {};

template<class... TTypes>
constexpr bool areSame = areSame_t<TTypes...>::value;
//--------------------------------------------------------------------------------
// Convert
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {
template<class TContainer, template<class...> class TNewContainer>
struct ConvertImpl {
  static_assert(isTypeContainer<TContainer>,
                "First argument of Convert MUST be a type container!");
};

template<template<class...> class TContainer, class... TTypes,
         template<class...> class TNewContainer>
struct ConvertImpl<TContainer<TTypes...>, TNewContainer> {
  using type = TNewContainer<TTypes...>;
};

} // namespace NSTypeContainerDetail

template<class TContainer, template<class...> class TNewContainer>
using Convert =
    typename NSTypeContainerDetail::ConvertImpl<TContainer,
                                                TNewContainer>::type;
template<class TContainer>
using ConvertToList = Convert<TContainer, List>;
//--------------------------------------------------------------------------------
// Append = PushBack
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {
template<class TContainer, class... TTypes>
struct AppendImpl {
  static_assert(isTypeContainer<TContainer>,
                "First argument of Append MUST be a type container!");
};

template<template<class...> class TContainer, class... TTypesIn,
         class... TTypes>
struct AppendImpl<TContainer<TTypesIn...>, TTypes...> {
  using type = TContainer<TTypesIn..., TTypes...>;
};
} // namespace NSTypeContainerDetail

template<class TContainer, class... TTypes>
using Append =
    typename NSTypeContainerDetail::AppendImpl<TContainer, TTypes...>::type;

template<class TContainer, class... TTypes>
using PushBack = Append<TContainer, TTypes...>;
//--------------------------------------------------------------------------------
// Prepend = PushFront
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class TType, class TContainer>
struct Prepend1Impl;

template<class TType, template<class...> class TContainer, class... TTypes>
struct Prepend1Impl<TType, TContainer<TTypes...>> {
  using type = TContainer<TType, TTypes...>;
};
template<class TType, class TContainer>
using Prepend1 = typename Prepend1Impl<TType, TContainer>::type;

template<class TType, class... TTypes>
struct PrependImpl {
  using type = Prepend1<TType, typename PrependImpl<TTypes...>::type>;
};

template<class TContainer>
struct PrependImpl<TContainer> {
  static_assert(isTypeContainer<TContainer>,
                "Last argument of Prepend MUST be a type container!");
};

template<template<class...> class TContainer, class... TTypes>
struct PrependImpl<TContainer<TTypes...>> {
  using type = TContainer<TTypes...>;
};
} // namespace NSTypeContainerDetail

template<class... TTypes>
using Prepend = typename NSTypeContainerDetail::PrependImpl<TTypes...>::type;

template<class... TTypes>
using PushFront = Prepend<TTypes...>;
//--------------------------------------------------------------------------------
// Merge2
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class T1, class T2>
struct Merge2Impl {
  static_assert(isTypeContainer<T1>,
                "First argument of Merge Must be a type container!");
  static_assert(isTypeContainer<T2>,
                "Second argument of Merge Must be a type container!");
};

template<template<class...> class TContainer, class... TTypes1,
         class... TTypes2>
struct Merge2Impl<TContainer<TTypes1...>, TContainer<TTypes2...>> {
  using type = TContainer<TTypes1..., TTypes2...>;
};
} // namespace NSTypeContainerDetail

template<class T1, class T2>
using Merge2 = typename NSTypeContainerDetail::Merge2Impl<T1, T2>::type;
//--------------------------------------------------------------------------------
// Merge
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class... TContainers>
struct MergeImpl {
  static_assert(And<isTypeContainer_t<TContainers>...>,
                "All arguments of Merge MUST be type containers!");
  static_assert(areSame<TContainers...>,
                "All areguments of Merge MUST be the same type containers!");
};

template<template<class...> class TContainer, class... TTypes1,
         class... TTypes2, class... TContainers>
struct MergeImpl<TContainer<TTypes1...>, TContainer<TTypes2...>,
                 TContainers...> {
  using type = typename MergeImpl<TContainer<TTypes1..., TTypes2...>,
                                  TContainers...>::type;
};

template<template<class...> class TContainer, class... TTypes>
struct MergeImpl<TContainer<TTypes...>> {
  using type = TContainer<TTypes...>;
};
} // namespace NSTypeContainerDetail

template<class... TContainers>
using Merge = typename NSTypeContainerDetail::MergeImpl<TContainers...>::type;
//--------------------------------------------------------------------------------
// Size
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {
template<class T>
struct SizeImpl {
  static_assert(isTypeContainer<T>,
                "Argument of Size MUST be a type container!");
};

template<template<class...> class TContainer, class... TTypes>
struct SizeImpl<TContainer<TTypes...>> {
  static constexpr int value = sizeof...(TTypes);
};
} // namespace NSTypeContainerDetail

template<class T>
constexpr int Size = NSTypeContainerDetail::SizeImpl<T>::value;
//--------------------------------------------------------------------------------
// isEmpty
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {
template<class TContainer>
struct isEmptyImpl {
  static_assert(isTypeContainer<TContainer>,
                "First argument of isEmpty MUST be a type container!");
};

template<template<class...> class TContainer, class... TTypes>
struct isEmptyImpl<TContainer<TTypes...>> {
  static constexpr bool value = Size<TContainer<TTypes...>> == 0;
};
} // namespace NSTypeContainerDetail

template<class TContainer>
constexpr bool isEmpty = NSTypeContainerDetail::isEmptyImpl<TContainer>::value;
//--------------------------------------------------------------------------------
// TypeAt
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class TContainer, int TIndex>
struct isInBoundImpl {
  static constexpr bool value = (0 <= TIndex) && (TIndex < Size<TContainer>);
};

template<class TContainer, int TIndex>
constexpr bool isInBound = isInBoundImpl<TContainer, TIndex>::value;

template<class TContainer, int TIndex,
         bool isContainer = isTypeContainer<TContainer>,
         bool inBound = isInBound<TContainer, TIndex>>
struct TypeAtImpl {
  static_assert(isTypeContainer<TContainer>,
                "First Argument of TypeAt MUST be a type container!");
};

template<template<class...> class TContainer, class... TTypes, int TIndex>
struct TypeAtImpl<TContainer<TTypes...>, TIndex, true, false> {
  static_assert(TIndex >= 0, "Second argument of TypeAt MUST be non-negative!");
  static_assert(TIndex < Size<TContainer<TTypes...>>,
                "Index of TypeAt is out of bounds!");
};

template<template<class...> class TContainer, class TType1, class... TTypes,
         int TIndex>
struct TypeAtImpl<TContainer<TType1, TTypes...>, TIndex, true, true> {
  static_assert(TIndex > 0, "Internall Error in TypeAt!");
  using type =
      typename TypeAtImpl<TContainer<TTypes...>, TIndex - 1, true, true>::type;
};

template<template<class...> class TContainer, class TType1, class... TTypes>
struct TypeAtImpl<TContainer<TType1, TTypes...>, 0, true, true> {
  using type = TType1;
};
} // namespace NSTypeContainerDetail

template<class TContainer, int TIndex>
using TypeAt =
    typename NSTypeContainerDetail::TypeAtImpl<TContainer, TIndex>::type;
//--------------------------------------------------------------------------------
// IndexOf1
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {
template<class TContainer, class TType>
struct IndexOf1Impl {
  static_assert(isTypeContainer<TContainer>,
                "First argument of IndexOf1 MUST be a type container!");
};

template<template<class...> class TContainer, class TType1, class... TTypes,
         class TType>
struct IndexOf1Impl<TContainer<TType1, TTypes...>, TType> {
private:
  static constexpr int temp = IndexOf1Impl<TContainer<TTypes...>, TType>::value;

public:
  static constexpr int value = (temp == -1 ? -1 : 1 + temp);
};

template<template<class...> class TContainer, class... TTypes, class TType>
struct IndexOf1Impl<TContainer<TType, TTypes...>, TType> {
  static constexpr int value = 0;
};

template<template<class...> class TContainer, class TType>
struct IndexOf1Impl<TContainer<>, TType> {
  static constexpr int value = -1;
};
} // namespace NSTypeContainerDetail

template<class TContainer, class TType>
constexpr int IndexOf1 =
    NSTypeContainerDetail::IndexOf1Impl<TContainer, TType>::value;
//--------------------------------------------------------------------------------
// IndicesOf
//--------------------------------------------------------------------------------

// TO DO
// Implement Element Containers

namespace NSTypeContainerDetail {

template<class T1, class T2>
struct Merge2IntImpl;

template<template<int...> class TContainer, int... TValues1, int... TValues2>
struct Merge2IntImpl<TContainer<TValues1...>, TContainer<TValues2...>> {
  using type = TContainer<TValues1..., TValues2...>;
};

template<class T1, class T2>
using Merge2Int = typename Merge2IntImpl<T1, T2>::type;

template<class TContainer, class TType, int TStart = 0>
struct IndicesOfImpl {
  static_assert(isTypeContainer<TContainer>,
                "First argument of IndicesOf MUST be a type container!");
};

template<template<class...> class TContainer, class TType1, class... TTypes,
         class TType, int TStart>
struct IndicesOfImpl<TContainer<TType1, TTypes...>, TType, TStart> {
private:
  using TailList =
      typename IndicesOfImpl<TContainer<TTypes...>, TType, TStart + 1>::type;

public:
  using type = IfElse<std::is_same_v<TType1, TType>,
                      Merge2Int<ListInt<TStart>, TailList>, TailList>;
};
template<template<class...> class TContainer, class TType, int TStart>
struct IndicesOfImpl<TContainer<>, TType, TStart> {
  using type = ListInt<>;
};

} // namespace NSTypeContainerDetail

template<class TContainer, class TType>
using IndicesOf =
    typename NSTypeContainerDetail::IndicesOfImpl<TContainer, TType>::type;
//--------------------------------------------------------------------------------
// Contains
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {
template<class TContainer, class TType>
struct ContainsImpl {
  static_assert(isTypeContainer<TContainer>,
                "First argument of Contains MUST be a type container!");
};
template<template<class...> class TContainer, class... TTypes, class TType>
struct ContainsImpl<TContainer<TTypes...>, TType> {
  static constexpr bool value = IndexOf1<TContainer<TTypes...>, TType> != -1;
};
} // namespace NSTypeContainerDetail

template<class TContainer, class TType>
constexpr bool Contains =
    NSTypeContainerDetail::ContainsImpl<TContainer, TType>::value;
//--------------------------------------------------------------------------------
// Erase1
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class TContainer, class TType>
struct Erase1Impl {
  static_assert(isTypeContainer<TContainer>,
                "First argument of Erase1 MUST be a type container!");
};
template<template<class...> class TContainer, class TType1, class... TTypes,
         class TType>
struct Erase1Impl<TContainer<TType1, TTypes...>, TType> {
  using type =
      IfElse<std::is_same_v<TType1, TType>, TContainer<TTypes...>,
             Prepend1<TType1,
                      typename Erase1Impl<TContainer<TTypes...>, TType>::type>>;
};

template<template<class...> class TContainer, class TType>
struct Erase1Impl<TContainer<>, TType> {
  using type = TContainer<>;
};
} // namespace NSTypeContainerDetail

template<class TContainer, class TType>
using Erase1 =
    typename NSTypeContainerDetail::Erase1Impl<TContainer, TType>::type;
//--------------------------------------------------------------------------------
// EraseAll
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {
template<class TContainer, class TType>
struct EraseAllImpl {
  static_assert(isTypeContainer<TContainer>,
                "First argument of EraseAll MUST be a type container!");
};

template<template<class...> class TContainer, class TType1, class... TTypes,
         class TType>
struct EraseAllImpl<TContainer<TType1, TTypes...>, TType> {
private:
  using Tail = typename EraseAllImpl<TContainer<TTypes...>, TType>::type;

public:
  using type =
      IfElse<std::is_same_v<TType1, TType>, Tail, Prepend1<TType1, Tail>>;
};
template<template<class...> class TContainer, class TType>
struct EraseAllImpl<TContainer<>, TType> {
  using type = TContainer<>;
};

} // namespace NSTypeContainerDetail

template<class TContainer, class TType>
using EraseAll =
    typename NSTypeContainerDetail::EraseAllImpl<TContainer, TType>::type;
//--------------------------------------------------------------------------------
// NoDuplicates
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class TContainer>
struct NoDuplicatesImpl {
  static_assert(isTypeContainer<TContainer>,
                "Argument of NoDuplicates MUST be a type container!");
};

template<template<class...> class TContainer, class TType1, class... TTypes>
struct NoDuplicatesImpl<TContainer<TType1, TTypes...>> {
private:
  using GoodTail = typename NoDuplicatesImpl<TContainer<TTypes...>>::type;
  using TailWithoutHead = Erase1<GoodTail, TType1>;

public:
  using type = Prepend1<TType1, TailWithoutHead>;
};

template<template<class...> class TContainer>
struct NoDuplicatesImpl<TContainer<>> {
  using type = TContainer<>;
};

} // namespace NSTypeContainerDetail

template<class TContainer>
using NoDuplicates =
    typename NSTypeContainerDetail::NoDuplicatesImpl<TContainer>::type;
//--------------------------------------------------------------------------------
// Replace1
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class TContainer, class TReplace, class TReplaceWith>
struct Replace1Impl {
  static_assert(isTypeContainer<TContainer>,
                "First argument of Replace1 MUST be a type container!");
};

template<template<class...> class TContainer, class TType1, class... TTypes,
         class TReplace, class TReplaceWith>
struct Replace1Impl<TContainer<TType1, TTypes...>, TReplace, TReplaceWith> {
  using type = IfElse<
      std::is_same_v<TType1, TReplace>, TContainer<TReplaceWith, TTypes...>,
      Prepend1<TType1, typename Replace1Impl<TContainer<TTypes...>, TReplace,
                                             TReplaceWith>::type>>;
};
template<template<class...> class TContainer, class TReplace,
         class TReplaceWith>
struct Replace1Impl<TContainer<>, TReplace, TReplaceWith> {
  using type = TContainer<>;
};

} // namespace NSTypeContainerDetail

template<class TContainer, class TReplace, class TReplaceWith>
using Replace1 =
    typename NSTypeContainerDetail::Replace1Impl<TContainer, TReplace,
                                                 TReplaceWith>::type;
//--------------------------------------------------------------------------------
// ReplaceAll
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class TContainer, class TReplace, class TReplaceWith>
struct ReplaceAllImpl {
  static_assert(isTypeContainer<TContainer>,
                "First argument of ReplaceAll MUST be a type container!");
};

template<template<class...> class TContainer, class TType1, class... TTypes,
         class TReplace, class TReplaceWith>
struct ReplaceAllImpl<TContainer<TType1, TTypes...>, TReplace, TReplaceWith> {
private:
  using Head = IfElse<std::is_same_v<TType1, TReplace>,
                      TContainer<TReplaceWith>, TContainer<TType1>>;
  using Tail = typename ReplaceAllImpl<TContainer<TTypes...>, TReplace,
                                       TReplaceWith>::type;

public:
  using type = Merge2<Head, Tail>;
};

template<template<class...> class TContainer, class TReplace,
         class TReplaceWith>
struct ReplaceAllImpl<TContainer<>, TReplace, TReplaceWith> {
  using type = TContainer<>;
};

} // namespace NSTypeContainerDetail

template<class TContainer, class TReplace, class TReplaceWith>
using ReplaceAll =
    typename NSTypeContainerDetail::ReplaceAllImpl<TContainer, TReplace,
                                                   TReplaceWith>::type;
//--------------------------------------------------------------------------------
// Map
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<template<class> class TF, class TContainer>
struct MapImpl {
  static_assert(isTypeContainer<TContainer>,
                "Second argument of Map MUST be a type container!");
};

template<template<class> class TF, template<class...> class TContainer,
         class... TTypes>
struct MapImpl<TF, TContainer<TTypes...>> {
  using type = TContainer<TF<TTypes>...>;
};
} // namespace NSTypeContainerDetail

template<template<class> class TF, class TContainer>
using Map = typename NSTypeContainerDetail::MapImpl<TF, TContainer>::type;
//--------------------------------------------------------------------------------
// PowerSet
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class TContainer>
struct PowerSetImpl {
  static_assert(isTypeContainer<TContainer>,
                "Argument of PowerSet MUST be a type container!");
};

template<template<class...> class TContainer, class TType, class... TTypes>
struct PowerSetImpl<TContainer<TType, TTypes...>> {
private:
  using TailPowerSet = typename PowerSetImpl<TContainer<TTypes...>>::type;

  template<class T>
  using AddHeader = Prepend1<TType, T>;

  using HeadWithOthers = Map<AddHeader, TailPowerSet>;

public:
  using type = Merge2<HeadWithOthers, TailPowerSet>;
};

template<template<class...> class TContainer>
struct PowerSetImpl<TContainer<>> {
  using type = TContainer<TContainer<>>;
};
} // namespace NSTypeContainerDetail

template<class TContainer>
using PowerSet = typename NSTypeContainerDetail::PowerSetImpl<TContainer>::type;
//--------------------------------------------------------------------------------
// PairSet
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class TContainer>
struct PairSetImpl {
  static_assert(isTypeContainer<TContainer>,
                "Argument of PairSet MUST be a type container!");
};

template<template<class...> class TContainer, class TType, class... TTypes>
struct PairSetImpl<TContainer<TType, TTypes...>> {
private:
  using TailPairs = typename PairSetImpl<TContainer<TTypes...>>::type;

  template<class T>
  using AddHeadFront = TContainer<TType, T>;

  using HeadListPairs = Map<AddHeadFront, TContainer<TType, TTypes...>>;

  template<class T>
  using AddHeadBack = TContainer<T, TType>;

  using TailHeadPairs = Map<AddHeadBack, TContainer<TTypes...>>;

public:
  using type = Merge<HeadListPairs, TailHeadPairs, TailPairs>;
};

template<template<class...> class TContainer>
struct PairSetImpl<TContainer<>> {
  using type = TContainer<>;
};
} // namespace NSTypeContainerDetail

template<class TContainer>
using PairSet = typename NSTypeContainerDetail::PairSetImpl<TContainer>::type;
//--------------------------------------------------------------------------------
// SymPairSet
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<class TContainer>
struct SymPairSetImpl {
  static_assert(isTypeContainer<TContainer>,
                "Argument of SymPairSet MUST be a type container!");
};

template<template<class...> class TContainer, class TType, class... TTypes>
struct SymPairSetImpl<TContainer<TType, TTypes...>> {
private:
  using SymTailPairs = typename SymPairSetImpl<TContainer<TTypes...>>::type;

  template<class T>
  using AddHeadFront = TContainer<TType, T>;

  using HeadWithTail = Map<AddHeadFront, TContainer<TType, TTypes...>>;

public:
  using type = Merge2<HeadWithTail, SymTailPairs>;
};

template<template<class...> class TContainer>
struct SymPairSetImpl<TContainer<>> {
  using type = TContainer<>;
};

} // namespace NSTypeContainerDetail

template<class TContainer>
using SymPairSet =
    typename NSTypeContainerDetail::SymPairSetImpl<TContainer>::type;
} // namespace TC
} // namespace NSLibrary
