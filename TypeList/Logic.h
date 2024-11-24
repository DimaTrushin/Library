#pragma once
#include <type_traits>

namespace NSLibrary {

namespace Logic {
//--------------------------------------------------------------------------------
// IfElse
//--------------------------------------------------------------------------------

namespace NSTypeContainerDetail {

template<bool TCondition, class T1, class T2>
struct IfElseImpl : std::conditional<TCondition, T1, T2> {};
} // namespace NSTypeContainerDetail

template<bool TCondition, class T1, class T2>
using IfElse =
    typename NSTypeContainerDetail::IfElseImpl<TCondition, T1, T2>::type;
//--------------------------------------------------------------------------------
// And, Or
//--------------------------------------------------------------------------------

template<class... Ts>
struct And_t : std::conjunction<Ts...> {};

template<class... Ts>
constexpr bool And = And_t<Ts...>::value;

template<class... Ts>
struct Or_t : std::disjunction<Ts...> {};

template<class... Ts>
constexpr bool Or = Or_t<Ts...>::value;

template<class T>
struct Not_t : std::negation<T> {};

template<class T>
constexpr bool Not = Not_t<T>::value;
} // namespace Logic
} // namespace NSLibrary
