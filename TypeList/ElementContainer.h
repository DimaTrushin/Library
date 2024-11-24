#pragma once

#include "Logic.h"

#include <tuple>

namespace NSLibrary {

namespace EC {
//--------------------------------------------------------------------------------
// List
//--------------------------------------------------------------------------------

template<class Type, Type... Elements>
struct List {};
template<class... Types>
struct ListT {};
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
// isElementContainer
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class T>
struct isElementContainerImpl : std::false_type {};

template<class Type, template<class, Type...> class TContainer,
         Type... TElements>
struct isElementContainerImpl<TContainer<Type, TElements...>> : std::true_type {
};
} // namespace NSElementContainerDetail

template<class T>
struct isElementContainer_t
    : NSElementContainerDetail::isElementContainerImpl<T> {};

template<class T>
constexpr bool isElementContainer =
    NSElementContainerDetail::isElementContainerImpl<T>::value;
//--------------------------------------------------------------------------------
// areSame2
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {
template<class T1, class T2>
struct areSame2Impl : std::false_type {};

template<class Type, template<class, Type...> class TContainer,
         Type... TElements1, Type... TElements2>
struct areSame2Impl<TContainer<Type, TElements1...>,
                    TContainer<Type, TElements2...>> : std::true_type {};

} // namespace NSElementContainerDetail

template<class T1, class T2>
struct areSame2_t : NSElementContainerDetail::areSame2Impl<T1, T2> {};

template<class T1, class T2>
constexpr bool areSame2 = areSame2_t<T1, T2>::value;
//--------------------------------------------------------------------------------
// areSame
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class... TElements>
struct areSameImpl : std::false_type {};

template<class TContainer, class... TContainers>
struct areSameImpl<TContainer, TContainers...> {
private:
  template<class T>
  using isAsFirstOne = areSame2_t<TContainer, T>;
  using areAllSame = And_t<isAsFirstOne<TContainers>...>;

public:
  static constexpr bool value = areAllSame::value;
};

template<class Type, template<class, Type...> class TContainer,
         Type... TElements>
struct areSameImpl<TContainer<Type, TElements...>> : std::true_type {};

template<>
struct areSameImpl<> : std::true_type {};
} // namespace NSElementContainerDetail

template<class... TElements>
struct areSame_t : NSElementContainerDetail::areSameImpl<TElements...> {};

template<class... TElements>
constexpr bool areSame = areSame_t<TElements...>::value;
//--------------------------------------------------------------------------------
// TypeOf
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {
template<class TContainer>
struct TypeOfImpl {
  static_assert(isElementContainer<TContainer>,
                "Argument of TypeOf MUST be an element container!");
};

template<class Type, template<class, Type...> class TContainer,
         Type... Elements>
struct TypeOfImpl<TContainer<Type, Elements...>> {
  using type = Type;
};

} // namespace NSElementContainerDetail

template<class TContainer>
using TypeOf = typename NSElementContainerDetail::TypeOfImpl<TContainer>::type;
//--------------------------------------------------------------------------------
// Convert
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {
template<class TContainer, template<class Type, Type...> class TNewContainer>
struct ConvertImpl {
  static_assert(isElementContainer<TContainer>,
                "First argument of Convert MUST be an element container!");
};

template<class Type, template<class, Type...> class TContainer,
         Type... TElements, template<class, Type...> class TNewContainer>
struct ConvertImpl<TContainer<Type, TElements...>, TNewContainer> {
  using type = TNewContainer<Type, TElements...>;
};

} // namespace NSElementContainerDetail

template<class TContainer, template<class Type, Type...> class TNewContainer>
using Convert =
    typename NSElementContainerDetail::ConvertImpl<TContainer,
                                                   TNewContainer>::type;
template<class TContainer>
using ConvertToList = Convert<TContainer, List>;
//--------------------------------------------------------------------------------
// Append = PushBack
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {
// TO DO
// implement list of elements of different types

template<auto... TElements>
struct haveSameTypeImpl;

template<class Type1, Type1 TFirst, class Type2, Type2 TSecond,
         auto... TElements>
struct haveSameTypeImpl<TFirst, TSecond, TElements...> {
  static constexpr bool value = std::is_same_v<Type1, Type2> &&
                                haveSameTypeImpl<TSecond, TElements...>::value;
};

template<class Type, Type TElement>
struct haveSameTypeImpl<TElement> : std::true_type {};

template<>
struct haveSameTypeImpl<> : std::true_type {};

template<auto... TElements>
constexpr bool haveSameType = haveSameTypeImpl<TElements...>::value;

template<auto... TElements>
struct FirstTypeImpl;

template<class Type, Type TFirst, auto... TElements>
struct FirstTypeImpl<TFirst, TElements...> {
  using type = Type;
};

template<>
struct FirstTypeImpl<> {
  using type = void;
};

template<auto... TElements>
using FirstType = typename FirstTypeImpl<TElements...>::type;

template<class TContainer, class Type, Type... TElements>
struct AppendSameImpl {
  static_assert(isElementContainer<TContainer>,
                "First argument of Append MUST be an element container!");
};

template<class Type1, template<class, Type1...> class TContainer,
         Type1... TElements1, class Type2, Type2... TElements2>
struct AppendSameImpl<TContainer<Type1, TElements1...>, Type2, TElements2...> {
  static_assert(std::is_same_v<Type1, Type2>,
                "Type of the container elements in Append "
                "MUST be the same as the "
                "type of appended elements!");
  using type = TContainer<Type1, TElements1..., TElements2...>;
};

template<class TContainer, auto... TElements>
struct AppendImpl {
  static_assert(haveSameType<TElements...>,
                "Elements of Append MUST have the same type!");
  using type = typename AppendSameImpl<TContainer, FirstType<TElements...>,
                                       TElements...>::type;
};

template<class TContainer>
struct AppendImpl<TContainer> {
  using type = TContainer;
};

} // namespace NSElementContainerDetail

template<class TContainer, auto... TElements>
using Append =
    typename NSElementContainerDetail::AppendImpl<TContainer,
                                                  TElements...>::type;

template<class TContainer, auto... TElements>
using PushBack = Append<TContainer, TElements...>;
//--------------------------------------------------------------------------------
// Prepend1 = PushFront1
//--------------------------------------------------------------------------------
namespace NSElementContainerDetail {

template<auto TElement, class TContainer>
struct Prepend1Impl {
  static_assert(isElementContainer<TContainer>,
                "First argument of Prepend MUST be an element container!");
  static_assert(std::is_same_v<decltype(TElement), TypeOf<TContainer>>,
                "Type of the first argument of Prepend1 MUST be the same as "
                "the type of the elements in the container!");
};

template<class Type, template<class, Type...> class TContainer, Type TElement,
         Type... TElements>
struct Prepend1Impl<TElement, TContainer<Type, TElements...>> {
  using type = TContainer<Type, TElement, TElements...>;
};

} // namespace NSElementContainerDetail

template<auto TElement, class TContainer>
using Prepend1 =
    typename NSElementContainerDetail::Prepend1Impl<TElement, TContainer>::type;

template<auto TElement, class TContainer>
using PushFront1 = Prepend1<TElement, TContainer>;
//--------------------------------------------------------------------------------
// Prepend = PushFront
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class TContainer, class Type, Type... TElements>
struct PrependSameImpl {
  static_assert(isElementContainer<TContainer>,
                "First argument of Prepend MUST be an element container!");
};

template<class Type1, template<class, Type1...> class TContainer,
         Type1... TElements1, class Type2, Type2... TElements2>
struct PrependSameImpl<TContainer<Type1, TElements1...>, Type2, TElements2...> {
  static_assert(std::is_same_v<Type1, Type2>,
                "Type of the container elements in Prepend "
                "MUST be the same as the "
                "type of prepended elements!");
  using type = TContainer<Type1, TElements2..., TElements1...>;
};

template<class TContainer, auto... TElements>
struct PrependImpl {
  static_assert(haveSameType<TElements...>,
                "Elements of Prepend MUST have the same type!");
  using type = typename PrependSameImpl<TContainer, FirstType<TElements...>,
                                        TElements...>::type;
};

} // namespace NSElementContainerDetail

template<class TContainer, auto... TElements>
using Prepend =
    typename NSElementContainerDetail::PrependImpl<TContainer,
                                                   TElements...>::type;

template<class TContainer, auto... TElements>
using PushFront = Prepend<TContainer, TElements...>;
//--------------------------------------------------------------------------------
// Merge2
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class T1, class T2>
struct Merge2Impl {
  static_assert(isElementContainer<T1>,
                "First argument of Merge MUST be an element container!");
  static_assert(isElementContainer<T2>,
                "Second argument of Merge MUST be an element container!");
  static_assert(areSame2<T1, T2>, "First and Second arguments of Merge MUST be "
                                  "the same element containers!");
  static_assert(std::is_same_v<TypeOf<T1>, TypeOf<T2>>,
                "Types of elements in containers in Merge MUST be the same!");
};

template<class Type, template<class, Type...> class TContainer,
         Type... TElements1, Type... TElements2>
struct Merge2Impl<TContainer<Type, TElements1...>,
                  TContainer<Type, TElements2...>> {
  using type = TContainer<Type, TElements1..., TElements2...>;
};
} // namespace NSElementContainerDetail

template<class T1, class T2>
using Merge2 = typename NSElementContainerDetail::Merge2Impl<T1, T2>::type;
//--------------------------------------------------------------------------------
// Merge
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class... Ts>
struct isSameImpl;

template<class T1, class T2, class... Ts>
struct isSameImpl<T1, T2, Ts...> {
  static constexpr bool value =
      std::is_same_v<T1, T2> && isSameImpl<T2, Ts...>::value;
};

template<class T>
struct isSameImpl<T> : std::true_type {};

template<>
struct isSameImpl<> : std::true_type {};

template<class... Ts>
constexpr bool isSame = isSameImpl<Ts...>::value;

template<class... TContainers>
struct MergeImpl {
  static_assert(And<isElementContainer_t<TContainers>...>,
                "All arguments of Merge MUST be element containers!");
  static_assert(areSame<TContainers...>,
                "All arguments of Merge MUST be the same element containers!");
  static_assert(isSame<TypeOf<TContainers>...>,
                "Types of all containers in Merge MUST be the same!");
  static_assert(sizeof...(TContainers) > 0,
                "You MUST provide at least one container to Merge!");
};

template<class Type, template<class, Type...> class TContainer,
         Type... TElements1, Type... TElements2, class... TContainers>
struct MergeImpl<TContainer<Type, TElements1...>,
                 TContainer<Type, TElements2...>, TContainers...> {
  using type =
      typename MergeImpl<TContainer<Type, TElements1..., TElements2...>,
                         TContainers...>::type;
};

template<class Type, template<class, Type...> class TContainer,
         Type... TElements>
struct MergeImpl<TContainer<Type, TElements...>> {
  using type = TContainer<Type, TElements...>;
};
} // namespace NSElementContainerDetail

template<class... TContainers>
using Merge =
    typename NSElementContainerDetail::MergeImpl<TContainers...>::type;
//--------------------------------------------------------------------------------
// Size
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {
template<class T>
struct SizeImpl {
  static_assert(isElementContainer<T>,
                "Argument of Size MUST be an element container!");
};

template<class Type, template<class, Type...> class TContainer,
         Type... TElements>
struct SizeImpl<TContainer<Type, TElements...>> {
  static constexpr int value = sizeof...(TElements);
};
} // namespace NSElementContainerDetail

template<class T>
constexpr int Size = NSElementContainerDetail::SizeImpl<T>::value;
//--------------------------------------------------------------------------------
// isEmpty
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {
template<class TContainer>
struct isEmptyImpl {
  static_assert(isElementContainer<TContainer>,
                "First argument of isEmpty MUST be an element container!");
};

template<class Type, template<class, Type...> class TContainer,
         Type... TElements>
struct isEmptyImpl<TContainer<Type, TElements...>> {
  static constexpr bool value = Size<TContainer<Type, TElements...>> == 0;
};
} // namespace NSElementContainerDetail

template<class TContainer>
constexpr bool isEmpty =
    NSElementContainerDetail::isEmptyImpl<TContainer>::value;
//--------------------------------------------------------------------------------
// ElementAt
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class TContainer, int TIndex>
struct isInBoundImpl {
  static constexpr bool value = (0 <= TIndex) && (TIndex < Size<TContainer>);
};

template<class TContainer, int TIndex>
constexpr bool isInBound = isInBoundImpl<TContainer, TIndex>::value;

template<class TContainer, int TIndex,
         bool isContainer = isElementContainer<TContainer>,
         bool inBound = isInBound<TContainer, TIndex>>
struct ElementAtImpl {
  static_assert(isElementContainer<TContainer>,
                "First Argument of ElementAt MUST be an element container!");
};

template<class Type, template<class, Type...> class TContainer,
         Type... TElements, int TIndex>
struct ElementAtImpl<TContainer<Type, TElements...>, TIndex, true, false> {
  static_assert(TIndex >= 0,
                "Second argument of ElementAt MUST be non-negative!");
  static_assert(TIndex < Size<TContainer<Type, TElements...>>,
                "Index of ElementAt is out of bounds!");
};

template<class Type, template<class, Type...> class TContainer, Type TElement1,
         Type... TElements, int TIndex>
struct ElementAtImpl<TContainer<Type, TElement1, TElements...>, TIndex, true,
                     true> {
  static_assert(TIndex > 0, "Internall Error in ElementAt!");
  static constexpr Type value = ElementAtImpl<TContainer<Type, TElements...>,
                                              TIndex - 1, true, true>::value;
};

template<class Type, template<class, Type...> class TContainer, Type TElement1,
         Type... TElements>
struct ElementAtImpl<TContainer<Type, TElement1, TElements...>, 0, true, true> {
  static constexpr Type value = TElement1;
};
} // namespace NSElementContainerDetail

template<class TContainer, int TIndex>
constexpr TypeOf<TContainer> ElementAt =
    NSElementContainerDetail::ElementAtImpl<TContainer, TIndex>::value;
//--------------------------------------------------------------------------------
// IndexOf1
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {
template<class TContainer, auto TElement>
struct IndexOf1Impl {
  static_assert(isElementContainer<TContainer>,
                "First argument of IndexOf1 MUST be an element container!");
  static_assert(
      std::is_same_v<decltype(TElement), TypeOf<TContainer>>,
      "The type of the second argument of IndexOf1 MUST be the same as the "
      "type of the elements in the container in the first argument!");
};

template<class Type, template<class, Type...> class TContainer, Type TElement1,
         Type... TElements, Type TElement>
struct IndexOf1Impl<TContainer<Type, TElement1, TElements...>, TElement> {
private:
  static constexpr int temp =
      IndexOf1Impl<TContainer<Type, TElements...>, TElement>::value;

public:
  static constexpr int value = (temp == -1 ? -1 : 1 + temp);
};

template<class Type, template<class, Type...> class TContainer,
         Type... TElements, Type TElement>
struct IndexOf1Impl<TContainer<Type, TElement, TElements...>, TElement> {
  static constexpr int value = 0;
};

template<class Type, template<class, Type...> class TContainer, Type TElement>
struct IndexOf1Impl<TContainer<Type>, TElement> {
  static constexpr int value = -1;
};
} // namespace NSElementContainerDetail

template<class TContainer, auto TElement>
constexpr int IndexOf1 =
    NSElementContainerDetail::IndexOf1Impl<TContainer, TElement>::value;
//--------------------------------------------------------------------------------
// IndicesOf
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class TContainer, auto TElement, int TStart = 0>
struct IndicesOfImpl {
  static_assert(isElementContainer<TContainer>,
                "First argument of IndicesOf MUST be an element container!");
  static_assert(
      std::is_same_v<decltype(TElement), TypeOf<TContainer>>,
      "The type of the second argument of IndecesOf MUST be the same as the "
      "type of the elements in the container in the first argument!");
};

template<class Type, template<class, Type...> class TContainer, Type TElement1,
         Type... TElements, Type TElement, int TStart>
struct IndicesOfImpl<TContainer<Type, TElement1, TElements...>, TElement,
                     TStart> {
private:
  using TailList = typename IndicesOfImpl<TContainer<Type, TElements...>,
                                          TElement, TStart + 1>::type;

public:
  using type = IfElse<(TElement1 == TElement),
                      Merge2<List<int, TStart>, TailList>, TailList>;
};
template<class Type, template<class, Type...> class TContainer, Type TElement,
         int TStart>
struct IndicesOfImpl<TContainer<Type>, TElement, TStart> {
  using type = List<int>;
};

} // namespace NSElementContainerDetail

template<class TContainer, auto TElement>
using IndicesOf =
    typename NSElementContainerDetail::IndicesOfImpl<TContainer,
                                                     TElement>::type;
//--------------------------------------------------------------------------------
// Contains
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {
template<class TContainer, auto TElement>
struct ContainsImpl {
  static_assert(isElementContainer<TContainer>,
                "First argument of Contains MUST be an element container!");
  static_assert(
      std::is_same_v<decltype(TElement), TypeOf<TContainer>>,
      "The type of the second argument of Contains MUST be the same as the "
      "type of the elements in the container in the first argument!");
};
template<class Type, template<class, Type...> class TContainer,
         Type... TElements, Type TElement>
struct ContainsImpl<TContainer<Type, TElements...>, TElement> {
  static constexpr bool value =
      IndexOf1<TContainer<Type, TElements...>, TElement> != -1;
};
} // namespace NSElementContainerDetail

template<class TContainer, auto TElement>
constexpr bool Contains =
    NSElementContainerDetail::ContainsImpl<TContainer, TElement>::value;
//--------------------------------------------------------------------------------
// Erase1
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class TContainer, auto TElement>
struct Erase1Impl {
  static_assert(isElementContainer<TContainer>,
                "First argument of Erase1 MUST be an element container!");
  static_assert(
      std::is_same_v<decltype(TElement), TypeOf<TContainer>>,
      "The type of the second argument of Erase1 MUST be the same as the "
      "type of the elements in the container in the first argument!");
};

template<class Type, template<class, Type...> class TContainer, Type TElement1,
         Type... TElements, Type TElement>
struct Erase1Impl<TContainer<Type, TElement1, TElements...>, TElement> {
  using type = IfElse<
      (TElement1 == TElement), TContainer<Type, TElements...>,
      Prepend1<TElement1, typename Erase1Impl<TContainer<Type, TElements...>,
                                              TElement>::type>>;
};

template<class Type, template<class, Type...> class TContainer, Type TElement>
struct Erase1Impl<TContainer<Type>, TElement> {
  using type = TContainer<Type>;
};
} // namespace NSElementContainerDetail

template<class TContainer, auto TElement>
using Erase1 =
    typename NSElementContainerDetail::Erase1Impl<TContainer, TElement>::type;
//--------------------------------------------------------------------------------
// EraseAll
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {
template<class TContainer, auto TElement>
struct EraseAllImpl {
  static_assert(isElementContainer<TContainer>,
                "First argument of EraseAll MUST be an element container!");
  static_assert(
      std::is_same_v<decltype(TElement), TypeOf<TContainer>>,
      "The type of the second argument of Erase MUST be the same as the "
      "type of the elements in the container in the first argument!");
};

template<class Type, template<class, Type...> class TContainer, Type TElement1,
         Type... TElements, Type TElement>
struct EraseAllImpl<TContainer<Type, TElement1, TElements...>, TElement> {
private:
  using Tail =
      typename EraseAllImpl<TContainer<Type, TElements...>, TElement>::type;

public:
  using type = IfElse<(TElement1 == TElement), Tail, Prepend1<TElement1, Tail>>;
};

template<class Type, template<class, Type...> class TContainer, Type TElement>
struct EraseAllImpl<TContainer<Type>, TElement> {
  using type = TContainer<Type>;
};

} // namespace NSElementContainerDetail

template<class TContainer, auto TElement>
using EraseAll =
    typename NSElementContainerDetail::EraseAllImpl<TContainer, TElement>::type;
//--------------------------------------------------------------------------------
// NoDuplicates
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class TContainer>
struct NoDuplicatesImpl {
  static_assert(isElementContainer<TContainer>,
                "Argument of NoDuplicates MUST be an element container!");
};

template<class Type, template<class, Type...> class TContainer, Type TElement1,
         Type... TElements>
struct NoDuplicatesImpl<TContainer<Type, TElement1, TElements...>> {
private:
  using GoodTail =
      typename NoDuplicatesImpl<TContainer<Type, TElements...>>::type;
  using TailWithoutHead = Erase1<GoodTail, TElement1>;

public:
  using type = Prepend1<TElement1, TailWithoutHead>;
};

template<class Type, template<class, Type...> class TContainer>
struct NoDuplicatesImpl<TContainer<Type>> {
  using type = TContainer<Type>;
};

} // namespace NSElementContainerDetail

template<class TContainer>
using NoDuplicates =
    typename NSElementContainerDetail::NoDuplicatesImpl<TContainer>::type;
//--------------------------------------------------------------------------------
// Replace1
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class TContainer, auto TReplace, auto TReplaceWith>
struct Replace1Impl {
  static_assert(isElementContainer<TContainer>,
                "First argument of Replace1 MUST be an element container!");
  static_assert(std::is_same_v<decltype(TReplace), TypeOf<TContainer>>,
                "The type of the second argument of Replace1 MUST be of the "
                "same type as the elements of the container!");
  static_assert(std::is_same_v<decltype(TReplaceWith), TypeOf<TContainer>>,
                "The type of the third argument of Replace1 MUST be of the "
                "same type as the elements of the container!");
};

template<class Type, template<class, Type...> class TContainer, Type TElement1,
         Type... TElements, Type TReplace, Type TReplaceWith>
struct Replace1Impl<TContainer<Type, TElement1, TElements...>, TReplace,
                    TReplaceWith> {
  using type = IfElse<
      (TElement1 == TReplace), TContainer<Type, TReplaceWith, TElements...>,
      Prepend1<TElement1, typename Replace1Impl<TContainer<Type, TElements...>,
                                                TReplace, TReplaceWith>::type>>;
};
template<class Type, template<class, Type...> class TContainer, Type TReplace,
         Type TReplaceWith>
struct Replace1Impl<TContainer<Type>, TReplace, TReplaceWith> {
  using type = TContainer<Type>;
};

} // namespace NSElementContainerDetail

template<class TContainer, auto TReplace, auto TReplaceWith>
using Replace1 =
    typename NSElementContainerDetail::Replace1Impl<TContainer, TReplace,
                                                    TReplaceWith>::type;
//--------------------------------------------------------------------------------
// ReplaceAll
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class TContainer, auto TReplace, auto TReplaceWith>
struct ReplaceAllImpl {
  static_assert(isElementContainer<TContainer>,
                "First argument of ReplaceAll MUST be an element container!");
  static_assert(std::is_same_v<decltype(TReplace), TypeOf<TContainer>>,
                "The type of the second argument of ReplaceAll MUST be of the "
                "same type as the elements of the container!");
  static_assert(std::is_same_v<decltype(TReplaceWith), TypeOf<TContainer>>,
                "The type of the third argument of ReplaceAll MUST be of the "
                "same type as the elements of the container!");
};

template<class Type, template<class, Type...> class TContainer, Type TElement1,
         Type... TElements, Type TReplace, Type TReplaceWith>
struct ReplaceAllImpl<TContainer<Type, TElement1, TElements...>, TReplace,
                      TReplaceWith> {
private:
  using Head = IfElse<(TElement1 == TReplace), TContainer<Type, TReplaceWith>,
                      TContainer<Type, TElement1>>;
  using Tail = typename ReplaceAllImpl<TContainer<Type, TElements...>, TReplace,
                                       TReplaceWith>::type;

public:
  using type = Merge2<Head, Tail>;
};

template<class Type, template<class, Type...> class TContainer, Type TReplace,
         Type TReplaceWith>
struct ReplaceAllImpl<TContainer<Type>, TReplace, TReplaceWith> {
  using type = TContainer<Type>;
};

} // namespace NSElementContainerDetail

template<class TContainer, auto TReplace, auto TReplaceWith>
using ReplaceAll =
    typename NSElementContainerDetail::ReplaceAllImpl<TContainer, TReplace,
                                                      TReplaceWith>::type;
//--------------------------------------------------------------------------------
// Map
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<auto TF, class Type>
struct ReturnTypeImpl {
  using type = decltype(TF(std::declval<Type>()));
};
template<auto TF, class Type>
using ReturnType = typename ReturnTypeImpl<TF, Type>::type;

template<auto TF, class TContainer>
struct MapImpl {
  static_assert(isElementContainer<TContainer>,
                "Second argument of Map MUST be an element container!");
};

template<auto TF, class Type, template<class, Type...> class TContainer,
         Type... TElements>
struct MapImpl<TF, TContainer<Type, TElements...>> {
  using type = TContainer<ReturnType<TF, Type>, TF(TElements)...>;
};
} // namespace NSElementContainerDetail

template<auto TF, class TContainer>
using Map = typename NSElementContainerDetail::MapImpl<TF, TContainer>::type;
//--------------------------------------------------------------------------------
// PowerSet
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

namespace NSTypeContainerDetail {

template<template<class> class TF, class TContainer>
struct MapTImpl {};

template<template<class> class TF, template<class...> class TContainer,
         class... TTypes>
struct MapTImpl<TF, TContainer<TTypes...>> {
  using type = TContainer<TF<TTypes>...>;
};
} // namespace NSTypeContainerDetail

template<template<class> class TF, class TContainer>
using MapT = typename NSTypeContainerDetail::MapTImpl<TF, TContainer>::type;

namespace NSTypeContainerDetail {

template<class T1, class T2>
struct Merge2TImpl {};

template<template<class...> class TContainer, class... TTypes1,
         class... TTypes2>
struct Merge2TImpl<TContainer<TTypes1...>, TContainer<TTypes2...>> {
  using type = TContainer<TTypes1..., TTypes2...>;
};
} // namespace NSTypeContainerDetail

template<class T1, class T2>
using Merge2T = typename NSTypeContainerDetail::Merge2TImpl<T1, T2>::type;

template<class TContainer>
struct PowerSetImpl {
  static_assert(isElementContainer<TContainer>,
                "Argument of PowerSet MUST be an element container!");
};

template<class Type, template<class, Type...> class TContainer, Type TElement,
         Type... TElements>
struct PowerSetImpl<TContainer<Type, TElement, TElements...>> {
private:
  using TailPowerSet =
      typename PowerSetImpl<TContainer<Type, TElements...>>::type;

  template<class T>
  using AddHeader = Prepend1<TElement, T>;

  using HeadWithOthers = MapT<AddHeader, TailPowerSet>;

public:
  using type = Merge2T<HeadWithOthers, TailPowerSet>;
};

template<class Type, template<class, Type...> class TContainer>
struct PowerSetImpl<TContainer<Type>> {
  using type = ListT<TContainer<Type>>;
};
} // namespace NSElementContainerDetail

template<class TContainer>
using PowerSet =
    typename NSElementContainerDetail::PowerSetImpl<TContainer>::type;
//--------------------------------------------------------------------------------
// PairSet
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class TContainer>
struct PairSetImpl {
  static_assert(isElementContainer<TContainer>,
                "Argument of PairSet MUST be an element container!");
};

template<class Type, template<class, Type...> class TContainer, Type TElement,
         Type... TElements>
struct PairSetImpl<TContainer<Type, TElement, TElements...>> {
private:
  using TailPairs = typename PairSetImpl<TContainer<Type, TElements...>>::type;

  template<class T>
  using AddHeadFront = Prepend1<TElement, T>;

  using HeadListPairs =
      MapT<AddHeadFront,
           ListT<TContainer<Type, TElement>, TContainer<Type, TElements>...>>;

  template<class T>
  using AddHeadBack = Append<T, TElement>;

  using TailHeadPairs =
      MapT<AddHeadBack, ListT<TContainer<Type, TElements>...>>;

public:
  using type = Merge2T<Merge2T<HeadListPairs, TailHeadPairs>, TailPairs>;
};

template<class Type, template<class, Type...> class TContainer>
struct PairSetImpl<TContainer<Type>> {
  using type = ListT<>;
};
} // namespace NSElementContainerDetail

template<class TContainer>
using PairSet =
    typename NSElementContainerDetail::PairSetImpl<TContainer>::type;
//--------------------------------------------------------------------------------
// SymPairSet
//--------------------------------------------------------------------------------

namespace NSElementContainerDetail {

template<class TContainer>
struct SymPairSetImpl {
  static_assert(isElementContainer<TContainer>,
                "Argument of SymPairSet MUST be an element container!");
};

template<class Type, template<class, Type...> class TContainer, Type TElement,
         Type... TElements>
struct SymPairSetImpl<TContainer<Type, TElement, TElements...>> {
private:
  using SymTailPairs =
      typename SymPairSetImpl<TContainer<Type, TElements...>>::type;

  template<class T>
  using AddHeadFront = Prepend1<TElement, T>;

  using HeadWithTail =
      MapT<AddHeadFront,
           ListT<TContainer<Type, TElement>, TContainer<Type, TElements>...>>;

public:
  using type = Merge2T<HeadWithTail, SymTailPairs>;
};

template<class Type, template<class, Type...> class TContainer>
struct SymPairSetImpl<TContainer<Type>> {
  using type = ListT<>;
};

} // namespace NSElementContainerDetail

template<class TContainer>
using SymPairSet =
    typename NSElementContainerDetail::SymPairSetImpl<TContainer>::type;
} // namespace EC
} // namespace NSLibrary
