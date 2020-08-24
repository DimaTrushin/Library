#ifndef IMPL_TYPEHELPER_H
#define IMPL_TYPEHELPER_H

#include <type_traits>

namespace NSLibrary {
namespace NSType {

template<class>
class TD;

template<class T>
bool constexpr isArithmetic = std::is_arithmetic_v<T>;

template<class T>
using EnableIfNotRef = std::enable_if_t<!std::is_reference_v<T>>;

template<class T>
using ConstRef = std::add_lvalue_reference_t<std::add_const_t<T>>;

template<class T, class = EnableIfNotRef<T>>
using ConstRefWrapp = std::reference_wrapper<std::add_const_t<T>>;

class CIsArithmetic {};
class CIsNotArithmetic {};

template<class T>
using IsArithmeticType = std::conditional_t<isArithmetic<T>, CIsArithmetic, CIsNotArithmetic>;

} // NSType
} // NSLibrary

#endif // IMPL_TYPEHELPER_H
