#pragma once

//--------------------------------------------------------------------------------
// Print
//--------------------------------------------------------------------------------

template<class>
struct Out;

template<class...>
struct List {};

template<auto...>
struct Seq {};

namespace NSLibrary {
namespace NSTypeContainerDetail {

template<class T>
struct ReNameImpl {
  using type = T;
};

template<template<class...> class TContainer, class... TTypes>
struct ReNameImpl<TContainer<TTypes...>> {
  using type = ::List<typename ReNameImpl<TTypes>::type...>;
};

template<class Type, template<class, Type...> class TContainer,
         Type... TElements>
struct ReNameImpl<TContainer<Type, TElements...>> {
  using type = ::Seq<TElements...>;
};

template<class T>
using ReName = typename ReNameImpl<T>::type;

template<class T>
struct PrintImpl : ::Out<NSTypeContainerDetail::ReName<T>> {};
} // namespace NSTypeContainerDetail

template<class T>
constexpr NSTypeContainerDetail::PrintImpl<T> Print;

} // namespace NSLibrary
