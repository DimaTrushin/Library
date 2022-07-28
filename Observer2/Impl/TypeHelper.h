#ifndef IMPL_LIBRARY_TYPEHELPER_H
#define IMPL_LIBRARY_TYPEHELPER_H

#include <type_traits>

namespace NSLibrary {

template<class TData>
bool constexpr isArithmetic = std::is_arithmetic_v<TData>;

template<class TData>
bool constexpr isPointer = std::is_pointer_v<TData>;

template<class TData>
bool constexpr isEnum = std::is_enum_v<TData>;

template<class TData>
bool constexpr isSimpleClass =
    isArithmetic<TData> || isPointer<TData> || isEnum<TData>;
} // namespace NSLibrary
#endif // IMPL_LIBRARY_TYPEHELPER_H
