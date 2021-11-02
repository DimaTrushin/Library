#ifndef FUNCTIONMODULEBASE_H
#define FUNCTIONMODULEBASE_H

#include "Selector.h"

#include <utility>

namespace NSLibrary {
namespace NSFunctionModuleBaseDetail {

template<class TSignature>
struct ReturnTypeImpl;

template<class TSignature>
using ReturnType = typename ReturnTypeImpl<TSignature>::Result;

template<class R, class... TArgs>
struct ReturnTypeImpl<R(TArgs...)> {
  using Result = R;
};

template<class TSignature>
struct ArgSequenceImpl;

template<class TSignature>
using ArgSequence = typename ArgSequenceImpl<TSignature>::Result;

template<class R, class... TArgs>
struct ArgSequenceImpl<R(TArgs...)> {
  using Result = TypeSeq<TArgs...>;
};

template<class TSignature>
using ArgTypeList = TL::Seq2List<ArgSequence<TSignature>>;

template<class TSignature, class TElementList, class... TFuncs>
class FModeBaseImpl {
  using CSignature = TSignature;
  using CFunctionPtr = CSignature*;
  using LevelList = TElementList;
  using ReturnType = ReturnType<CSignature>;
  static_assert(EL::IsDecreasing<LevelList>::value,
                "Levels must go in decreasing order!");
  static_assert(EL::Length<LevelList>::value + 1 == sizeof...(TFuncs),
                "Incompatible amount of constructor arguments!");

public:
  FModeBaseImpl(TFuncs*... funcs, int level)
      : Function_(Selector::SetSignature<CSignature>::template SetLevelsT<
                  TElementList>::choose(funcs..., level)) {
  }

  template<class... TArgs>
  ReturnType operator()(TArgs&&... args) const {
    return Function_(std::forward<TArgs>(args)...);
  }

private:
  CFunctionPtr Function_;
};

template<class TSignature, class TElementList, class TSeq>
struct FModeHelperImpl;

template<class TSignature, class TElementList, class TSeq>
using FModeHelper =
    typename FModeHelperImpl<TSignature, TElementList, TSeq>::Result;

template<class TSignature, class TElementList, class... TArgs>
struct FModeHelperImpl<TSignature, TElementList, TypeSeq<TArgs...>> {
  using Result = FModeBaseImpl<TSignature, TElementList, TArgs...>;
};

template<class TSignature, class TElementList>
using FModeBase = FModeHelper<
    TSignature, TElementList,
    TL::MakeTypeSeqN<TSignature, EL::Length<TElementList>::value + 1>>;
} // namespace NSFunctionModuleBaseDetail

template<class TSignature, int... TInts>
using FunctionModuleBase =
    NSFunctionModuleBaseDetail::FModeBase<TSignature,
                                          EL::MakeElementList<TInts...>>;

} // namespace NSLibrary

#endif // FUNCTIONMODULEBASE_H
